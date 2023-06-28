#include "Axiom.h"
#include "Atom.h"
#include "Expr.h"

bool Axiom::parse(ExprPtr ex)
{
   _axiomExpr = ex;

   if (ex->kind() == Expr::_IMPL) {
      ExprOpPtr e = SPTR_DCAST(ExprOp,ex);
      parseAtoms(e->_lf, true);             // ����� �� -> � ����������
      parseAtoms(e->_rt, false);            // ������ �� -> ��� ���������
   }
   else
      parseAtoms(ex, false);                // ��� ��� ���

   return true;
}

bool Axiom::check()
{
   int id_undef = -1, num_false = 0, n_undef = 0;         // ����� �������������� �����
   for (size_t i = 0; i < _atoms.size(); i++) {
      Atom & a = _atoms[i];
      a.calc();                                           // ��������� ���������� �������� �����
      if (a._val == Logic::True)
         return true;                                     // ������������� �����������
      if (a._val != Logic::Undef && a._val != Logic::False)
         return true;                                     // ��� �� �������� ������

      if (a._val == Logic::Undef) {
         if (++n_undef > 1)                               // ����� ������ ��������������
            return true;
      }
      // �� ����� c set � ����������
      if (a._val == Logic::Undef)
         id_undef = int(i);                       // ���������� ���

      if (a._val == Logic::False)
         num_false++;
   }
   if (num_false == _atoms.size()) {              // ��� �������������� �������
      TRACE_ERROR << "! In axiom " << print() << " all atoms False:" << std::endl;
      for (auto &atom : _atoms)
      {
         TRACE_ERROR << atom.print() << " == False;" << std::endl;
      }
      return false;
   }

   if (id_undef < 0)
      return true;                                    // �� �������������

   Atom & a = _atoms[id_undef];
   a.set();                                           // ������������� ����� ��������
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
