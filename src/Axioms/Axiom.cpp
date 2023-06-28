#include "Axiom.h"
#include "Atom.h"
#include "Expr.h"

bool Axiom::parse(ExprPtr ex)
{
   _axiomExpr = ex;

   if (ex->kind() == Expr::_IMPL) {
      ExprOpPtr e = SPTR_DCAST(ExprOp,ex);
      parseAtoms(e->_lf, true);             // слева от -> с отрицанием
      parseAtoms(e->_rt, false);            // справа от -> без отрицания
   }
   else
      parseAtoms(ex, false);                // это уже КНФ

   return true;
}

bool Axiom::check()
{
   int id_undef = -1, num_false = 0, n_undef = 0;         // номер неопределённого атома
   for (size_t i = 0; i < _atoms.size(); i++) {
      Atom & a = _atoms[i];
      a.calc();                                           // вычисляем логическое значение атома
      if (a._val == Logic::True)
         return true;                                     // автоматически выполняется
      if (a._val != Logic::Undef && a._val != Logic::False)
         return true;                                     // это не трочиная логика

      if (a._val == Logic::Undef) {
         if (++n_undef > 1)                               // более одного неопределённого
            return true;
      }
      // не демон c set и неопределён
      if (a._val == Logic::Undef)
         id_undef = int(i);                       // запоминаем его

      if (a._val == Logic::False)
         num_false++;
   }
   if (num_false == _atoms.size()) {              // это противоречивая аксиома
      TRACE_ERROR << "! In axiom " << print() << " all atoms False:" << std::endl;
      for (auto &atom : _atoms)
      {
         TRACE_ERROR << atom.print() << " == False;" << std::endl;
      }
      return false;
   }

   if (id_undef < 0)
      return true;                                    // всё удовлетворяет

   Atom & a = _atoms[id_undef];
   a.set();                                           // устанавливаем новое значение
   return true;
}

string Axiom::print()
{
   stringstream str;
   if (_axiomExpr)
   {
      _axiomExpr->print(str);
   }
   return str.str();
}

void Axiom::parseAtoms(ExprPtr ex, bool _not)
{
   Atom atom;
   atom._not = _not;
   if (ex->kind() == Expr::_NOT) {
      atom._not = !_not;
      ex = SPTR_DCAST(ExprNot,ex)->_expr;
   }

   if (ex->kind() == Expr::_OR || ex->kind() == Expr::_AND) {
      ExprOpPtr e = SPTR_DCAST(ExprOp, ex);
      parseAtoms(e->_lf, _not);
      parseAtoms(e->_rt, _not);
      return;
   }
   atom._expr = ex;
   _atoms.push_back(atom);
}
