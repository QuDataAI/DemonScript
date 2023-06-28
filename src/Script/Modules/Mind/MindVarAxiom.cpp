#include "MindVarAxiom.h"
#include "MindState.h"

/****************************************************************************************
*                                        MindVarAxiom                                      *
*****************************************************************************************/
MindVarAxiom::MindVarAxiom():
   _p(1.0),
   _num_vars(0),
   _num_used(0),
   _num_checked(0),
   _group(0),
   _all_vars_ok(false),
   _notSplittedAtoms(0)
{ 
}

//! явл€етс€ ли данное выражение атомарным, т.е. X.@rel.Y, X==Y или их отрицание
//
Bool MindVarAxiom::is_atom(ExprPtr ex)
{
   if (ex->kind() == Expr::_NOT)                  // это отрицание
      ex = SPTR_DCAST(ExprNot, ex)->_expr;

   if (ex->kind() == Expr::_FUN || ex->kind() == Expr::_EQ || ex->kind() == Expr::_NEQ || ex->kind() == Expr::_EDGE)
      return true;

   return false;
}
//=======================================================================================
//! —одержит ли выражение ex только бинарные операции kind и атомы
//
Bool MindVarAxiom::is_kind(ExprPtr ex, Expr::Kind kind)
{
   if (!ex)
      return false;

   if (is_atom(ex))                               // это атом
      return true;

   if (ex->kind() != kind)                        // это не требуема€ операци€
      return false;

   ExprOpPtr  op = SPTR_DCAST(ExprOp, ex);        // провер€ем дл€ левой и правой ветки
   if (!is_kind(op->_lf, kind) || !is_kind(op->_rt, kind))
      return false;

   return true;
}
//=======================================================================================
// явл€етс€ ли выражение ex аксиомой в правильном формате: A & B & C & ... -> C | D | ...
//
Bool MindVarAxiom::is_valid(ExprPtr ex)
{
   if (!ex)
      return false;

   if (is_kind(ex, Expr::_OR))
      return true;                                // уже  Ќ‘

   if (ex->kind() == Expr::_IMPL) {               // A -> B
      ExprImplPtr e = SPTR_DCAST(ExprImpl, ex);
      if (!is_kind(e->_lf, Expr::_AND) || !is_kind(e->_rt, Expr::_OR))
         return false;
      return true;
   }
   return false;                                 // незнакома€ операци€
}
//=======================================================================================
// ѕоместить все атомы выражени€ ex в вектор _atoms
//
void MindVarAxiom::get_atoms(ExprPtr ex, Bool not_, Logic p, MindNames & names)
{
   MindVarAtom a;
   a._not = not_;
   a._p   = p;
   a._needSplit = a._p != Logic::True;
   if (a._needSplit)
      _notSplittedAtoms++;
   if (ex->kind() == Expr::_NOT) {
      a._not = !a._not;
      ex = SPTR_DCAST(ExprNot, ex)->_expr;
   }

   if (ex->kind() == Expr::_FUN) {
      ExprFunPtr e = SPTR_DCAST(ExprFun, ex);
      Int ind;
      if (e->_fun->_name == "edge") {               // это отношение
         if (e->_args.size() != 2) {
            TRACE_CRITICAL << "! MindVarAxiom::get_atoms> Wrong count args in  " << e->_fun->_name << endl;
            return;
         }
         a._edge = e->_args[0]->run().get_Int();     // номер ребра
         ind = 1;
      }
      else if (e->_fun->_name == "isa") {
         if (e->_args.size() != 1) {
            TRACE_CRITICAL << "! MindVarAxiom::get_atoms> Wrong count args in  " << e->_fun->_name << endl;
            return;
         }
         a._edge = -1;                                // означает isa
         ind = 0;
      }
      else {                                          // произвольный демон
                                                      //TRACE << "! MindVarAxiom::get_atoms> Wrong function name " << e->_fun->_name << endl;
         a._demon = SPTR_MAKE(MindDemon)();
         a._demon->_fun = SPTR_DCAST(FunDemon, e->_fun);
         for (size_t i = 0; i < e->_args.size(); i++)
            a._demon->_vars.push_back(names.get(e->_args[i]));
         _atoms.push_back(a);
         return;
      }
      get_atom(e->_obj, e->_args[ind], names, a);
      return;
   }
   else if (ex->kind() == Expr::_EDGE) {
      ExprEdgePtr e = SPTR_DCAST(ExprEdge, ex);
      a._edge = e->_edgeExpr->run().get_Int();
      if (a._edge == 0)
      {
         //что-то пошло не так
         stringstream str;
         ex->print(str);
         TRACE_CRITICAL << "! MindVarAxiom::get_atoms> Wrong edge in atom " << str.str() << endl;
         return;
      }
      if (e->_val == Logic::False)
         a._not = !a._not;
      get_atom(e->_node1Expr, e->_node2Expr, names, a);
      return;
   }
   if (ex->kind() == Expr::_EQ)
   {
      //проверим, не €вл€етс€ ли атом типа: EXPR == LOGIC либо LOGIC == EXPR
      ExprOpPtr e = SPTR_DCAST(ExprOp, ex);
      bool isLogic = false;
      ExprPtr      exEqLogic;
      ExprLogicPtr exLogic;
      if (e->_rt->kind() == Expr::_LOGIC)
      {
         exLogic = SPTR_DCAST(ExprLogic, e->_rt);
         exEqLogic = e->_lf;
         isLogic = true;
      }
      else if (e->_lf->kind() == Expr::_LOGIC)
      {
         exLogic = SPTR_DCAST(ExprLogic, e->_lf);
         exEqLogic = e->_rt;
         isLogic = true;
      }
      if (isLogic)
      {
         Logic atomP = exLogic->run().get_Logic();
         get_atoms(exEqLogic, not_, atomP, names);
         return;
      }
   }
   if (ex->kind() == Expr::_EQ || ex->kind() == Expr::_NEQ) {
      ExprOpPtr e = SPTR_DCAST(ExprOp, ex);
      a._edge = 0;                                // будет X==Y
      if (!(e->_lf->kind() == Expr::_VAR || e->_lf->kind() == Expr::_NODE_CONST)
         || !(e->_rt->kind() == Expr::_VAR || e->_rt->kind() == Expr::_NODE_CONST)) {
         TRACE_CRITICAL << "! MindVarAxiom::get_atoms> Args in L==R must be varibales or consts\n";
         return;
      }
      if (ex->kind() == Expr::_NEQ)
         a._not = !a._not;

      get_atom(e->_lf, e->_rt, names, a);
      return;
   }

   if (ex->kind() == Expr::_OR || ex->kind() == Expr::_AND) {
      ExprOpPtr e = SPTR_DCAST(ExprOp, ex);
      get_atoms(e->_lf, not_, Logic::True, names);
      get_atoms(e->_rt, not_, Logic::True, names);
   }
}
//=======================================================================================
//
void MindVarAxiom::get_atom(ExprPtr lf, ExprPtr rt, MindNames & names, MindVarAtom &a)
{
   a._x1 = names.get(lf);
   a._x2 = names.get(rt);
   _atoms.push_back(a);
}
//=======================================================================================
// ѕреобразовать выражение в список атомов _atoms ( Ќ‘)
//
Bool MindVarAxiom::set(ExprPtr ex, MindNames & names)
{
   _axiomExpr = ex;

   if (!is_valid(ex)) {
      TRACE_STREAM_CRITICAL(tout);
      tout << "! MindVarAxiom::set> Not valid axiom format: ";
      ex->print(tout); tout << endl;
      return false;
   }

   _atoms.clear();

   if (ex->kind() == Expr::_EQ)
   {
      //проверим, не €вл€етс€ ли атом типа: EXPR == LOGIC либо LOGIC == EXPR
      ExprOpPtr e = SPTR_DCAST(ExprOp, ex);
      bool isLogic = false;
      ExprPtr      exEqLogic;
      ExprLogicPtr exLogic;
      if (e->_rt->kind() == Expr::_LOGIC)
      {
         exLogic = SPTR_DCAST(ExprLogic, e->_rt);
         exEqLogic = e->_lf;
         isLogic = true;
      }
      else if (e->_lf->kind() == Expr::_LOGIC)
      {
         exLogic = SPTR_DCAST(ExprLogic, e->_lf);
         exEqLogic = e->_rt;
         isLogic = true;
      }
      if (isLogic)
      {
         Logic atomP = exLogic->run().get_Logic();
         get_atoms(exEqLogic, false, atomP, names);
         return true;
      }
   }

   if (ex->kind() == Expr::_IMPL) {
      ExprOpPtr e = SPTR_DCAST(ExprOp, ex);
      get_atoms(e->_lf, true,  Logic::True,  names);           // слева  от -> с отрицанием
      get_atoms(e->_rt, false, Logic::True, names);            // справа от -> с отрицанием
   }
   else
      get_atoms(ex, false, Logic::True, names);                // это уже  Ќ‘

   std::sort(_atoms.begin(), _atoms.end());     // упор€дочиваем атомы

   return true;
}

//=======================================================================================
// ¬ывести отладочную информацию
//
string MindVarAxiom::to_str(Graph &g, vector<UInt> &vals, MindNames & names, int id, bool val/* = true*/)
{
   string res = string("[") + to_string(_index);
   MindVarAtom &atom = _atoms[id];
   Logic printVal = val ? atom._not : !atom._not;
   if (_p != 1.0)
   {
      res += ":p:" + to_string(val ? _p : 1.0 - _p) + "";
   }
   res += string("]");
   res +=": ";
   bool wasAtom = false;
   for (size_t i = 0; i < _atoms.size(); i++)
      if (i != id) {
         if (wasAtom)
         {
            res += " & ";
         }
         wasAtom = true;
         if (_atoms[i]._edge > 0)
         {
            res += _atoms[i].to_str(g, vals, names, !_atoms[i]._not);
         }
         else
         {
            res += (!printVal) ? "!" : " ";
            res += _atoms[i].to_str(g, vals, names, true);
         }
      }
   if (_atoms.size() > 1)
      res += " -> ";

   if (_atoms[id]._edge > 0)
   {
      res += _atoms[id].to_str(g, vals, names, printVal);
   }
   else
   {
      if (!printVal)
         res += "!";
      res += _atoms[id].to_str(g, vals, names, printVal);
   }

   return res;
}
string MindVarAxiom::to_str_cnf(Graph & g, vector<UInt>& vals, MindNames & names)
{
   string res = " CNF: ";
   for (size_t i = 0; i < _atoms.size(); i++)
   {
      //res += ((_atoms[i]._not && _atoms[i]._edge > 0) ? "!" : "");
      res += "(";
      res += _atoms[i].to_str(g, vals, names, _atoms[i]._not);
      res += ")";
      res += (i < _atoms.size() - 1) ? " | " : " ";
   }
   return res;
}
//=======================================================================================
// ѕрименить аксиому с переменными vals, return -1, если было противоречие
//
Int MindVarAxiom::check_rule1(MindState &state)
{
   if (state._mode == MindState::MODE_COMPILE && state._axioms)
   {
      //тут мы собираем все аксиомы при различных комбинаци€х переменных
      state._axioms->add(state);
      return 0;
   }
   int id_undef = -1, num_false = 0, n_undef = 0;    // номер неопределЄнного атома
   Value & graphValue = state._graphValue;
   vector<UInt> &vals = state._vals;
   MindNames   &names = state._names;
   Graph *g = graphValue.get_Graph();
   for (size_t i = 0; i < _atoms.size(); i++) {
      MindVarAtom & a = _atoms[i];
      a.calc(graphValue, vals, names);            // вычисл€ем логическое значение атома
      if (state._verbose == 4) {
         TRACE << "check [" << _index << "]:" << a.to_str(*g, vals, names, false) << "=" << a._val.to_str() << std::endl;
      }
      if (a._val == Logic::True)
         return 0;                                // автоматически выполн€етс€
      if (a._val != Logic::Undef && a._val != Logic::False) {
         _all_vars_ok = false;
         return 0;                                // это не трочина€ логика
      }

      if (a._val == Logic::Undef) {
         if (++n_undef > 1) {                     // более одного неопределЄнного
            _all_vars_ok = false;

            if (state._out_undef) {                     // выводим неопределЄнное выражение:
               bool was_true = false;
               for (auto & at : _atoms) {
                  at.calc(graphValue, vals, names);
                  if (at._val == Logic::True) { was_true = true; break; }
               }
               if (!was_true) {
                  TRACE << "[" << _index << "]: ";
                  for (auto & at : _atoms)
                     if (at._val == Logic::Undef)
                        TRACE << (at._not ? "!" : "") << at.to_str(*g, vals, names, at._not) << " | ";
                  TRACE << "\n";
               }
            }
            return 0;
         }
      }
      // не демон c set и неопределЄн
      if ((a._demon == 0 || a._demon->_set) && a._val == Logic::Undef)
         id_undef = int(i);                       // запоминаем его

      if (a._val == Logic::False)
         num_false++;
   }
   if (num_false == _atoms.size()) {                  // это противоречива€ аксиома
      if (state._mode == MindState::MODE_CHECK)
         return 0;
      if (state._show_false) {
         stringstream exprStr;
         _axiomExpr->print(exprStr);
         TRACE << "! " << graphValue.get_Graph()->name() << ": In axiom [" << _index << ":" << exprStr.str() << "] all atoms False for vars: ";
         for (size_t i = 1; i <= _num_vars; i++) TRACE << ((i != 1) ? ", " : " ") << g->node_name(vals[i]);
         TRACE << to_str_cnf(*g, vals, names) << "\n";
      }
      _all_vars_ok = false;
      state.setBlock();  //дальнейший вывод заблокировн
      return -1;
   }

   if (id_undef < 0)
      return 0;                                   // всЄ удовлетвор€ет

   //добавл€ем в очередь транзакцию установки атома
   state.addTransaction(state._axiom, id_undef);

   return 1;
}

//=======================================================================================
//! ѕрименить аксиому с переменными vals, в нечЄткой логике
//
Int MindVarAxiom::check_rule2(MindState &state)
{
   Value & graphValue = state._graphValue;
   vector<UInt> &vals = state._vals;
   MindNames   &names = state._names;

   Graph *g = graphValue.get_Graph();
   for (size_t i = 0; i < _atoms.size(); i++) {
      MindVarAtom & a = _atoms[i];
      a.calc(graphValue, vals, names);                     // вычисл€ем логическое значение атома
      if (a._val == Logic::True)
         return 0;                                // выполн€етс€ автоматически
   }

   for (size_t i = 0; i < _atoms.size(); i++) {
      MindVarAtom & a = _atoms[i];
      if (a._demon || a._edge == 0)
         continue;

      Logic cond = Logic::True;
      for (size_t j = 0; j < _atoms.size(); j++)
         if (j != i) {
            cond = cond & !_atoms[j]._val;
            if (cond.p1() == 0)
               break;
         }
      if (cond.p1() == 0)
         continue;                               // не сможем сделать логический вывод

      Logic impl = Logic(0, cond.p1());          // следствие дл€ атома

                                                 // равна€ неопределЄнность:
      if (fabs((a._val.p0() + a._val.p1()) - (impl.p0() + impl.p1())) < 1e-5) {
         if (fabs(a._val.p1() - 0.5) + 1e-5 >= fabs(impl.p1() - 0.5))
            continue;
      }
      else if (a._val.p0() + a._val.p1() > impl.p0() + impl.p1())
         continue;

      Int edge = a._edge;
      if (edge < 0)
         edge = ReservedEdgeIDs::EDGE_ISA;

      UInt   n1 = a._x1 > 0 ? vals[a._x1] : names._consts[-a._x1].get_UInt();
      UInt   n2 = a._x2 > 0 ? vals[a._x2] : names._consts[-a._x2].get_UInt();
      Graph *g1 = a._x1 > 0 ? 0 : names._consts[-a._x1].get_Graph();
      Graph *g2 = a._x2 > 0 ? 0 : names._consts[-a._x2].get_Graph();

      string out;
      if (state._verbose) {                              // перед
         TRACE << "[" << _index << "] " << a.to_str(*g, vals, names, a._not) << " => ";
         out = to_str(*g, vals, names, (int)i);
      }
      a._val = impl;
      if (state._verbose)                                // после
         TRACE << (a._not ? Logic(cond.p1(), 0) : Logic(0, cond.p1())) << out << endl;

      g1->add(edge, n1, n2, a._not ? Logic(cond.p1(), 0) : Logic(0, cond.p1()), g2);

      return 1;
   }
   return 0;
}


