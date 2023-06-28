#include "Command.h"
#include "Script.h"
#include "Function.h"
#include "GraphManager.h"

//=======================================================================================

ostream & Command_edges::print(ostream & out, ScriptPtr script)
{
   out << "edges ";
   for (UInt i = 0; i < _edges.size(); i++)
      out << _edges[i] << (i + 1 < _edges.size() ? ", " : "");
   return out;
}

void Command_nodes::run(Int &ret, Value &retVal, ScriptPtr script/* = 0*/)
{
   Graph* currentGraph = _graphExpr ? _graphExpr->run().get_Graph() :  script->_graph->get_Graph();
   if (!currentGraph)
      return;

   UInt graphClassID = currentGraph->classID();
   for (UInt i = 0; i < _nodes.size(); i++)
      GraphManager::instance().addNode(graphClassID, _nodes[i]);
}

ostream & Command_nodes::print(ostream & out, ScriptPtr script)
{
   out << "nodes ";
   for (UInt i = 0; i < _nodes.size(); i++)
      out << _nodes[i] << (i + 1 < _nodes.size() ? ", " : "");
   return out;
}

//=======================================================================================
//                                Commands
//=======================================================================================
// Выполнить комманду вычисления выражения, ret=1 при return  
//
void Command_expr::run(Int &ret, Value &retVal, ScriptPtr script/* = 0*/)
{
   ret = 0;
#ifdef _DS_CHECK_POINTER
   if (!_expr) {
      TRACE_CRITICAL << "!!! Command_expr::run> _expr=0\n";
      return;
   }
#endif
   if (script->_trace) {                          // режим трассировки скрипта:
      Value res = _expr->run();
      TRACE_STREAM(tout);
      tout << "<<< "; _expr->print(tout);
      tout << "\n>>> " << res << endl;
      return;
   }
   _expr->run();
}
//=======================================================================================
// Выполнить комманду объявления переменной, ret=1 при return      
//
void Command_var::run(Int &ret, Value &retVal, ScriptPtr script/* = 0*/)
{
   ret = 0;
   if (script->_trace) {                          // режим трассировки скрипта:
      TRACE_STREAM(tout);
      tout << "<<< ";  print(tout, script);
      for (UInt i = 0; i < _exprs.size(); i++)
         if (_exprs[i]) {
            Value res = _exprs[i]->run();
            tout << ">>> " << res << "\n";
         }
      return;
   }
   else {
      for (UInt i = 0; i < _exprs.size(); i++)
         if (_exprs[i])
            _exprs[i]->run();
   }
}
//=======================================================================================
// Выполнить комманду вывода на консоль списка выражений, ret=1 при return      
//
void Command_out::run(Int &ret, Value &retVal, ScriptPtr script/* = 0*/)
{
   ret = 0;

   stringstream out;
   
   if (script->_trace) {                          // режим трассировки скрипта:
      TRACE_STREAM(tout);
      tout << "<<< ";  print(tout, script);
   }
   Value res;
   for (UInt i = 0; i < _exprs.size(); i++) {
      ExprPtr expr = _exprs[i];
      if (!expr)
         continue;
      res = expr->run();
      out << res.toString();         
   }

   if (_fout && script->_fout.is_open())
   {
      script->_fout << out.str();
      if (_carRet)
         script->_fout << "\n";
   }
   else
   {
      if (_carRet)
      {
         TRACE << out.str() << endl;
      }
      else
      {
         TRACE << out.str();
      }
   }
}
//=======================================================================================
// Выполнить комманду возврата из функции, будет ret=1
//
void Command_return::run(Int &ret, Value &retVal, ScriptPtr script/* = 0*/)
{
   if (!_expr) {
      ret = WAS_RETURN;
      return;
   }
   retVal = _expr->run().ref();
   if (_isdef && (retVal == Value(Logic())) == Logic::True)
      ret = 0;
   else
      ret = WAS_RETURN;

   if (script->_trace) {                          // режим трассировки скрипта:
      TRACE_STREAM(tout);
      tout << "<<< ";  print(tout);
      tout << ">>> " << retVal << "was return: " << ret << "\n";
   }   
}
//=======================================================================================
// Выполнить комманду break, ret=2
//
void Command_break::run(Int &ret, Value &retVal, ScriptPtr script/* = 0*/)
{
   ret = WAS_BREAK;
   if (script->_trace) {                          // режим трассировки скрипта:
      TRACE_STREAM(tout);
      tout << "<<< ";  print(tout);
      tout << ">>> was break: " << ret << "\n";
   }
}

//=======================================================================================
// Выполнить комманду continue, ret=3
//
void Command_continue::run(Int &ret, Value &retVal, ScriptPtr script/* = 0*/)
{
   ret = WAS_CONTINUE;
   if (script->_trace) {                          // режим трассировки скрипта:
      TRACE_STREAM(tout);
      tout << "<<< ";  print(tout);
      tout << ">>> was continue: " << ret << "\n";
   }
}
//=======================================================================================
// Выполнить комманду условного оператора, будет ret=1 если был return      
//
void Command_if::run(Int &ret, Value &retVal, ScriptPtr script/* = 0*/)
{
   ret = 0;
#ifdef _DS_CHECK_POINTER
   if (!_expr || !_codeIf) {
      TRACE_CRITICAL << "!!! Command_if::run> _expr=0 || _code=0\n";
      return;
   }
#endif
   Value cond = _expr->run();
   if (script->_trace) {                          // режим трассировки скрипта:
      TRACE_STREAM(tout);
      tout << "<<< if "; _expr->print(tout); tout << " :\n";
      tout << ">>> ";  cond.print(tout); tout << "\n";
   }
   Logic res = cond.get_Logic();
   if (res == Logic::True)
   {
      _num_oks++;
      _codeIf->run(ret, retVal, script);
      return;
   }
   if (res == Logic::False && _codeFalse)
   {
      _codeFalse->run(ret, retVal, script);
      return;
   }
   if (res == Logic::Undef && _codeUndef)
   {
      _codeUndef->run(ret, retVal, script);
      return;
   }
   //иначе
   if (_codeElse)
   {
      _codeElse->run(ret, retVal, script);
      return;
   }
}
//=======================================================================================
// Выполнить комманду while
//
void  Command_while::run(Int &ret, Value &retVal, ScriptPtr script/* = 0*/)
{
#ifdef _DS_CHECK_POINTER
   if (!script || !_expr || !_code) {
      TRACE_CRITICAL << "? Command_while::run> no expr, code or script pointer\n";
      return;
   }
#endif
   ret = 0; 
   if (script->_trace) {                          // режим трассировки скрипта:
      Value val = _expr->run();
      TRACE_STREAM(tout);
      tout << "<<< while "; _expr->print(tout); tout << " : \n";
   }
   while (_expr->run().is_true()) {              // ?
      _code->run(ret, retVal, script);
      if (ret == WAS_RETURN || ret == WAS_BREAK)
      {
         break;                                // сработал return или break
      }
      else if (ret == WAS_CONTINUE)
      {
         ret = WAS_NONE;                       // сработал continue
         continue;
      }
   }
   if (ret != WAS_RETURN)                      // это был не return (break или continue)
      ret = WAS_NONE;
}
//=======================================================================================
// Выполнить комманду for, ret=1 для return
// \todo ускорить для условий edge, is_edge, чтобы не переберать все узлы графа
//
void  Command_for::run(Int &ret, Value &retVal, ScriptPtr script/* = 0*/)
{
#ifdef _DS_CHECK_POINTER
   if (!script || !_var1 || !_expr || !_code) {
      TRACE_CRITICAL << "? Command_for::run> no var, expr, code or script pointer\n";
      return;
   }
#endif
   ret = 0;
   if (script->_trace) {                          // режим трассировки скрипта:
      Value val = _expr->run();
      TRACE_STREAM(tout);
      tout << "<<< for "; _var1->print(tout); tout << " in ";
      _expr->print(tout); tout << " : \n";
   }

   if (_expr->kind() == Expr::_FUN && runFuncIter(ret, retVal, script))
   {
      return;
   }

   if (_iterGraphExpr)
   {
      runGraphExprIter(ret, retVal, script);
   }
   else
   {
      runValIter(ret, retVal, script);
   }

   if (ret != WAS_RETURN)                         // это был не return (break или continue)
      ret = WAS_NONE;
}

bool Command_for::runFuncIter(Int &ret, Value &retVal, ScriptPtr script/* = 0*/)
{
   ExprFunPtr  expr = SPTR_DCAST(ExprFun,_expr);
   FunctionPtr fun  = expr->_fun;
   string funcName  = expr->_fun->name();
   ret = 0; 
   if (funcName == "range" && fun->initRun(expr->_args))
   {
      //ниже используется часть кода из функции range()
      //за исключением итерации в которой мы не тратим время на создание 
      //и заполнение массива индексов

      Float first = 0.0;
      Float last = 0.0;
      Float step = 1.0;

      if (fun->_vals.size() == 1)
      {
         last = fun->_vals[0].get_Float();
      }
      else if (fun->_vals.size() > 1)
      {
         first = fun->_vals[0].get_Float();
         last = fun->_vals[1].get_Float();
         if (fun->_vals.size() > 2)
            step = fun->_vals[2].get_Float();
      }       
      //запускаем цикл
      _var1->val() = first;
      if (_varRight)
         _varRight->val() = first;
      Float index = 0.0;
      for (Float i = first; i < last; i += step)
      {
         if (_var2)
         {
            _var1->val().set_Float(index);
            _var2->val().set_Float(i);
         }
         else
         {
            _var1->val().set_Float(i);
         }
         _code->run(ret, retVal, script);
         index += 1.0;
         if (ret == WAS_RETURN || ret == WAS_BREAK)
         {
            break;                                // сработал return или break
         }
         else if (ret == WAS_CONTINUE)
         {
            ret = WAS_NONE;                       // сработал continue
            continue;
         }
      }
      if (ret != WAS_RETURN)                         // это был не return (break или continue)
         ret = WAS_NONE;
      return true;
   }
   return false;
}
void Command_for::runGraphExprIter(Int & ret, Value & retVal, ScriptPtr script)
{
   if (_var3)
   {
      //перебираем ребра графа
      runGraphExprIterEdges(ret, retVal, script);
   }
   else
   {
      //перебираем узлы графа
      runGraphExprIterNodes(ret, retVal, script);
   }
}
void Command_for::runGraphExprIterNodes(Int & ret, Value & retVal, ScriptPtr script)
{
   Value* graphValue = GraphManager::instance().currentGraphValue();
   Graph & graph = *(graphValue->get_Graph());
   for (UInt i = 1; i < graph._nodes.size(); i++) {
      if (_var2)
      {
         _var1->val() = ValueNode(graph.classID(), i);
         _var2->val() = graph._nodes[i].value();
      }
      else
      {
         _var1->val() = ValueNode(graph.classID(), i);
      }
      if (!_expr->run().is_true())           //? условие не выполняется 
         continue;
      _code->run(ret, retVal, script);
      if (ret == WAS_RETURN || ret == WAS_BREAK)
      {
         break;                                // сработал return или break
      }
      else if (ret == WAS_CONTINUE)
      {
         ret = WAS_NONE;                       // сработал continue
         continue;
      }
   }
}

void Command_for::runGraphExprIterEdges(Int & ret, Value & retVal, ScriptPtr script)
{
   Value* graphValue = GraphManager::instance().currentGraphValue();
   Graph & graph = *(graphValue->get_Graph());
   for (UInt i = 1; i < graph._nodes.size(); i++) {
      _var1->val() = ValueNode(graph.classID(), i);
      Node &node = graph._nodes[i];
      vector<EdgeList> edgeList = node.edgesOut();
      for (auto &edgeList : edgeList)
      {
         _var2->val() = Value(edgeList._kind);
         for (auto &edge : edgeList._edges)
         {
            if (_var4)
            {
               _var4->val() = Value(edge._val);
            }
            Graph & graph2 = edge._valGraph2 ? *(edge._valGraph2->get_Graph()) : graph;
            _var3->val() = (graph == graph2) ? ValueNode(graph.classID(), edge._n2) : ValueNode(edge._valGraph2, edge._n2);
            if (!_expr->run().is_true())           //? условие не выполняется 
               continue;
            _code->run(ret, retVal, script);
            if (ret == WAS_RETURN || ret == WAS_BREAK)
            {
               break;                                // сработал return или break
            }
            else if (ret == WAS_CONTINUE)
            {
               ret = WAS_NONE;                       // сработал continue
               continue;
            }
         }
      }
   }
}

void Command_for::runValIter(Int & ret, Value & retVal, ScriptPtr script)
{
   Value iterVal = _expr->run();
   switch (iterVal.type())
   {
      case ValueBase::_ARRAY: runValIterArr(  iterVal,ret, retVal, script);  break;
      case ValueBase::_MAP:   runValIterMap(  iterVal,ret, retVal, script);  break;
      case ValueBase::_GRAPH: runValIterGraph(iterVal,ret, retVal, script);  break;
   }
}
void Command_for::runValIterArr(Value &iterVal, Int & ret, Value & retVal, ScriptPtr script)
{
   UInt size = iterVal.size();
   if (_var2)
   {
      _var1->val() = 0.0;
   }
   Value index(0.0);
   for (UInt i = 0; i < size; i++) {
      if (_var2)
      {
         _var1->val().set_Float(Float(i));
         _var2->val() = iterVal[_var1->val()];
      }
      else
      {
         index.set_Float(Float(i));
         _var1->val() = iterVal[index];
      }
      _code->run(ret, retVal, script);
      if (ret == WAS_RETURN || ret == WAS_BREAK)
      {
         break;                                // сработал return или break
      }
      else if (ret == WAS_CONTINUE)
      {
         ret = WAS_NONE;                       // сработал continue
         continue;
      }
   }
}
void Command_for::runValIterMap(Value &iterVal, Int & ret, Value & retVal, ScriptPtr script)
{
   ValueMap * map = (ValueMap*)iterVal.ptr().get();
   for (auto &i : map->_val)
   {
      if (_var2)
      {
         _var1->val() = i.first.val;
         _var2->val() = i.second;
      }
      else
      {
         _var1->val() = i.first.val;
      }
      _code->run(ret, retVal, script);
      if (ret == WAS_RETURN || ret == WAS_BREAK)
      {
         break;                                // сработал return или break
      }
      else if (ret == WAS_CONTINUE)
      {
         ret = WAS_NONE;                       // сработал continue
         continue;
      }
   }
}
void Command_for::runValIterGraph(Value &iterVal, Int & ret, Value & retVal, ScriptPtr script)
{
   if (_var3)
   {
      //перебираем ребра графа
      runValIterGraphEdges(iterVal, ret, retVal, script);
   }
   else
   {
      //перебираем узлы графа
      runValIterGraphNodes(iterVal, ret, retVal, script);
   }
}
void Command_for::runValIterGraphNodes(Value &iterVal, Int & ret, Value & retVal, ScriptPtr script)
{
   Graph * graph = iterVal.get_Graph();
   for (UInt i = 1; i < graph->_nodes.size(); i++) {
      if (_var2)
      {
         _var1->val() = ValueNode(graph->classID(), i);
         _var2->val() = graph->_nodes[i].value();
      }
      else
      {
         _var1->val() = ValueNode(graph->classID(), i);
      }
      _code->run(ret, retVal, script);
      if (ret == WAS_RETURN || ret == WAS_BREAK)
      {
         break;                                // сработал return или break
      }
      else if (ret == WAS_CONTINUE)
      {
         ret = WAS_NONE;                       // сработал continue
         continue;
      }
   }
}
void Command_for::runValIterGraphEdges(Value &iterVal, Int & ret, Value & retVal, ScriptPtr script)
{
   Graph & graph = *iterVal.get_Graph();
   for (UInt i = 1; i < graph._nodes.size(); i++) {
      _var1->val() = ValueNode(graph.classID(), i);
      Node &node = graph._nodes[i];
      vector<EdgeList> edgeList = node.edgesOut();
      for (auto &edgeList : edgeList)
      {
         _var2->val() = Value(edgeList._kind);
         for (auto &edge : edgeList._edges)
         {
            if (_var4)
            {
               _var4->val() = Value(edge._val);
            }
            Graph & graph2 = edge._valGraph2 ? *(edge._valGraph2->get_Graph()) : graph;
            _var3->val() = (graph == graph2) ? ValueNode(graph.classID(), edge._n2) : ValueNode(edge._valGraph2, edge._n2);
            _code->run(ret, retVal, script);
            if (ret == WAS_RETURN || ret == WAS_BREAK)
            {
               break;                                // сработал return или break
            }
            else if (ret == WAS_CONTINUE)
            {
               ret = WAS_NONE;                       // сработал continue
               continue;
            }
         }
      }
   }
}
//=======================================================================================
// Вывести в поток out комманду вычисления выражения в виде текста
//
ostream& Command_expr::print(ostream& out, ScriptPtr script)
{
   if (_expr)
      _expr->print(out);
   return out;
}
//=======================================================================================
// Вывести в поток out
//
ostream& Command_const::print(ostream& out, ScriptPtr script)
{
   out << "const ";
   for (UInt i = 0; i < _consts.size(); i++)
      out << _consts[i] << (i + 1 < _consts.size() ? ", " : "");
   return out;
}
//=======================================================================================
// Вывести в поток out комманду объявления переменных в виде текста
//
ostream& Command_var::print(ostream& out, ScriptPtr script)
{
   out << "var ";
   for (UInt i = 0; i < _exprs.size(); i++) {
      if (_exprs[i])
         _exprs[i]->print(out);
      if (i + 1 < _exprs.size()) out << ", ";
   }
   return out;
}
//=======================================================================================
// Вывести в поток out комманду вывода в виде текста
//
ostream& Command_out::print(ostream& out, ScriptPtr script)
{
   out << "out ";
   for (UInt i = 0; i < _exprs.size(); i++)
      if (_exprs[i]) {
         _exprs[i]->print(out);
         //out << '\"'; _exprs[i]->print(out); out << '\"';
         if (i + 1 < _exprs.size()) out << ", ";
      }
   return out;
}
//=======================================================================================
// Вывести в поток out комманду возврата из функции в виде текста
//
ostream& Command_return::print(ostream& out, ScriptPtr script)
{
   out << "return "; if (_isdef) out << "isdef ";
   if (_expr)
      _expr->print(out);
   return out;
}
//=======================================================================================
// Вывести в поток out комманду
//
ostream& Command_include::print(ostream& out, ScriptPtr script)
{
   return out << "#include \"" << _fname;
}
//=======================================================================================
// Вывести в поток out комманду условного оператора в виде текста
//
ostream& Command_if::print(ostream& out, ScriptPtr script)
{
   out << "if ";
   if (_expr)
      _expr->print(out);
   if (_codeIf->_commands.size() > 1)  out << " {\n"; else out << " :\n";
   if (_codeIf)
      _codeIf->print(out);
   if (_codeIf->_commands.size() > 1) {
      string shift; shift.assign(_codeIf->_shift >= 3 ? _codeIf->_shift - 3 : 0, ' ');
      out << shift << "}\n";
   }
   if (_codeElse)
   {
      if (_codeElse->_commands.size() > 1)  out << "else {\n"; else out << "else :\n";
      if (_codeElse)
         _codeElse->print(out);
      if (_codeElse->_commands.size() > 1) {
         string shift; shift.assign(_codeElse->_shift >= 3 ? _codeElse->_shift - 3 : 0, ' ');
         out << shift << "}\n";
      }
   }

   return out;
}
//=======================================================================================
// Вывести информацию о вычислениях команды if
//
ostream& Command_if::out_info(ostream& out, UInt shift)
{
   string st; st.assign(shift, ' ');                       // shift штук пробелов
   out << st << std::right << std::setw(8) << _num_oks << "    : ";
   out << "if ";
   if (_expr)
      _expr->print(out);
   out << "\n";
   if (_codeIf)
      _codeIf->out_info(out, shift + 3);
   if (_codeElse)
   {
      out << "else\n";
      _codeElse->out_info(out, shift + 3);
   }
   return out;
}
//=======================================================================================
// Вывести в поток out комманду while в виде текста
//
ostream& Command_while::print(ostream& out, ScriptPtr script)
{
   out << "while ";
   if (_expr) _expr->print(out);
   if (_code->_commands.size() > 1)  out << " : {\n"; else out << " :\n";
   if (_code)
      _code->print(out);
   if (_code->_commands.size() > 1) {
      string shift; shift.assign(_code->_shift >= 3 ? _code->_shift - 3 : 0, ' ');
      out << shift << "}";
   }
   return out;
}
//=======================================================================================
// Вывести в поток out комманду for в виде текста
//
ostream& Command_for::print(ostream& out, ScriptPtr script)
{
   out << "for ";
   if (_var1) _var1->print(out);
   if (_var2)
   {
      out << ",";
      _var2->print(out);
   }
   out << " in ";
   if (_expr) _expr->print(out);
   if (_code->_commands.size() > 1)  out << " : {\n"; else out << " :\n";
   if (_code)
      _code->print(out);
   if (_code->_commands.size() > 1) {
      string shift; shift.assign(_code->_shift >= 3 ? _code->_shift - 3 : 0, ' ');
      out << shift << "}";
   }
   return out;
}

void Command_static::run(Int & ret, Value & retVal, ScriptPtr script)
{
   ret = 0;
   if (_wasRun)
      return;

   if (script->_trace) {                          // режим трассировки скрипта:
      TRACE_STREAM(tout);
      tout << "<<< ";  print(tout, script);
      for (UInt i = 0; i < _exprs.size(); i++)
         if (_exprs[i]) {
            Value res = _exprs[i]->run();
            tout << ">>> " << res << "\n";
         }
      return;
   }
   else {
      for (UInt i = 0; i < _exprs.size(); i++)
         if (_exprs[i])
            _exprs[i]->run();
   }
   _wasRun = true;
}

ostream & Command_static::print(ostream & out, ScriptPtr script)
{
   out << "static ";
   for (UInt i = 0; i < _exprs.size(); i++) {
      if (_exprs[i])
         _exprs[i]->print(out);
      if (i + 1 < _exprs.size()) out << ", ";
   }
   return out;
}
