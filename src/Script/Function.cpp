#include <regex>
#include "Function.h"
#include "Script.h"
#include "Stack/StackFrame.h"
#include "Graph/GraphManager.h"
#include "Modules/FileModule.h"
#include "Axioms/Axiom.h"
#include "Compiler.h"
#include "Command.h"
#include <climits>


Function::Function(ScriptPtr script):
   _script(script),
   _num_calls(0),
   _minArgs(0),
   _maxArgs(UINT_MAX),
   _objRequired(false)
{

}

Function::Function(const string & name, ScriptPtr script):
   _name(name),
   _script(script),
   _num_calls(0),
   _minArgs(0),
   _maxArgs(UINT_MAX),
   _objRequired(false)
{
   
}

Bool Function::initRun(vector<ExprPtr>& args, ExprPtr obj)
{
   _num_calls++;
   eval(args, obj);

#ifdef _DS_CHECK_TYPE
   if (_vals.size() < _minArgs || _vals.size() > _maxArgs)
   {
      TRACE_CRITICAL << fullName() << ": Invalid number of arguments. \n";
      return false;
   }
   if (_objVal.type() == ValueBase::_NONE)
   {
      if (_objRequired)
      {
         TRACE_CRITICAL << fullName() << ": Object is not specified. \n";
         return false;
      }
   }
   else
   {
      bool permit = _objTypes.size() > 0 ? false : true;
      for (auto &permittedObjType : _objTypes)
      {
         if (permittedObjType == ValueBase::_ANY || permittedObjType == _objVal.type())
         {
            permit = true;
            break;
         }
      }
      if (!permit)
      {
         stringstream errStr;
         errStr << fullName() << ": Object type is incorrect (expected ";
         bool first = true;
         for (auto &permittedObjType : _objTypes)
         {
            if (!first)
               errStr << " or ";
            errStr << ValueBase::type_str(permittedObjType);
            first = false;
         }
         errStr << ")";
         TRACE_CRITICAL << errStr.str();
         return false;
      }
   }

   for (size_t argIndex = 0; argIndex<_argTypes.size(); argIndex++)
   {
      if (argIndex >= _vals.size())
         continue;

      bool permit = _argTypes[argIndex].size() > 0 ? false : true;
      for (auto &permittedArgType : _argTypes[argIndex])
      {
         if (permittedArgType == ValueBase::_ANY || permittedArgType == _vals[argIndex].type())
         {
            permit = true;
            break;
         }
      }
      if (!permit)
      {
         stringstream errStr;
         errStr << fullName() << ": Type of " << argIndex + 1 << " argument is incorrect (expected ";
         bool first = true;
         for (auto &permittedArgType : _argTypes[argIndex])
         {
            if (!first)
               errStr << " or ";
            errStr << ValueBase::type_str(permittedArgType);
            first = false;
         }
         errStr << ")";
         TRACE_CRITICAL << errStr.str();
         return false;
      }
   }
#endif
   return true;
}

void Function::eval(vector<ExprPtr>& args, ExprPtr _obj)
{
   _vals.clear();
   for (UInt i = 0; i < args.size(); i++)
      _vals.push_back(args[i]->run());
   if (_obj)
      _objVal = _obj->run();
}

//=======================================================================================
FunGraphJson::FunGraphJson(ScriptPtr script)
{
   _name = "json";
   _script = script;
   _num_calls = 0;
   _objRequired = true;                      // объект нужен обязательно
   _objTypes.push_back(ValueBase::_GRAPH);   // тип объекта только массив
}
//=======================================================================================
Value FunGraphJson::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   Graph * g = _objVal.get_Graph();
   if (!g)
   {
      TRACE_CRITICAL << "! .json: You must used defined variable of graph type\n";
      return Value();
   }

   int show_values = 1;
   int show_inverse = 0;
   if (_vals.size() > 0)
   {
      show_values = _vals[0].get_Int();
   }
   if (_vals.size() > 1)
   {
      show_inverse = _vals[1].get_Int();
   }

   //TRACE_STREAM(tout);
   std::stringstream tout;
   g->print(tout, show_inverse, show_values);
   return Value(tout.str());
}

FunIs::FunIs(ScriptPtr script)
{
   _name = "is";
   _script = script;
   _num_calls = 0;
   _objRequired = true;                      // объект нужен обязательно
   _objTypes.push_back(ValueBase::_ARRAY);   // тип объекта только массив
   _objTypes.push_back(ValueBase::_MAP);   // тип объекта только массив
   _minArgs = _maxArgs = 1;                  // разрешен только один аргумент
}

Value FunIs::run(vector<ExprPtr>& args, ExprPtr obj)
{
   if (!initRun(args, obj))
      return Value();
   
   return _objVal.is(_vals[0]);
}

Value FunFind::run(vector<ExprPtr>& args, ExprPtr obj)
{
   _num_calls++;
#ifdef _DS_CHECK_POINTER   
   if (!obj) {
      TRACE_CRITICAL << "! .find: No object for this function\n";              // также в push
      return Value();
   }
#endif      
#ifdef _DS_CHECK_TYPE
   Value arr = obj->run();
   if (arr.type() != ValueBase::_ARRAY) {
      TRACE_CRITICAL << "! .find: You must used variable of array type\n";     // выше подправь
      return Value();
   }
#endif
   if (args.size() != 1)
   {
      TRACE_CRITICAL << "! .find: Incorrect arguments count (1 expected)\n";
   }
   Value ob = obj->run();
   return Float(ob.find(args[0]->run()));
}

Value FunPush::run(vector<ExprPtr>& args, ExprPtr obj)
{
   _num_calls++;
#ifdef _DS_CHECK_POINTER   
   if (!obj) {
      TRACE_CRITICAL << "! .push: No object for this function\n";              // также в push
      return Logic();
   }
#endif      
#ifdef _DS_CHECK_TYPE
   Value arr = obj->run();
   if (arr.type() != ValueBase::_ARRAY) {
      TRACE_CRITICAL << "! .push: You must used variable of array type\n";     // выше подправь
      return Value();
   }
#endif    
   Value ob = obj->run();
   for (UInt i = 0; i < args.size(); i++)
      ob.push(args[i]->run());
   return Logic();
}

Value FunUnshift::run(vector<ExprPtr>& args, ExprPtr obj)
{
   _num_calls++;
#ifdef _DS_CHECK_POINTER   
   if (!obj) {
      TRACE_CRITICAL << "! .unshift: No object for this function\n";              // также в push
      return Logic();
   }
#endif      
#ifdef _DS_CHECK_TYPE
   Value arr = obj->run();
   if (arr.type() != ValueBase::_ARRAY) {
      TRACE_CRITICAL << "! .unshift: You must used variable of array type\n";     // выше подправь
      return Value();
   }
#endif    
   Value ob = obj->run();
   for (UInt i = 0; i < args.size(); i++)
      ob.unshift(args[i]->run());
   return Logic();
}

Value FunPop::run(vector<ExprPtr>& args, ExprPtr obj)
{
   _num_calls++;
#ifdef _DS_CHECK_POINTER   
   if (!obj) {
      TRACE_CRITICAL << "! .pop: No object for this function\n";              // также в push
      return Logic();
   }
#endif      
#ifdef _DS_CHECK_TYPE
   Value arr = obj->run();
   if (arr.type() != ValueBase::_ARRAY) {
      TRACE_CRITICAL << "! .pop: You must used variable of array type\n";     // выше подправь
      return Value();
   }
#endif      
   return arr.pop();
}

Value FunShift::run(vector<ExprPtr>& args, ExprPtr obj)
{
   _num_calls++;
#ifdef _DS_CHECK_POINTER   
   if (!obj) {
      TRACE_CRITICAL << "! .shift: No object for this function\n";              // также в push
      return Logic();
   }
#endif      
#ifdef _DS_CHECK_TYPE
   Value arr = obj->run();
   if (arr.type() != ValueBase::_ARRAY) {
      TRACE_CRITICAL << "! .shift: You must used variable of array type\n";     // выше подправь
      return Value();
   }
#endif      
   return arr.shift();
}

Value FunToString::run(vector<ExprPtr>& args, ExprPtr obj)
{
   _num_calls++;
#ifdef _DS_CHECK_POINTER   
   if (!obj) {
      TRACE_CRITICAL << "! .toString: No object for this function\n";              // также в push
      return Value();
   }
#endif      
   if (args.size() != 0)
   {
      TRACE_CRITICAL << "! .find: Incorrect arguments count (0 expected)\n";
   }
   Value ob = obj->run();
   return ob.toString();
}

FunDemon::FunDemon(ScriptPtr script):
   Function(script),
   _numCallsInStack(0)
{
   _num_calls  = 0;   
   _codeCommon = SPTR_MAKE(Code)();   
   _codeGet    = SPTR_MAKE(Code)();
   _codeSet    = SPTR_MAKE(Code)();
}

FunDemon::FunDemon(const string & name, vector<string>& vars, ScriptPtr script):
   FunDemon(script)
{
   _name = name;
   _vars = vars;                               // копируем массив
   _minArgs = _maxArgs = (UInt)vars.size();
   _codeCommon->_shift = _codeGet->_shift = _codeSet->_shift = 3;
   _codeGet->_parent = _codeCommon;
   _codeSet->_parent = _codeCommon;
   _num_calls = 0;
   _numCallsInStack = 0;
   for (UInt i = 0; i < vars.size(); i++)
   {
      _codeCommon->add_local_variable(vars[i]);
   }
   //переменная, используемая в режиме Set
   _valueVar = _codeCommon->add_local_variable("value");
}

//=======================================================================================
//                                   Function
//=======================================================================================
// Выполнение функции, возвращает результат
//
Value FunDemon::run(vector<ExprPtr> & args, ExprPtr obj)
{
   eval(args);
   if (_vals.size() != _vars.size()) {
      TRACE_CRITICAL << "? FunDemon::run> Wrong number of arguments in function " << _name << "\n";
      return Logic();
   }

   if (_script->_trace) {                         // режим трассировки скрипта:
      TRACE_STREAM(tout);
      tout << "<<< run ";  tout << _name << "(";
      for (UInt i = 0; i < _vals.size(); i++) {
         _vals[i].print(tout); tout << (i + 1 < _vars.size() ? "," : "");
      }
      tout << ")" << endl;
   }

   {
      StackFrame frame(*this);
      
      for (UInt i = 0; i < _vals.size(); i++)
         *(_codeCommon->_local_variables[i]) = _vals[i];

      return run();
   }   
}

//=======================================================================================
// Выполнение функции, возвращает результат
//
Value FunDemon::run()
{   
   _num_calls++;

   CurrentGraph currentGraph; //сохраняем значение текущего графа и восстанавливаем его при выходе из функции

   Int ret = 0;

   Value res;
   _codeCommon->run(ret, res, _script);
   if (ret)
      return res;
   if (_codeGet->_commands.size() > 0)
   {
      _codeGet->run(ret, res, _script);
      if (ret)
         return res;
   }

   return Logic();        // если не было return, возвращаем неопределённое значение
}
Value FunDemon::set(vector<ExprPtr> & args, ExprPtr _obj, const Value & v)
{
   eval(args);

   *_valueVar = v;

   if (_vals.size() != _vars.size()) {
      TRACE_CRITICAL << "? FunDemon::run> Wrong number of arguments in function " << _name << "\n";
      return Logic();
   }

   if (_script->_trace) {                         // режим трассировки скрипта:
      TRACE_STREAM(tout);
      tout << "<<< run ";  tout << _name << "(";
      for (UInt i = 0; i < _vals.size(); i++) {
         _vals[i].print(tout); tout << (i + 1 < _vars.size() ? "," : "");
      }
      tout << ")" << endl;
   }

   {
      StackFrame frame(*this);

      for (UInt i = 0; i < _vals.size(); i++)
         *(_codeCommon->_local_variables[i]) = _vals[i];

      return set();
   }
}

Value FunDemon::set()
{
   _num_calls++;

   CurrentGraph currentGraph; //сохраняем значение текущего графа и восстанавливаем его при выходе из функции

   Int ret = 0;

   Value res;
   _codeCommon->run(ret, res, _script);
   if (ret)
      return _valueVar->val();
   if (_codeSet->_commands.size() > 0)
   {
      _codeSet->run(ret, res, _script);
      if (ret)
         return _valueVar->val();
   }

   return _valueVar->val();
}
//=======================================================================================
// Вывести в поток out функцию fun в виде текста (программы)
//
ostream& FunDemon::print(ostream& out)
{
   out << _name << "(";
   for (UInt i = 0; i < _vars.size(); i++)
      out << _vars[i] << (i + 1 < _vars.size() ? "," : "");
   out << ")" << "\n{\n";
   _codeCommon->print(out, _script);
   if (_codeGet->_commands.size() > 0)
   {
      out << "get:\n";
      _codeGet->print(out, _script);
   }
   if (_codeSet->_commands.size() > 0)
   {
      out << "set:\n";
      _codeSet->print(out, _script);
   }
   return out << "}\n";
}
//=======================================================================================
// Вывести информацию о вычислениях функции
//
ostream& FunDemon::out_info(ostream& out)
{
   _codeCommon->out_info(out, 3);
   _codeGet->out_info(out, 3);
   _codeSet->out_info(out, 3);
   return out;
}

FunDemon::VarsSavePtr FunDemon::pushVars(CodePtr code)
{
   auto savedVars = SPTR_MAKE(VarsSave)();
   savedVars->pushCodeVars(code);
   return savedVars;
}

void FunDemon::popVars(VarsSavePtr savedVars)
{
   savedVars->popCodeVars();
}

bool FunEdgesCount::prepare(vector<ExprPtr>& args, ExprPtr obj, Graph* & graph, UInt & nodeID, Int & edgeID, Logic & logic)
{
   _num_calls++;                               // число вызовов        
#ifdef _DS_CHECK_POINTER   
   if (!obj) {
      TRACE_CRITICAL << "! " << _name << " No node for this function\n";
      return false;
   }
#endif
   Value valObj = obj->run();
   if (valObj.type() == ValueBase::_NODE)
   {
      //конструкция типа NODE.count_in(EDGE,VALUE), ожидаем один либо два аргумента
      if (args.size() < 1 || args.size() > 2)
      {
         TRACE_CRITICAL << "." << _name << ": Incorrect amount of parameters (1 or 2 expected)" << endl;
         return false;
      }
      Value edge = args[0]->run();
      Value val = (args.size() == 2) ? args[1]->run() : Logic(true);
      _isVal = (args.size() == 2);
      if (edge.type() != ValueBase::_EDGE || val.type() != ValueBase::_LOGIC)
      {
         TRACE_CRITICAL << "." << _name << ": Incorrect types of arguments (expected EDGE and LOGIC)" << endl;
         return false;
      }
      graph  = valObj.get_Graph();
      nodeID = valObj.get_UInt();
      edgeID = edge.get_Int();
      logic  = val.get_Logic();
      return true;
   }
   else if (valObj.type() == ValueBase::_GRAPH)
   {
      //конструкция типа GRAPH.count_in(NODE,EDGE,VALUE), ожидаем два или три аргумента
      if (args.size() < 2 || args.size() > 3)
      {
         TRACE_CRITICAL << "." << _name << ": Incorrect amount of parameters (2 or 3 expected)" << endl;
         return false;
      }
      Value node = args[0]->run();
      Value edge = args[1]->run();
      Value val  = (args.size() == 3) ? args[2]->run() : Logic(true);
      _isVal = (args.size() == 3);
      if (
          node.type() != ValueBase::_NODE ||
          edge.type() != ValueBase::_EDGE || 
          val.type()  != ValueBase::_LOGIC)
      {
         TRACE_CRITICAL << "." << _name << ": Incorrect types of arguments (expected: NODE,EDGE,LOGIC)" << endl;
         return false;
      }
      graph  = valObj.get_Graph();
      nodeID = node.get_UInt();
      edgeID = edge.get_Int();
      logic  = val.get_Logic();
      return true;
   }
   else
   {
      TRACE_CRITICAL << "." << _name << ": Incorrect type of object (expected: NODE or GRAPH)" << endl;
      return false;
   }
}

Value FunEdgesCountIn::run(vector<ExprPtr>& args, ExprPtr obj)
{
   _num_calls++;                               // число вызовов

   Graph *graph;
   UInt   node;
   Int    edge;
   Logic  val;

   if (!prepare(args, obj, graph, node, edge, val))
      return Logic();

   if (_isVal)
   {
      return Float(graph->countIn(node, edge, val));
   }
   else
   {
      return Float(graph->countIn(node, edge));
   }
}

Value FunEdgesCountOut::run(vector<ExprPtr>& args, ExprPtr obj)
{
   _num_calls++;                               // число вызовов

   Graph *graph;
   UInt   node;
   Int    edge;
   Logic  val;

   if (!prepare(args, obj, graph, node, edge, val))
      return Logic();

   if (_isVal)
   {
      return Float(graph->countOut(node, edge, val));
   }
   else
   {
      return Float(graph->countOut(node, edge));
   }

}

Value FunPath::run(vector<ExprPtr>& args, ExprPtr obj)
{
   _num_calls++;                               // число вызовов

   eval(args);
#ifdef _DS_CHECK_POINTER   
   if (!obj) {
      TRACE_CRITICAL << "! .path: No node for this function\n";
      return Logic();
   }
#endif
   Value ob = obj->run();
#ifdef _DS_CHECK_TYPE      
   if (_vals.size() < 2) {
      TRACE_CRITICAL << "! .path: Wrong number of arguments: " << _vals.size() << " (need 2)\n";
      return Logic();
   }
#endif
   Int e;
   Value node1 = ob;
   Value node2 = _vals[1];

   if (node1.type() != ValueBase::_NODE) {
      TRACE_CRITICAL << "! .path: No node for this function\n";
      return Logic();
   }

   UInt x = ob.get_UInt();
   UInt y = _vals[1].get_UInt();

   Graph* xGraph = node1.get_Graph();
   Graph* yGraph = node2.get_Graph();

   bool loop = true;
   if (_vals.size() > 2 && _vals[2].get_Logic() == Logic::False)
      loop = false;

   bool back = true;
   if (_vals.size() > 3 && _vals[3].get_Logic() == Logic::False)
      back = false;

   if (_vals[0].type() == ValueBase::_EDGE) {
      e = _vals[0].get_Int();
      //vector<Int> es; es.push_back(e);
      return xGraph->path(e, x, y, xGraph, yGraph, loop, back);
   }
   if (_vals[0].type() == ValueBase::_ARRAY) {
      vector<Int> es;
      for (UInt i = 0; i < _vals[0].size(); i++) {
         Int e = _vals[0][i]->get_Int();
         es.push_back(e);
      }
      return xGraph->path(es, x, y, xGraph, yGraph, loop, back);
   }
   return Value();
}

FunLoad::FunLoad(ScriptPtr script)
{
   _name = "load";  
   _script = script; 
   _num_calls = 0; 
   _minArgs = 1;
   _maxArgs = 2;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_GRAPH);
   _argTypes.resize(2);
   _argTypes[0].push_back(ValueBase::_STR);
   _argTypes[1].push_back(ValueBase::_FLOAT);
}

Value FunArray::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   _num_calls++;
   if (!obj)
      return "???";
   Value v = obj->run();
   if (v.type() == ValueBase::_ARRAY)
   {
      return v;
   }
   if (v.type() == ValueBase::_LOGIC)
   {
      auto res = SPTR_MAKE(ValueArr)();
      res->push(v[0].value());
      res->push(v[1].value());

      return (shared_ptr<ValueBase>)res;
   }
   if (v.type() == ValueBase::_FUSNUMBER)
   {
      auto res = SPTR_MAKE(ValueArr)();
      res->push(v[0].value());
      res->push(v[1].value());
      //if (v[1].value()->eq(v[2].value().get()).isTrue())            
      if (v[1].value()->neq(v[2].value().get()))  // такой вариант if (v[1] != v[2]) не компилится под маком
      {
         res->push(v[2].value());
      }
      res->push(v[3].value());

      return (SPTR(ValueBase))res;
   }

   shared_ptr<ValueArr> res = make_shared<ValueArr>();
   res->push(v.ptr());

   return (SPTR(ValueBase))res;
}

Value FunLoad::run(vector<ExprPtr>& args, ExprPtr obj)
{
   if (!initRun(args, obj))
      return Value();

   Graph* graph = _objVal.get_Graph();
   if (!graph)
      return Logic();

   string fileName = _vals[0].get_Str();
   Int    openMode = 0;
   if (_vals.size() > 1)
      openMode = (Int)_vals[1].get_Float();
   fileName = _script->workPath(fileName);

   return graph->load(fileName, openMode);
}

Value FunAddNodes::run(vector<ExprPtr>& args, ExprPtr obj)
{
   _num_calls++;
   eval(args);
#ifdef _DS_CHECK_TYPE      
   if (_vals.size() != 1) {
      TRACE_CRITICAL << "! .add_nodes: Wrong number of arguments: " << _vals.size() << " (need 1)\n";
      return Graph();
   }
   if (!obj) {
      TRACE_CRITICAL << "! .add_nodes: object is not specified (need GRAPH)\n";
      return Graph();
   }
#endif

   Value ob = obj->run();
   if (ob.type() != ValueBase::_GRAPH)
   {
      TRACE_CRITICAL << "! .add_nodes: object type is incorrect (need GRAPH)\n";
      return Graph();
   }

   Graph* graph = ob.get_Graph();

   if (!graph)
   {
      TRACE_CRITICAL << "! .add_nodes: object type is incorrect (need GRAPH)\n";
      return Graph();
   }

   if (_vals[0].type() != ValueBase::_FLOAT)
   {
      TRACE_CRITICAL << "! .add_nodes: argument type is incorrect (need FLOAT)\n";
      return Logic();
   }

   GraphManager::instance().addNodes(graph->classID(), _vals[0].get_UInt());

   return ob;
}

//Value FunNodes::run(vector<ExprPtr>& args, ExprPtr obj)
//{
//   _num_calls++;
//   eval(args);
//#ifdef _DS_CHECK_TYPE      
//   if (_vals.size() != 0) {
//      TRACE_CRITICAL << "! .nodes: Wrong number of arguments: " << _vals.size() << " (need 0)\n";
//      return Graph();
//   }
//   if (!obj) {
//      TRACE_CRITICAL << "! .nodes: object is not specified (need GRAPH)\n";
//      return Graph();
//   }
//#endif
//
//   Value ob = obj->run();
//   if (ob.type() != ValueBase::_GRAPH)
//   {
//      TRACE_CRITICAL << "! .nodes: object type is incorrect (need GRAPH)\n";
//      return Graph();
//   }
//
//   Graph* graph = ob.get_Graph();
//
//   if (!graph)
//   {
//      TRACE_CRITICAL << "! .nodes: object type is incorrect (need GRAPH)\n";
//      return Graph();
//   }
//
//   UInt nodes = GraphManager::instance().nodes(graph->classID());
//
//   return Value(Float(nodes));
//}

Value FunEdgesNum::run(vector<ExprPtr>& args, ExprPtr obj)
{
   _num_calls++;
   eval(args);
#ifdef _DS_CHECK_TYPE      
   if (_vals.size() != 0) {
      TRACE_CRITICAL << "! .edges: Wrong number of arguments: " << _vals.size() << " (need 0)\n";
      return Graph();
   }
   if (!obj) {
      TRACE_CRITICAL << "! .edges: object is not specified (need GRAPH)\n";
      return Graph();
   }
#endif

   Value ob = obj->run();
   if (ob.type() != ValueBase::_GRAPH)
   {
      TRACE_CRITICAL << "! .edges: object type is incorrect (need GRAPH)\n";
      return Graph();
   }

   Graph* graph = ob.get_Graph();

   if (!graph)
   {
      TRACE_CRITICAL << "! .edges: object type is incorrect (need GRAPH)\n";
      return Graph();
   }

   UInt edges = GraphManager::instance().edges();

   return Value(Float(edges));
}

Value FunValue::run(vector<ExprPtr>& args, ExprPtr obj)
{
   _num_calls++;
   if (args.size() > 2)
   {
      TRACE_CRITICAL << "! .value: incorrect number of arguments (need less than two)\n";
      return Value();
   }
   if (!obj)
   {
      TRACE_CRITICAL << "! .value: object is not specified (need NODE)\n";
      return Value();
   }
   Value ob = obj->run();
   if (ob.type()!= ValueBase::_NODE)
   {
      TRACE_CRITICAL << "! .value: object type is incorrect (need NODE)\n";
      return Value();
   }

   eval(args);
   Graph* graph = ob.get_Graph();
   if (args.size() == 0)
   {
      //it is getter function
      return Value(graph->nodeValue(ob.get_UInt()));
   }
   else
   {
      //it is setter function
      graph->nodeValue(ob.get_UInt(), _vals[0].ptr());
      return _vals[0];
   }

   return Value();
}

Value FunClassID::run(vector<ExprPtr>& args, ExprPtr obj)
{
   if (obj)
   {
      Value ob = obj->run();
      if (ob.type() != ValueBase::_GRAPH)
      {
         TRACE_CRITICAL << "! .class_id: object type is incorrect (need GRAPH)\n";
         return Value();
      }
      Graph* graph = ob.get_Graph();
      return Value(Float(graph->classID()));
   }
   else
      return Value(Float(GraphManager::instance().currentGraphClass()));
}

Value FunGraph::run(vector<ExprPtr>& args, ExprPtr obj)
{
   eval(args);
   if (!obj)
   {
      TRACE_CRITICAL << "! .graph: object is not specified (need NODE)\n";
      return Value();
   }
   if (args.size() > 0)
   {
      TRACE_CRITICAL << "! .graph: incorrect number of arguments (need 0)\n";
      return Value();
   }

   Value ob = obj->run();

   if (ob.type() != ValueBase::_NODE)
   {
      TRACE_CRITICAL << "! .graph: object type is incorrect (need NODE)\n";
      return Value();
   }

   ValueNode* node = (ValueNode*)ob.ptr().get();

   return Value(node->get_GraphValue());
}

FunRange::FunRange(ScriptPtr script)
{
   _name = "range";  
   _script = script; 
   _num_calls = 0;
   _minArgs = 1;
   _maxArgs = 3;

   //все три аргумента должны быть типа float
   _argTypes.resize(3);
   _argTypes[0].push_back(ValueBase::_FLOAT);
   _argTypes[1].push_back(ValueBase::_FLOAT);
   _argTypes[2].push_back(ValueBase::_FLOAT);
}

Value FunRange::run(vector<ExprPtr>& args, ExprPtr obj)
{
   if (!initRun(args, obj))
      return Value();

   Float first = 0.0;
   Float last  = 0.0;
   Float step  = 1.0;

   if (_vals.size() == 1)
   {
      last = _vals[0].get_Float();
   }
   else if (_vals.size() > 1)
   {
      first = _vals[0].get_Float();
      last  = _vals[1].get_Float();
      if (_vals.size() > 2)
         step = _vals[2].get_Float();
   }

   Value arr = Value(ValueArr());

   for (Float i = first; i < last; i += step)
      arr.push(Value(i));

   return arr;
}

FunId::FunId(ScriptPtr script)
{
   _name = "id";
   _script = script;
   _num_calls = 0;
   _minArgs = 0;
   _maxArgs = 0;
}

Value FunId::run(vector<ExprPtr>& args, ExprPtr obj)
{
   if (!initRun(args, obj))
      return Value();

   return _objVal.id();
}

FunAddNode::FunAddNode(ScriptPtr script) 
{
   _name = "add_node";  
   _script = script; 
   _num_calls = 0;
   _minArgs = 0;
   _maxArgs = 1;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_GRAPH);
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_STR);
}

//=======================================================================================
// Добавление одного безымянного узла в граф, возвращает его id  
//
Value FunAddNode::run(vector<ExprPtr>& args, ExprPtr obj)
{
   if (!initRun(args, obj))
      return Value();

   Graph* graph = _objVal.get_Graph();

   if (!graph)
   {
      TRACE_CRITICAL << "! .add_node: object type is incorrect (need GRAPH)\n";
      return Graph();
   }

   if (_vals.size() == 0)
   {
      UInt id = GraphManager::instance().addNodes(graph->classID(), 1);
      return ValueNode(_objVal.ptr(), id);
   }
   else
   {
      UInt id = GraphManager::instance().addNode(graph->classID(), _vals[0].get_Str());
      return ValueNode(graph->classID(), id);
   }

   return Value();
}

FunSplice::FunSplice(ScriptPtr  script)
{
   _name = "splice";
   _script = script;
   _num_calls = 0;
   _minArgs = 1;
   _maxArgs = 2;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_ARRAY);
   _argTypes.resize(2);
   _argTypes[0].push_back(ValueBase::_FLOAT);
   _argTypes[1].push_back(ValueBase::_FLOAT);
}

Value FunSplice::run(vector<ExprPtr>& args, ExprPtr obj)
{
   if (!initRun(args, obj))
      return Value();

   Int start       = _vals[0].get_Int();
   Int deleteCount = 0;

   if (_vals.size() > 1)
   {
      deleteCount = _vals[1].get_Int();
   }
   else
   {
      deleteCount = (Int)_objVal.size() - start;
   }

   return _objVal.splice(start, deleteCount);
}

Value FunVerbose::run(vector<ExprPtr>& args, ExprPtr obj)
{
   _num_calls++;
   eval(args);

   Value ob = obj->run();
   if (ob.type() != ValueBase::_GRAPH)
   {
      TRACE_CRITICAL << "! .verbose: object type is incorrect (need GRAPH)\n";
      return Graph();
   }

   Graph* graph = ob.get_Graph();

   if (!graph)
   {
      TRACE_CRITICAL << "! .verbose: object type is incorrect (need GRAPH)\n";
      return Graph();
   }

   if (_vals[0].type() != ValueBase::_FLOAT)
   {
      TRACE_CRITICAL << "! .verbose: argument type is incorrect (need FLOAT)\n";
      return Logic();
   }

   graph->verbose(_vals[0].get_Int());

   return ob;
}

Value FunLocked::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   if (_objVal.type() != ValueBase::_GRAPH)
   {
      TRACE_CRITICAL << "! .locked: object type is incorrect (need GRAPH)\n";
      return Value();
   }

   Graph* graph = _objVal.get_Graph();

   if (_vals.size() > 0)
   {
      graph->locked(_vals[0].get_Int());
   }

   return Value(graph->locked());
}

Value FunSorted::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   if (_objVal.type() != ValueBase::_GRAPH)
   {
      TRACE_CRITICAL << "! .sorted: object type is incorrect (need GRAPH)\n";
      return Value();
   }

   Graph* graph = _objVal.get_Graph();

   if (_vals.size() > 0)
   {
      graph->autoSort(_vals[0].get_Int());
   }

   return Value(graph->autoSort());
}

Value FunGetLine::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   if (_objVal.type() != ValueBase::_FILE)
   {
      TRACE_CRITICAL << "! .getline: object type is incorrect (need FILE)\n";
      return Value();
   }

   int fileId = _objVal.get_Int();
   FileModule* module = FileModule::instance();

   return Value(module->getline(fileId));
}

Value FunGetLines::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   if (_objVal.type() != ValueBase::_FILE)
   {
      TRACE_CRITICAL << "! .getlines: object type is incorrect (need FILE)\n";
      return Value();
   }

   int fileId = _objVal.get_Int();
   FileModule* module = FileModule::instance();

   vector<string> lines;
   module->getlines(fileId, lines);

   vector<shared_ptr<ValueBase>> linesValues;
   for (size_t i = 0; i < lines.size(); i++)
      linesValues.push_back(Value(lines[i]).ptr());

   return ValueArr(linesValues);
}

Value FunIn::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   if (_objVal.type() != ValueBase::_FILE)
   {
      TRACE_CRITICAL << "! .in: object type is incorrect (need FILE)\n";
      return Value();
   }

   int fileId = _objVal.get_Int();
   FileModule* module = FileModule::instance();

   return Value(module->in(fileId));
}

Value FunEOF::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   if (_objVal.type() != ValueBase::_FILE)
   {
      TRACE_CRITICAL << "! .eof: object type is incorrect (need FILE)\n";
      return Value();
   }

   int fileId = _objVal.get_Int();
   FileModule* module = FileModule::instance();

   return Value(Logic(module->eof(fileId)));
}

Value FunIsOpen::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   if (_objVal.type() != ValueBase::_FILE)
   {
      TRACE_CRITICAL << "! .is_open: object type is incorrect (need FILE)\n";
      return Value();
   }

   int fileId = _objVal.get_Int();
   FileModule* module = FileModule::instance();

   return Value(Logic(module->is_open(fileId)));
}

Value FunOut::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   if (_objVal.type() != ValueBase::_FILE)
   {
      TRACE_CRITICAL << "! .out: object type is incorrect (need FILE)\n";
      return Value();
   }

   int fileId = _objVal.get_Int();
   FileModule* module = FileModule::instance();

   for (size_t i = 0; i < _vals.size(); i++)
   {
      module->out(fileId, _vals[i].toString());
      module->out(fileId, i < _vals.size() - 1 ? " " : "\n");
   }

   return _objVal;
}

Value FunClose::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   if (_objVal.type() == ValueBase::_FILE)
   {
      int fileId = _objVal.get_Int();
      FileModule* module = FileModule::instance();

      module->close(fileId);

      return _objVal;
   }
   else if (_objVal.type() == ValueBase::_GRAPH)
   {
      if (_vals.size() > 0)
      {
         //_objVal.get_Graph()->close(_vals[0].is_true());
         GraphManager::instance().close(_objVal.get_Graph()->classID(), _vals[0].get_Logic().isTrue());
      }

      return _objVal.get_Graph()->close() ? Logic::True : Logic::False;
   }

   TRACE_CRITICAL << "! .close: object type is incorrect (need FILE or GRAPH)\n";
   return Value();
}

Value FunSplit::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   if (_objVal.type() != ValueBase::_STR)
   {
      TRACE_CRITICAL << "! .split: object type is incorrect (need STR)\n";
      return Value();
   }

   vector<string> lines;

   const string s(_objVal.get_Str());

   string separators = " ";
   if (_vals.size() > 0)
   {
      separators = _vals[0].get_Str();
   }

   size_t maxCount = 0;
   if (_vals.size() > 1)
   {
      maxCount = _vals[1].get_UInt();
   }

   const regex separator(separators);
   const sregex_token_iterator endOfSequence;

   sregex_token_iterator token(s.begin(), s.end(), separator, -1);
   while (token != endOfSequence)
   {
      lines.push_back(*token++);
      if (maxCount > 0 && lines.size() >= maxCount)
         break;
   }   

   vector<shared_ptr<ValueBase>> linesValues;
   for (size_t i = 0; i < lines.size(); i++)
      linesValues.push_back(Value(lines[i]).ptr());

   return ValueArr(linesValues);
}

Value FunIndexOf::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   if (_objVal.type() != ValueBase::_STR)
   {
      TRACE_CRITICAL << "! .indexOf: object type is incorrect (need STR)\n";
      return Value();
   }

   if (_vals.size() == 0 || _vals[0].type() != ValueBase::_STR)
   {
      TRACE_CRITICAL << "! .indexOf: 1st argument type is incorrect (need STR)\n";
      return Value();
   }

   size_t firstPos = 0;

   if (_vals.size() > 1)
   {
      firstPos = _vals[1].get_UInt();
   }

   size_t res = _objVal.get_Str().find(_vals[0].get_Str(), firstPos);

   if (res == string::npos)
   {
      return Value(-1);
   }

   return Value(res);
}

Value FunTrim::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   if (_objVal.type() != ValueBase::_STR)
   {
      TRACE_CRITICAL << "! .trim: object type is incorrect (need STR)\n";
      return Value();
   }

   string res = _objVal.get_Str();

   static const std::string spaces(" \t\n");
   size_t head = res.find_first_not_of(spaces);
   if (head == std::string::npos)
      return Value("");
   else if (head > 0)
      res.erase(0, head);
   size_t tail = res.find_last_not_of(spaces);
   if (tail != res.size() - 1)
      res.erase(tail + 1);

   return Value(res);
}

Value FunLength::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   if (_objVal.type() != ValueBase::_STR)
   {
      TRACE_CRITICAL << "! .length: object type is incorrect (need STR)\n";
      return Value();
   }

   return Value((Float)_objVal.get_Str().length());
}

Value FunSubstr::run(vector<ExprPtr> & args, ExprPtr obj/* = 0*/)
{
   if (!initRun(args, obj))
      return Value();

   if (_objVal.type() != ValueBase::_STR)
   {
      TRACE_CRITICAL << "! .substr: object type is incorrect (need STR)\n";
      return Value();
   }
   size_t start = 0;
   size_t cnt = _objVal.get_Str().length();

   if (_vals.size() > 0)
   {
      start = _vals[0].get_UInt();
   }
   if (_vals.size() > 1)
   {
      cnt = _vals[1].get_UInt();
   }

   return Value(_objVal.get_Str().substr(start, cnt));
}

Value FunSubstring::run(vector<ExprPtr> & args, ExprPtr obj/* = 0*/)
{
   if (!initRun(args, obj))
      return Value();

   if (_objVal.type() != ValueBase::_STR)
   {
      TRACE_CRITICAL << "! .substring: object type is incorrect (need STR)\n";
      return Value();
   }
   size_t start = 0;
   size_t cnt = _objVal.get_Str().length();

   if (_vals.size() > 0)
   {
      start = _vals[0].get_UInt();
   }
   if (_vals.size() > 1)
   {
      cnt = _vals[1].get_UInt() - _vals[0].get_UInt();
   }

   return Value(_objVal.get_Str().substr(start, cnt));
}

FunReplace::FunReplace(ScriptPtr script) 
{ 
   _name = "replace";  
   _script = script; 
   _num_calls = 0; 

   _minArgs = 2;
   _maxArgs = 3;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_STR);
   _argTypes.resize(3);
   _argTypes[0].push_back(ValueBase::_STR);
   _argTypes[1].push_back(ValueBase::_STR);
   _argTypes[2].push_back(ValueBase::_FLOAT);
}

Value FunReplace::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   int num = 1;
   if (_vals.size() > 2)
   {
      num = _vals[2].get_Int();
   }

   string str = _objVal.get_Str();
   size_t start_pos = 0;

   while ((start_pos = str.find(_vals[0].get_Str(), start_pos)) != std::string::npos && num != 0)
   {
      str.replace(start_pos, _vals[0].get_Str().length(), _vals[1].get_Str());
      start_pos += _vals[1].get_Str().length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
      num--;
   }
   return Value(str);
}

FunReplaceRegex::FunReplaceRegex(ScriptPtr script)
{
   _name = "replaceRegexp";
   _script = script;
   _num_calls = 0;

   _minArgs = 2;
   _maxArgs = 3;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_STR);
   _argTypes.resize(2);
   _argTypes[0].push_back(ValueBase::_STR);
   _argTypes[1].push_back(ValueBase::_STR);
}

Value FunReplaceRegex::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   string str = _objVal.get_Str();
   regex r(_vals[0].get_Str());

   str = std::regex_replace(str, r, _vals[1].get_Str());

   return Value(str);
}

FunRepeat::FunRepeat(ScriptPtr script)
{
   _name = "repeat";
   _script = script;
   _num_calls = 0;

   _minArgs = 1;
   _maxArgs = 1;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_STR);
   _argTypes.resize(2);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}

Value FunRepeat::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   string str = ""; 
   int cnt = _vals[0].get_Int();

   for (int i = 0; i < cnt; i++)
      str += _objVal.get_Str();

   return Value(str);
}

FunInclude::FunInclude(ScriptPtr script)
{
   _name = "include";
   _script = script;
   _num_calls = 0;

   _minArgs = 1;
   _maxArgs = 1;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_STR);
   _argTypes.resize(2);
   _argTypes[0].push_back(ValueBase::_STR);
}

Value FunInclude::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   if (_objVal.get_Str().find(_vals[0].get_Str(), 0) != std::string::npos)
      return Logic::True;

   return Logic::False;
}

FunStartsWith::FunStartsWith(ScriptPtr script)
{
   _name = "startsWith";
   _script = script;
   _num_calls = 0;

   _minArgs = 1;
   _maxArgs = 1;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_STR);
   _argTypes.resize(2);
   _argTypes[0].push_back(ValueBase::_STR);
}

Value FunStartsWith::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   if (_objVal.get_Str().find(_vals[0].get_Str(), 0) == 0)
      return Logic::True;

   return Logic::False;
}

FunEndsWith::FunEndsWith(ScriptPtr script)
{
   _name = "endsWith";
   _script = script;
   _num_calls = 0;

   _minArgs = 1;
   _maxArgs = 1;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_STR);
   _argTypes.resize(2);
   _argTypes[0].push_back(ValueBase::_STR);
}

Value FunEndsWith::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   if (_objVal.get_Str().find(_vals[0].get_Str(), 0) == _objVal.get_Str().length() - _vals[0].get_Str().length())
      return Logic::True;

   return Logic::False;
}

FunSearch::FunSearch(ScriptPtr script)
{
   _name = "search";
   _script = script;
   _num_calls = 0;

   _minArgs = 1;
   _maxArgs = 1;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_STR);
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_STR);
}

Value FunSearch::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   size_t start = 0;
   if (_vals.size() > 1)
      start = _vals[1].get_UInt();

   string s = _objVal.get_Str();
   smatch m;
   regex e(_vals[0].get_Str());

   regex_search(s, m, e);

   if (m.size() == 0)
      return -1;

   return ((float)m.position(0));
}

void FunDemon::VarsSave::pushCodeVars(CodePtr code)
{
   if (code->_local_variables.size() > 0)
   {
      _codesVars.push_back(CodeVars());
      CodeVars & codeVars = _codesVars.back();
      codeVars._vars = &code->_local_variables;
      for (auto var : code->_local_variables)
      {
         codeVars._vals.push_back(var->_val);
      }
   }
   if (!code->_parent)
      return;

   pushCodeVars(code->_parent);
}

void FunDemon::VarsSave::popCodeVars()
{
   for (auto &codeVar : _codesVars)
   {
      vector<VariablePtr>* vars = codeVar._vars;
      vector<Value>        vals = codeVar._vals;

      for (int i = 0; i < vals.size(); i++)
      {
         (*vars)[i]->_val = vals[i];
      }
   }
}

FunFloat::FunFloat(ScriptPtr  script)
{
   _name = "float";  
   _script = script; 
   _num_calls = 0; 
   _minArgs = 0;
   _maxArgs = 0;
   _objRequired = true;
}

Value FunFloat::run(vector<ExprPtr>& args, ExprPtr obj)
{
   if (!initRun(args, obj))
      return Value();

   return _objVal.toFloat();
}

FunAttr::FunAttr(ScriptPtr  script)
{
   _name = "attr";  
   _script = script; 
   _num_calls = 0;
   _minArgs = 0;
   _maxArgs = 1;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_NODE);
   _objTypes.push_back(ValueBase::_GRAPH);
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_NODE);
}

Value FunAttr::run(vector<ExprPtr>& args, ExprPtr obj)
{
   if (!initRun(args, obj))
      return Value();

   if (_vals.size() == 0)
   {
      if (_objVal.type() == ValueBase::_GRAPH)
      {
         return (shared_ptr<ValueBase>)_objVal.get_Graph()->attrGraphValue();
      }
      else if (_objVal.type() == ValueBase::_NODE)
      {
         shared_ptr<ValueArr> attrNodesVal = make_shared<ValueArr>();
         _objVal.get_Graph()->attr_list(_objVal.get_UInt(), false, *attrNodesVal.get());
         return (shared_ptr<ValueBase>)attrNodesVal;
      }
   }
   else
   {
      return _objVal.get_Graph()->get_attr(_objVal.get_UInt(), _vals[0].get_UInt(), _vals[0].get_Graph(), false);
   }
   
   return Value();
}

FunNodes::FunNodes(ScriptPtr  script)
{
   _name = "nodes";
   _script = script;
   _num_calls = 0;
   _minArgs = 0;
   _maxArgs = 2;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_NODE);
   _objTypes.push_back(ValueBase::_GRAPH);
   _argTypes.resize(2);
   _argTypes[0].push_back(ValueBase::_EDGE);
   _argTypes[1].push_back(ValueBase::_ANY);
}

Value FunNodes::run(vector<ExprPtr>& args, ExprPtr obj)
{
   if (!initRun(args, obj))
      return Value();

   if (_objVal.type() == ValueBase::_NODE)
   {
      if (_vals.size() < 1)
      {
         TRACE_CRITICAL << "? FunNodes::run> Wrong number of arguments in function: need at least one\n";
         return Value();
      }

      Logic val = Logic::Undef;

      if (_vals.size() > 1)
      {
         val = _vals[1].get_Logic();
      }

      shared_ptr<ValueArr> attrNodesVal = make_shared<ValueArr>();
      _objVal.get_Graph()->node_list(_objVal.get_UInt(), _vals[0].get_Int(), *attrNodesVal.get(), val);
      attrNodesVal->sort();

      return (shared_ptr<ValueBase>)attrNodesVal;
   }
   else
   {
      shared_ptr<ValueArr> res = make_shared<ValueArr>();      

      for (size_t i = 1; i < _objVal.get_Graph()->_nodes.size(); i++)
         res->push(make_shared<ValueNode>(_objVal.get_Graph()->classID(), _objVal.get_Graph()->_nodes[i]._id));

      return (shared_ptr<ValueBase>)res;
   }
}

FunGet::FunGet(ScriptPtr  script)
{
   _name = "get";
   _script = script;
   _num_calls = 0;
   _minArgs = 0;
   _maxArgs = 2;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_NODE);
   _argTypes.resize(2);
   _argTypes[0].push_back(ValueBase::_EDGE);
   //_argTypes[0].push_back(ValueBase::_NODE);
   _argTypes[1].push_back(ValueBase::_NODE);
}

Value FunGet::run(vector<ExprPtr>& args, ExprPtr obj)
{
   if (!initRun(args, obj))
      return Value();

   if (_vals.size() == 2)
   {
      if (_vals[0].type() != ValueBase::_EDGE || _vals[1].type() != ValueBase::_NODE)
      {
         TRACE_CRITICAL << "! .get: incorrect parameters. Try get(edge, node)\n";
         return Value();
      }
      return _objVal.get_Graph()->get(_vals[0].get_Int(), _objVal.get_UInt(), _vals[1].get_UInt(), _objVal.get_Graph(), _vals[1].get_Graph());
   }
   else if (_vals.size() == 1)
   {
      if (_vals[0].type() == ValueBase::_EDGE)
      { 
         shared_ptr<ValueArr> out = make_shared<ValueArr>();
         _objVal.get_Graph()->getAllHelper(_vals[0].get_Int(), _objVal.get_UInt(), _objVal.get_Graph(), out);
         return (shared_ptr<ValueBase>)out;
      }      
      //else if (_vals[0].type() == ValueBase::_NODE)
      //{
      //   shared_ptr<ValueArr> out = make_shared<ValueArr>();
      //   for (int i = ReservedEdgeIDs::EDGE_HAS; i < ReservedEdgeIDs::EDGE_RESERVED_MAX; i++)
      //   {
      //      _objVal.get_Graph()->getAllHelper(_vals[0].get_Int(), _objVal.get_UInt(), _objVal.get_Graph(), out);
      //   }
      //}

      TRACE_CRITICAL << "! .get: incorrect parameter. Try get(edge) \n";
   }

   Value res = ValueManager::createValue(ValueMap());
   for (int i = ReservedEdgeIDs::EDGE_HAS; i < ReservedEdgeIDs::EDGE_RESERVED_MAX; i++)
   {
      shared_ptr<ValueArr> out = make_shared<ValueArr>();      
      _objVal.get_Graph()->getAllHelper(i, _objVal.get_UInt(), _objVal.get_Graph(), out);
      res[Value(i)] = Value(out);
   }

   return res;
}

FunEdges::FunEdges(ScriptPtr  script)
{
   _name = "edges";
   _script = script;
   _num_calls = 0;
   _minArgs = 0;
   _maxArgs = 1;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_GRAPH);
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_LOGIC);
}

Value FunEdges::run(vector<ExprPtr>& args, ExprPtr obj)
{
   if (!initRun(args, obj))
      return Value();

   Logic val = Logic::Undef;

   if (_vals.size() > 0)
   {
      val = _vals[0].get_Logic();
   }

   shared_ptr<ValueArr> res = make_shared<ValueArr>();
   
   for (auto it : GraphManager::instance().edgeNames())
   {
      if (it.second == 0)
         continue;

      if (it.second < ReservedEdgeIDs::EDGE_RESERVED_MAX && val.isFalse())
         continue;

      res->push(make_shared<ValueEdge>(it.second));
   }

   return (shared_ptr<ValueBase>)res;
}

FunBind::FunBind(ScriptPtr  script)
{
   _name = "bind";
   _script = script;
   _num_calls = 0;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_NODE);
}

Value FunBind::run(vector<ExprPtr>& args, ExprPtr obj)
{
   if (!initRun(args, obj))
      return Value();

   ValueNode * node = (ValueNode *)(_objVal.ptr().get());

   node->bind();

   return Value(*node);
}

FunUnbind::FunUnbind(ScriptPtr  script)
{
   _name = "unbind";
   _script = script;
   _num_calls = 0;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_NODE);
}

Value FunUnbind::run(vector<ExprPtr>& args, ExprPtr obj)
{
   if (!initRun(args, obj))
      return Value();

   ValueNode * node = (ValueNode *)(_objVal.ptr().get());

   node->unbind();

   return Value(*node);
}

FunSort::FunSort(ScriptPtr script)
{
   _name = "sort";
   _script = script;
   _num_calls = 0;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_ARRAY);
   _objTypes.push_back(ValueBase::_GRAPH);
}

Value FunSort::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   FunLambdaPtr lambdaFn;

   if (_vals.size() > 0)
   {
      lambdaFn = _vals[0].get_Lambda();
   }

   if (_objVal.type() == ValueBase::_ARRAY)
   {
      ValueArr * arr = (ValueArr *)(_objVal.ptr().get());
      arr->sort(lambdaFn);
      return Value(*arr);
   }
   else if (_objVal.type() == ValueBase::_GRAPH)
   {
      _objVal.get_Graph()->sort();
   }   
   return Value();
}

Value FunDeleteNodes::run(vector<ExprPtr>& args, ExprPtr obj)
{
   _num_calls++;                               // число вызовов      
   UInt classID = 0;
   if (!obj) {
      classID = GraphManager::instance().currentGraphClass();
   }
   else
   {
      Graph * g = obj->run().get_Graph();
      if (!g)
         return "! .delete_nodes: You must used defined variable of graph type\n";
      classID = g->classID();
   }   
   GraphManager::instance().deleteNodes(classID);
   return Value();
}

FunIsomorphic::FunIsomorphic(ScriptPtr script/* = 0*/)
{
   _name = "isomorphic";
   _script = script;
   _num_calls = 0;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_GRAPH);
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_GRAPH);
}

Value FunIsomorphic::run(vector<ExprPtr>& args, ExprPtr obj)
{
   if (!initRun(args, obj))
      return Value();

   _num_calls++;                               // число вызовов 

   Graph* g1 = obj->run().get_Graph();
   Graph* g2 = args[0]->run().get_Graph();

   bool res = g1->isomorphic(g2);

   return Value(Logic(res));
}

FunSet::FunSet(ScriptPtr  script)
{
   _name = "set";
   _script = script;
   _num_calls = 0;
   _objRequired = false;
}

Value FunSet::run(vector<ExprPtr>& args, ExprPtr obj)
{
   _num_calls++;
   for (UInt i=0; i<args.size(); i++)
   {
      Axiom axiom(i);
      axiom.parse(args[i]);
      axiom.check();
   }
   return Logic::True;
}

ostream & FunLambda::print(ostream & out)
{
   out << _name << "(";
   for (UInt i = 0; i < _vars.size(); i++)
      out << _vars[i] << (i + 1 < _vars.size() ? "," : "");
   out << ") => ";

   size_t numCommands = _codeCommon->_commands.size() + _codeGet->_commands.size() + _codeSet->_commands.size();
   if (numCommands > 1)
   {
      out << "\n{\n";
   }
   _codeCommon->print(out, _script);
   if (_codeGet->_commands.size() > 0)
   {
      out << "get:\n";
      _codeGet->print(out, _script);
   }
   if (_codeSet->_commands.size() > 0)
   {
      out << "set:\n";
      _codeSet->print(out, _script);
   }
   if (numCommands > 1)
   {
      out << "\n}\n";
   }
   return out;
}

FunCreate::FunCreate(ScriptPtr script)
{
   _name = "create";
   _script = script;
   _num_calls = 0;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_ARRAY);
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}

Value FunCreate::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   int newSize = _vals[0].get_Int();

   if (newSize < 0)
      newSize = 0;

   ValueArr * arr = (ValueArr *)(_objVal.ptr().get());
   while (arr->size() < newSize)
      arr->push();
   while (arr->size() > newSize)
      arr->pop();

   return _objVal;
}

FunArrayConstr::FunArrayConstr(ScriptPtr script)
{
   _name = "ARRAY";
   _script = script;
   _num_calls = 0;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}

Value FunArrayConstr::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   int newSize = _vals[0].get_Int();

   if (newSize < 1)
      newSize = 1;

   shared_ptr<ValueArr> res = make_shared<ValueArr>();

   for (int i = 0; i < newSize; i++)
   {
      res->push();
   }

   return (shared_ptr<ValueBase>)res;
}

FunGraphAdd::FunGraphAdd(ScriptPtr script)
{
   _name = "add";
   _script = script;
   _num_calls = 0;
   _argTypes.resize(3);
   _argTypes[0].push_back(ValueBase::_GRAPH);
   _argTypes[1].push_back(ValueBase::_GRAPH);
}

Value FunGraphAdd::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   Graph * g1 = _vals[0].get_Graph();
   Graph * g2 = _vals[1].get_Graph();

   if (g1->classID() != g2->classID())
      return _vals[0];

   //копируем связи из графа y, которые отсутствуют в результирующем
   for (size_t yNodeIndex = 0; yNodeIndex < g2->_nodes.size(); yNodeIndex++)
   {
      if (yNodeIndex >= g1->_nodes.size())
      {
         //такого узла в res нет, поэтому копируем все значения из y
         Node & yNode = (Node &)g2->_nodes[yNodeIndex];
         for (auto &yEdgeType : yNode.edgesOut())
         {
            if (_vals.size() > 2 && _vals[2].is(yEdgeType._kind))
               continue;
            auto &yEdgeVec = yEdgeType._edges;
            for (auto &yEdge : yEdgeVec)
               g1->add(yEdge);
         }
         for (auto &yEdgeType : yNode.edgesIn())
         {
            if (_vals.size() > 2 && _vals[2].is(yEdgeType._kind))
               continue;
            auto &yEdgeVec = yEdgeType._edges;
            for (auto &yEdge : yEdgeVec)
               g1->add(yEdge);
         }
         continue;
      }

      Node & yNode = (Node &)g2->_nodes[yNodeIndex];
      Node & resNode = g1->_nodes[yNodeIndex];

      for (auto yEdgeType : yNode.edgesOut())
      {
         if (_vals.size() > 2 && _vals[2].is(yEdgeType._kind))
            continue;
         if (resNode.edgesOut(yEdgeType._kind)._edges.size() == 0)
         {
            //связей с таким типом в res нет, копируем все из y
            for (auto &yEdge : yEdgeType._edges)
               g1->add(yEdge);
            continue;
         }

         auto yEdgeVec = yEdgeType._edges;
         auto resEdgeVec = resNode.edgesOut(yEdgeType._kind)._edges;

         for (auto &yEdge : yEdgeVec)
         {
            bool ok = true;
            for (auto &i : resEdgeVec)
            {
               if (i._knd == yEdge._knd && i._n1 == yEdge._n1 && i._n2 == yEdge._n2)
               {
                  ok = false;
                  break;
               }
            }
            if (ok)
            {
               //в res такого ребра нет, берем из y
               g1->add(yEdge);
            }
         }
      }
   }

   g1->setOwner();

   return _vals[0];
}

FunGraphClose::FunGraphClose(ScriptPtr script)
{
   _name = "closed_world";
   _script = script;
   _num_calls = 0;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_GRAPH);
}

Value FunGraphClose::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   if (_vals.size() > 0)
   {
      //_objVal.get_Graph()->close(_vals[0].is_true());
      GraphManager::instance().close(_objVal.get_Graph()->classID(), _vals[0].get_Logic().isTrue());
   }

   return _objVal.get_Graph()->close() ? Logic::True : Logic::False;
}

FunGraphClearEdges::FunGraphClearEdges(ScriptPtr script)
{
   _name = "delete_edges";
   _script = script;
   _num_calls = 0;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_NODE);
   _argTypes.resize(2);
   _argTypes[0].push_back(ValueBase::_EDGE);
}

Value FunGraphClearEdges::run(vector<ExprPtr> & args, ExprPtr obj /*= 0*/)
{
   if (!initRun(args, obj))
      return Value();

   bool recursion = true;
   if (_vals.size() > 1)
   {
      recursion = _vals[1].get_Logic().isTrue();
   }

   Graph * g = _objVal.get_Graph();

   g->clear_edges(_vals[0].get_Int(), _objVal.get_UInt(), recursion);

   return Value();
}

FunGraphSensesDef::FunGraphSensesDef(ScriptPtr script)
{
   _name = "senses_graph";
   _script = script;
   _num_calls = 0;
   _objRequired = true;
   _objTypes.push_back(ValueBase::_GRAPH);
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_GRAPH);
}

Value FunGraphSensesDef::run(vector<ExprPtr>& args, ExprPtr obj)
{
   if (!initRun(args, obj))
      return Value();

   Graph * g = _objVal.get_Graph();

   g->setValueDefSensesGraph(_vals[0].get_Graph());

   return Value();
}

FunEval::FunEval(ScriptPtr script)
{
   _name = "eval";
   _script = script;
   _num_calls = 0;
   _objRequired = false;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_STR);
}

Value FunEval::run(vector<ExprPtr>& args, ExprPtr obj)
{
   if (!initRun(args, obj))
      return Value();

   string         codeStr   = _vals[0].get_Str();
   ScriptPtr      scriptPtr = Script::instance();
   Compiler       compiler(scriptPtr);
   Code*          parentCode = StackFrame::currentStackFrame()->_code;
   CodePtr        codePtr = SPTR_MAKE(Code)(parentCode->shared_from_this());
   compiler.set(codeStr);
   compiler.parse_return(codePtr, codeStr.length()); // парсим как return, чтоб команда вернула значение
   if (codePtr->_commands.size() == 0)
   {
      TRACE_CRITICAL << "can't evaluate string:" << codeStr << endl;
      return Value();
   }
   CommandPtr command = codePtr->_commands[0];
   Int ret = 0; 
   Value retVal;
   command->run(ret, retVal, scriptPtr);
   return retVal;
}
