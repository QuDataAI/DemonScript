/*!            Function - Прототип всех функций скрипта

Объявленные в модуле классы :
class Function;              // Прототип всех функций
class FunDemon;              // Функция (демон)

В конце модуля объявленно множество классов встроенных функций, начледников Function.

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef FunctionH
#define FunctionH


#include "Value.h"
#include "Expr.h"
#include "Code.h"
#include "Script.h"
#include "GraphManager.h"
#include <map>

class StackFrame;
//=======================================================================================
//! Прототип для всех функций (демонов и встроенных)
//
class Function
{
public:
   ScriptPtr      _script;                        //!< указатель на скрипт
   string         _name;                          //!< имя функции
   UInt           _num_calls;                     //!< число вызовов функций
   vector<Value>  _vals;                          //!< значения аргументов функции
   Value          _objVal;                        //!< значение объекта функции
   
   UInt                            _minArgs;      //!< минимально разрешенное число аргументов
   UInt                            _maxArgs;      //!< максимально разрешенное число аргументов
   bool                            _objRequired;  //!< необходимо указывать объект для функции
   vector<ValueBase::Kind>         _objTypes;     //!< разрешенные типы объекта
   vector<vector<ValueBase::Kind>> _argTypes;     //!< разрешенные типы аргументов

   Function(ScriptPtr script = 0);
   Function(const string & name, ScriptPtr script = 0);
   virtual ~Function() {}
   /*!
   Получить полное имя функции (Для модулей Module.func)
   \return результат инициализации
   */
   virtual string fullName() { return _name; }
   /*!
   Получить имя функции
   \return имя функции
   */
   virtual string name() { return _name; }
   //! выполнение функции с аргументами args для объекта obj
   virtual Value run(vector<ExprPtr> & args, ExprPtr _obj = 0) { return Value(); }
   /*!
      Инициализируем запуск функции:
         - инкрементируем счетчик вызовов _num_calls++
         - вычисляем аргументы и значение объекта
         - проверяем корректность типов
      \param args аргументы функции в выражениях
      \param obj выражение объекта функции
      \return результат инициализации
   */
   virtual Bool initRun(vector<ExprPtr> & args, ExprPtr obj = 0);
   //! Вывести в поток out функцию в виде текста (программы)
   virtual ostream & print(ostream & out) { return out; }
   //! Вывести информацию о вычислениях функции
   virtual ostream & out_info(ostream & out) { return out; }
   //! вычисление значений аргументов   
   virtual void eval(vector<ExprPtr> & args, ExprPtr _obj = 0);
   //!< установка значений
   virtual Value set(vector<ExprPtr> & args, ExprPtr _obj, const Value &v) { return v; }
};

//=======================================================================================
//! Скриптовая функция-демон
//
class FunDemon : public Function
{
public:   
   vector<string> _vars;            //!< список имён переменных
   CodePtr        _codeCommon;      //!< список команд демона которые выполняются в обоих режимах Get и Set
   CodePtr        _codeGet;         //!< список команд демона которые выполняются в режиме Get
   CodePtr        _codeSet;         //!< список команд демона которые выполняются в режиме Set
   VariablePtr    _valueVar;        //!< ссылка на переменную "value"
   Int            _numCallsInStack; //!< число вызовов данной функции в текущем стеке

   FunDemon(ScriptPtr script = 0);
   FunDemon(const string & name, vector<string> &vars, ScriptPtr script = 0);
   //! выполнение функции со значениями vals
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
   //! выполнение функции со значениями vals
   Value run();
   //!< установка значений в функцию
   Value set(vector<ExprPtr> & args, ExprPtr _obj, const Value &v);
   //! установка значений в функцию cо значениями vals
   Value set();
   //! Вывести в поток out функцию в виде текста (программы)
   ostream & print(ostream & out);
   //! Вывести информацию о вычислениях функции
   ostream & out_info(ostream & out);
private:
   ///структура для хранения значений переменных при саморекурсии
   SPTR_DEF(VarsSave)
   class VarsSave {
   public:
      class CodeVars {
      public:
         vector<VariablePtr>* _vars;       //!< ссылка на массив переменных
         vector<Value>        _vals;       //!< значения переменных блока кода 
      };
      //! Сохранить значения переменных соответствующего кода и вложенных блоков
      void pushCodeVars(CodePtr code);
      //! Восстановить значения переменных соответствующего кода и вложенных блоков
      void popCodeVars();
      vector<CodeVars> _codesVars;   //!< значения переменных блоков кода
   };
   //! Сохранить значения переменных
   VarsSavePtr pushVars(CodePtr code);
   //! Сохранить значения переменных
   void  popVars(VarsSavePtr savedVars);
   friend class StackFrame;
};

//=======================================================================================
//! Скриптовая лямбда-функция
//
class FunLambda : public FunDemon
{
public:
   FunLambda() : FunDemon()  {}
   FunLambda(vector<string> &vars, ScriptPtr script = 0) : FunDemon("lambda", vars, script) {}
   //! Вывести в поток out функцию в виде текста (программы)
   ostream & print(ostream & out);
};

//=======================================================================================
//! Встроенная функция булевого or
//
class FunOr : public Function
{
public:
   FunOr(ScriptPtr script = 0) { _name = "or";  _script = script;  _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;
      for (UInt i = 0; i < args.size(); i++)
         if (args[i]->run().is_true())
            return Logic(0, 1);                    // есть один истинный
      return Logic(1, 0);                          // нет ни одного истинного
   }
};
//=======================================================================================
//! Встроенная функция булевого and
//
class FunAnd : public Function
{
public:
   FunAnd(ScriptPtr script = 0) { _name = "and";  _script = script;  _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;
      for (UInt i = 0; i < args.size(); i++)
         if (!args[i]->run().is_true())
            return Logic(1, 0);                    // есть один не истинный
      return Logic(0, 1);                          // все истинные
   }
};
//=======================================================================================
//! Встроенная функция существования
//
class FunExists : public Function
{
public:
   FunExists(ScriptPtr script = 0) { _name = "exists";  _script = script;  _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;
#ifdef _DS_CHECK_TYPE
      if (args.size() < 2 || args[0]->kind() != Expr::_VAR) 
      {
         TRACE_CRITICAL << "! FunExists::run> Wrong arguments in function exists\n";
         return Logic(0, 0);
      }
      int minCount = 0;
      int maxCount = 0;
      int curCount = 0;
      bool wasUndef = false;
      if (args.size() >= 3)
      {
         minCount = args[2]->run().get_Int();
         maxCount = minCount;
         if (args.size() > 3)
         {
            maxCount = args[3]->run().get_Int();
         }
      }
#endif
      size_t firstFound = 0;
      Variable & v = *SPTR_DCAST(ExprVar,args[0])->_var;
      vector<Node> & nodes = _script->_graph->get_Graph()->_nodes;
      for (UInt i = 1; i < nodes.size(); i++) 
      {
         v._val = nodes[i].id();
         if (args[1]->run().is_true())
         {
            if (firstFound == 0)
               firstFound = i;
            curCount++;
            if (minCount == 0 && maxCount == 0)
               return Logic::True;
         }
         else if (args[1]->run().get_Logic() != Logic::False)
         {
            wasUndef = true;
         }
      }

      if ((minCount != 0 || maxCount != 0) && curCount >= minCount && curCount <= maxCount)
      {
         v._val = nodes[firstFound].id();
         return Logic::True;
      }

      return (_script->_graph->get_Graph()->close() && !wasUndef) ? Logic::False : Logic::Undef;
   }
};
//=======================================================================================
//! Встроенная функция всеобщности
//
class FunForall : public Function
{
public:
   FunForall(ScriptPtr script = 0) { _name = "forall";  _script = script;  _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;
#ifdef _DS_CHECK_TYPE
      if (args.size() != 2 || args[0]->kind() != Expr::_VAR) {
         TRACE_CRITICAL << "! FunForall::run> Wrong arguments in function exists\n";
         return Logic(0, 0);
      }
#endif
      Variable & v = *(SPTR_DCAST(ExprVar,args[0]))->_var;
      vector<Node> & nodes = _script->_graph->get_Graph()->_nodes;
      for (UInt i = 1; i < nodes.size(); i++) {
         v._val = nodes[i].id();
         if (!args[1]->run().is_true())
            return Logic(1, 0);
      }
      return Logic(0, 1);
   }
};
//=======================================================================================
//! Встроенная функция значения ребра
//
class FunEdge : public Function
{
public:
   FunEdge(ScriptPtr script = 0) { _name = "edge";  _script = script;  _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;                               // число вызовов      
#ifdef _DS_CHECK_POINTER   
      if (!obj) {
         TRACE_CRITICAL << "! .edge: No node for this function\n";
         return Logic();
      }
#endif
      Value edge  = args[0]->run();
      Value node1 = obj->run();
      Value node2 = args[1]->run();
#ifdef _DS_CHECK_TYPE
      if (node1.type() != ValueBase::_NODE) {
         TRACE_CRITICAL << "! .edge: No node for this function\n";
         return Logic();
      }
#endif
      return node1.get_Graph()->val(edge.get_Int(),
         node1.get_UInt(),  node2.get_UInt(),
         node1.get_Graph(), node2.get_Graph());
   }
};

//=======================================================================================
//! Встроенная функция добавления ребра
//
class FunSetEdge : public Function
{
public:
   FunSetEdge(ScriptPtr script = 0) { _name = "set_edge";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;                               // число вызовов        
#ifdef _DS_CHECK_POINTER   
      if (!obj) {
         TRACE_CRITICAL << "! .set_edge: No node for this function\n";
         return Logic();
      }
#endif
      if (args.size()<2) {
         TRACE_CRITICAL << "! .set_edge: Wrong arguments count in function\n";
         return Logic();
      }
      Value edge  = args[0]->run();
      Value val   = Logic::True;  
      Value node1 = obj->run();
      Value node2 = args[1]->run();
      if (args.size()>2)
         val = args[2]->run();
#ifdef _DS_CHECK_TYPE
      if (node1.type() != ValueBase::_NODE  && 
          node1.type() != ValueBase::_ARRAY &&
          node2.type() != ValueBase::_NODE  &&
          node2.type() != ValueBase::_ARRAY 
         ) {
         TRACE_CRITICAL << "! .set_edge: No node for this function\n";
         return Logic();
      }
#endif
      Int edgeID = edge.get_Int();
      if (node1.type() == ValueBase::_NODE)
      {
         setEdge(edgeID, node1, node2, val);
      }
      else if (node1.type() == ValueBase::_ARRAY)
      {
         for (UInt i = 0; i < node1.size(); i++)
         {
            Value node1Item = node1[i];
            setEdge(edgeID, node1Item, node2, val);
         }
         return val;
      }
      return val;
   }

   Value setEdge(Int edge, Value &node1, Value &node2, Value &val)
   {
      if (node2.type() == ValueBase::_NODE)
      {
         node1.get_Graph()->add(edge, node1.get_UInt(), node2.get_UInt(), val.get_Logic(), node2.get_Graph());
         return val;
      }
      else if (node2.type() == ValueBase::_ARRAY)
      {
         for (UInt i = 0; i < node2.size(); i++)
         {
            Value node2Item = node2[i];
            node1.get_Graph()->add(edge, node1.get_UInt(), node2Item.get_UInt(), val.get_Logic(), node2Item.get_Graph());
         }
         return val;
      }
      return Logic();
   }
};

//=======================================================================================
//! Встроенная функция подсчета числа входящих/исходящих ребер с указанной вероятностью 
//
class FunEdgesCount : public Function
{
public:
   FunEdgesCount(ScriptPtr script = 0) { _name = "count_in";  _script = script; _num_calls = 0; }
   bool prepare(vector<ExprPtr> & args, ExprPtr obj, Graph* &graph, UInt &nodeID, Int &edgeID, Logic &logic);
protected:
   Bool _isVal = false;
   
};

//=======================================================================================
//! Встроенная функция подсчета числа входящих ребер с указанной вероятностью 
//
class FunEdgesCountIn : public FunEdgesCount
{
public:
   FunEdgesCountIn(ScriptPtr script = 0): FunEdgesCount(script) { _name = "count_in"; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Встроенная функция подсчета числа исходящих ребер с указанной вероятностью 
//
class FunEdgesCountOut : public FunEdgesCount
{
public:
   FunEdgesCountOut(ScriptPtr script = 0) : FunEdgesCount(script) { _name = "count_out"; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Встроенная функция значения ребра
//
class FunIsEdge : public Function
{
public:
   FunIsEdge(ScriptPtr script = 0) { _name = "is_edge";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      eval(args);
#ifdef _DS_CHECK_POINTER   
      if (!obj) {
         TRACE_CRITICAL << "! .is_edge: No node for this function\n";
         return Logic();
      }
#endif
      Value ob = obj->run();
#ifdef _DS_CHECK_TYPE      
      if (ob.type() != ValueBase::_NODE) {
         TRACE_CRITICAL << "! .is_edge: No node for this function\n";
         return Logic();
      }
      if (_vals.size() != 2) {
         TRACE_CRITICAL << "! is_edge: Wrong number of arguments: " << _vals.size() << " (need 2)\n";
         return Logic();
      }
#endif
      return ob.get_Graph()->is_edge(_vals[0].get_Int(),
         ob.get_UInt(), _vals[1].get_UInt(), ob.get_Graph(), _vals[1].get_Graph());
   }
};
//=======================================================================================
//! Встроенная функция проверки изолированности узла
//
class FunIsolated : public Function
{
public:
   FunIsolated(ScriptPtr script = 0) { _name = "isolated";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      eval(args);
#ifdef _DS_CHECK_POINTER   
      if (!obj) {
         TRACE_CRITICAL << "! .isolated: No node for this function\n";
         return Logic();
      }
#endif
      Value ob = obj->run();
#ifdef _DS_CHECK_TYPE      
      if (ob.type() != ValueBase::_NODE) {
         TRACE_CRITICAL << "! .isolated: No node for this function\n";
         return Logic();
      }
#endif
      if (ob.get_Graph()->isolated(ob.get_UInt()))
         return Logic(0, 1);
      return Logic(1, 0);
   }
};
//=======================================================================================
//! Встроенная функция наличия пути по графу
//
class FunPath : public Function
{
public:
   FunPath(ScriptPtr script = 0) { _name = "path";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Встроенная функция добавления ребра
//
class FunIsa : public Function
{
public:
   FunIsa(ScriptPtr script = 0) { _name = "isa";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;                               // число вызовов        
#ifdef _DS_CHECK_POINTER   
      if (!obj) {
         TRACE_CRITICAL << "! .isa: No node for this function\n";
         return Logic();
      }
#endif
      Value objVal = obj->run();

      if ((objVal.type() == ValueBase::_GRAPH && args.size() != 2) || (objVal.type() == ValueBase::_NODE && args.size() !=1))
      {
         TRACE_CRITICAL << "! .isa: Incorrect parameters count\n";
      }

#ifdef _DS_CHECK_TYPE

      if (objVal.type() != ValueBase::_GRAPH && objVal.type() != ValueBase::_NODE)
      {
         TRACE_CRITICAL << "! .isa: Object is not a graph\n";
         return Logic();
      }

#endif   
      Graph* graph = objVal.get_Graph();

      Int edgeID = ReservedEdgeIDs::EDGE_ISA;

      if (objVal.type() == ValueBase::_GRAPH)
      {
         Value node1 = args[0]->run(), node2 = args[1]->run();
         if (node1.type() != ValueBase::_NODE && node2.type() != ValueBase::_NODE) {
            TRACE_CRITICAL << "! .set_edge: No node for this function\n";
            return Logic();
         }
         return graph->path(edgeID, node1.get_UInt(), node2.get_UInt(), node1.get_Graph(), node2.get_Graph());
      }
      else  if (objVal.type() == ValueBase::_NODE)
      {
         Value node2 = args[0]->run();
         if (node2.type() != ValueBase::_NODE) {
            TRACE_CRITICAL << "! .set_edge: No node for this function\n";
            return Logic();
         }
         return graph->path(edgeID, objVal.get_UInt(), node2.get_UInt(), objVal.get_Graph(), node2.get_Graph());
      }
      return Logic();
   }
};

//=======================================================================================
//! Встроенная функция наличия пути по графу
//
class FunCommon : public Function
{
public:
   FunCommon(ScriptPtr script = 0) { _name = "common";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      eval(args);
#ifdef _DS_CHECK_POINTER   
      if (!obj) {
         TRACE_CRITICAL << "! .common: No node for this function\n";
         return Logic();
      }
#endif
      Value ob = obj->run();
#ifdef _DS_CHECK_TYPE      
      if (ob.type() != ValueBase::_NODE) {
         TRACE_CRITICAL << "! .common: No node for this function\n";
         return Logic();
      }
      if (_vals.size() != 2) {
         TRACE_CRITICAL << "! .common: Wrong number of arguments: " << _vals.size() << " (need 2)\n";
         return Logic();
      }
#endif
      Int e;
      UInt y = _vals[1].get_UInt();
      //UInt x = ob.get_Int();
      UInt x = ob.get_UInt();

      if (_vals[0].type() == ValueBase::_EDGE) {
         e = _vals[0].get_Int();
         vector<Int> es; es.push_back(e);
         return _script->_graph->get_Graph()->common(es, x, y);
      }
      if (_vals[0].type() == ValueBase::_ARRAY) {
         vector<Int> es;
         for (UInt i = 0; i < _vals[0].size(); i++) {
            Int e = _vals[0][i]->get_Int();
            es.push_back(e);
         }
         return _script->_graph->get_Graph()->common(es, x, y);
      }
      return Value();
   }
};
//=======================================================================================
//! Встроенная функция удаления узлов
//
class FunDeleteNodes : public Function
{
public:
   FunDeleteNodes(ScriptPtr script = 0) { _name = "delete_nodes";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Встроенная функция удаления узлов
//
class FunIsomorphic : public Function
{
public:
   FunIsomorphic(ScriptPtr script = 0);
   Value run(vector<ExprPtr>& args, ExprPtr obj = 0);
};

//=======================================================================================
//! Встроенная функция очистки рёбер
//
class FunClearEdges : public Function
{
public:
   FunClearEdges(ScriptPtr script = 0) { _name = "clear_edges";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;                               // число вызовов      
#ifdef _DS_CHECK_POINTER   
      if (!obj) {
         TRACE_CRITICAL << "! .clear_edges: No object for this function\n";
         return Logic();
      }
#endif
#ifdef _DS_CHECK_TYPE
      if (obj->kind() != Expr::_VAR)
      {
         TRACE_CRITICAL << "! .clear_edges: You must used variable of graph type\n";
         return Logic();
      }         
#endif
      Graph * g = obj->run().get_Graph();
      if (!g)
         return "! clear_edges: You must used defined variable of graph type\n";
      g->clear_edges();
      return Logic();
   }
};
//=======================================================================================
//! Встроенная функция блокировки изменения уже заданных рёбер графа
//
class FunEdgesBlocked : public Function
{
public:
   FunEdgesBlocked(ScriptPtr script = 0) { _name = "edges_blocked";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;                               // число вызовов      
#ifdef _DS_CHECK_POINTER   
      if (!obj) {
         TRACE_CRITICAL << "! .edges_blocked: No node for this function\n";
         return Logic();
      }
#endif
#ifdef _DS_CHECK_TYPE
      if (obj->kind() != Expr::_VAR)
      {
         TRACE_CRITICAL << "! .edges_blocked: You must used variable of graph type\n";
         return Logic();
      }
#endif
      Graph * g = obj->run().get_Graph();
      if (!g)
      {
         TRACE_CRITICAL << "! .edges_blocked:You must used defined variable of graph type\n";
         return Logic();
      }

      if (args.size() == 1) {
         Float f = args[0]->run().get_Float();
         if (f == 0) g->_edges_blocked = 0;
         else if (f == 1) g->_edges_blocked = 1;

      }
      return Float(g->_edges_blocked);
   }
};
//=======================================================================================
//! Встроенная функция числа изменений уже заданных рёбер графа
//
class FunEdgesChanges : public Function
{
public:
   FunEdgesChanges(ScriptPtr script = 0) { _name = "edges_changes";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;                               // число вызовов      
#ifdef _DS_CHECK_POINTER   
      if (!obj) {
         TRACE_CRITICAL << "! .edges_changes: No node for this function\n";
         return Logic();
      }
#endif
#ifdef _DS_CHECK_TYPE
      if (obj->kind() != Expr::_VAR)
      {
         TRACE_CRITICAL << "! .edges_changes: You must used variable of graph type\n";
         return Logic();
      }
#endif
      Graph * g = obj->run().get_Graph();
      if (!g)
         return "! .edges_changes: You must used defined variable of graph type\n";

      UInt res = g->_edges_changes;
      if (args.size() == 1) {
         Float f = args[0]->run().get_Float();
         if (f >= 0) g->_edges_changes = UInt(f);
      }
      return Float(res);
   }
};
//=======================================================================================
//! Встроенная функция определения того, изменился ли граф
//
class FunGraphChanged : public Function
{
public:
   FunGraphChanged(ScriptPtr script) { _name = "changed";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;                               // число вызовов      
#ifdef _DS_CHECK_POINTER   
      if (!obj) {
         TRACE_CRITICAL << "! .changed: No node for this function\n";
         return Logic();
      }
#endif      
#ifdef _DS_CHECK_TYPE
      if (obj->kind() != Expr::_VAR)
      {
         TRACE_CRITICAL << "! .changed: You must used variable of graph type\n";
         return Logic();
      }
#endif
      Graph * g = obj->run().get_Graph();
      if (!g)
         return "! .changed: You must used defined variable of graph type\n";

      UInt changes = g->_changes;
      g->_changes = 0;
      return changes == 0 ? Value(Logic(1, 0)) : Value(Logic(0, 1));
   }
};
//=======================================================================================
//! Встроенная функция вывода графа на экран в dot-формате
//
class FunGraphDot : public Function
{
public:
   FunGraphDot(ScriptPtr script) { _name = "dot";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;                               // число вызовов      
#ifdef _DS_CHECK_POINTER   
      if (!obj) {
         TRACE_CRITICAL << "! .dot: No node for this function\n";
         return Logic();
      }
#endif
#ifdef _DS_CHECK_TYPE
      if (obj->kind() != Expr::_VAR)
      {
         TRACE_CRITICAL << "! .dot: You must used variable of graph type\n";
         return Logic();
      }
         
#endif
      Graph * g = obj->run().get_Graph();
      if (!g)
      {
         TRACE_CRITICAL << "! .dot: You must used defined variable of graph type\n";
         return Logic();
      }
        
      shared_ptr<vector<Int>> edgesFilter;
      bool         showLoop    = true;
      Int          mode        = 0;

      if (args.size() > 0)
      {
         Value fn = args[0]->run();
         if (fn.type() == ValueBase::_STR) {
            string fname = fn.get_Str();
            ofstream fout(_script->workPath(fname));
            if (fout.is_open()) {
               if (args.size() > 1)
               {
                  Value edgesFilterVal = args[1]->run();
                  if (edgesFilterVal.type() == ValueBase::_ARRAY)
                  {
                     edgesFilter = shared_ptr<vector<Int>>(new vector<Int>);
                     for (UInt i = 0; i < edgesFilterVal.size(); i++)
                     {
                        Value edgeID = edgesFilterVal[i];
                        edgesFilter->push_back(edgeID.get_Int());
                     }
                  }
                  else
                  {
                     TRACE_CRITICAL << "! out_graph_dot: Wrong type of the second argument (Array is expected)" << endl;
                     return Logic();
                  }
               }
               if (args.size() > 2)
               {
                  Value showLoopVal = args[2]->run();
                  if (showLoopVal.type() == ValueBase::_LOGIC)
                  {
                     showLoop = showLoopVal.get_Logic().isTrue();
                  }
                  else
                  {
                     TRACE_CRITICAL << "! out_graph_dot: Wrong type of the third argument (Logic is expected)" << endl;
                     return Logic();
                  }
               }
               if (args.size() > 3)
               {
                  Value modeVal = args[3]->run();
                  if (modeVal.type() != ValueBase::_FLOAT)
                  {
                     TRACE_CRITICAL << "! out_graph_dot: Wrong type of the fourth argument (Float is expected)" << endl;
                     return Logic();
                  }
                  mode = (Int)modeVal.get_Float();
               }
               if(args.size() > 4)
               {
                  TRACE_CRITICAL << "! out_graph_dot: Wrong number of arguments: " << args.size() << " (maximum 3)\n";
                  return Logic();
               }
               g->print_dot(fout, edgesFilter.get(), showLoop, mode);
               fout.close();
            }
            else {
               TRACE_CRITICAL << "! out_graph_dot: Can\'t open file " << fname << endl;
               TRACE_STREAM(tout);
               g->print_dot(tout, edgesFilter.get(), showLoop, mode);
            }
         }
      }
      else
      {
         TRACE_STREAM(tout);
         g->print_dot(tout, edgesFilter.get(), showLoop, mode);
      }
      return Logic(0, 1);
   }
};
//=======================================================================================
//! Фунеция работает аналогично out GRAPH, но имеет параметры настройки фомата вывода графа.
//! Первый параметр - показывать ли Value
//
class FunGraphJson : public Function
{
public:
   FunGraphJson(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Встроенная функция вывода графа на формальном естественном языке
//
class FunGraphStr : public Function
{
public:
   FunGraphStr(ScriptPtr script) { _name = "str";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;                               // число вызовов      
#ifdef _DS_CHECK_POINTER   
      if (!obj) {
         TRACE_CRITICAL << "! .str: No node for this function\n";
         return Logic();
      }
#endif
#ifdef _DS_CHECK_TYPE
      if (obj->kind() != Expr::_VAR)
      {
         TRACE_CRITICAL << "! .str: You must used variable of graph type\n";
         return Logic();
      }
#endif
      Graph * g = obj->run().get_Graph();
      if (!g)
      {
         TRACE_CRITICAL << "! .str: You must used defined variable of graph type\n";
         return Logic();
      }
         
      string txt;
      g->get_str(txt);
      return txt;
   }
};
//=======================================================================================
//! Встроенная функция копирования графа и массива
// \todo и доя массива
//
class FunCopy : public Function
{
public:
   FunCopy(ScriptPtr script) { _name = "copy";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;                               // число вызовов      
      Value gc = obj->run().copy();
      if (args.size()) {
         Value name = args[0]->run();
         if (name.type() == ValueBase::_STR) {
            string n = name.get_Str();
            if (gc.type() == ValueBase::_GRAPH)
               gc.get_Graph()->_graph_name = n;
         }
         else
            TRACE_CRITICAL << "! .copy: must has string argument\n";
      }
      return gc;
   }
};
//=======================================================================================
//! Встроенная функция копирования любого объекта с копированием элементов контейнера
// \todo и доя массива
//
class FunCopyDeep : public Function
{
public:
   FunCopyDeep(ScriptPtr script) { _name = "copy_deep";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;                               // число вызовов      
      Value gc = obj->run().copyDeep();
      if (args.size()) 
      {
         Value name = args[0]->run();
         if (name.type() == ValueBase::_STR) 
         {
            string n = name.get_Str();
            if (gc.type() == ValueBase::_GRAPH)
               gc.get_Graph()->_graph_name = n;
         }
         else
            TRACE_CRITICAL << "! .copy_deep: must have string argument\n";
      }
      return gc;
   }
};

//=======================================================================================
//! Встроенная функция задания/изменения имени графа
// \todo и для массива
//
class FunName : public Function
{
public:
   FunName(ScriptPtr script) { _name = "name";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;                               // число вызовов      
#ifdef _DS_CHECK_POINTER   
      if (!obj) {
         TRACE_CRITICAL << "! .name: No node for this function\n";
         return Logic();
      }
#endif         
      Value g = obj->run();                       // \todo array
      if (g.type() == ValueBase::_GRAPH) {
         Graph *gr = g.get_Graph();
         if (args.size()) {
            Value name = args[0]->run();
            if (name.type() == ValueBase::_STR) {
               string n = name.get_Str();
               gr->_graph_name = n;
            }
            else
               TRACE_CRITICAL << "! FunName> .name must has string argument\n";
         }
         return gr->_graph_name;
      }
      return "unknown";
   }
};
//=======================================================================================
//! Встроенная функция открытия файла
//
class FunFopen : public Function
{
public:
   clock_t _start;
   FunFopen(ScriptPtr script) { _name = "fopen";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      eval(args);
      auto openMode = std::ofstream::out;
      if (_vals.size() > 2) {
         TRACE_CRITICAL << "! fopen: Wrong number of arguments: " << _vals.size() << " (need 1 or 2)\n";
         return Logic();
      }

      if (_vals.size() > 1) {
         if (_vals[1].type() != ValueBase::_STR)
         {
            TRACE_CRITICAL << "! fopen: Wrong type of second argument (need STR:\"w\" or \"a\")\n";
            return Logic();
         }
         string strMode = _vals[1].get_Str();
         if (strMode == "w")
         {
            openMode = std::ofstream::out;
         }
         else if (strMode == "a")
         {
            openMode = std::ofstream::app;
         }
         else
         {
            TRACE_CRITICAL << "! fopen: Wrong value of second argument (need \"w\" or \"a\")\n";
            return Logic();
         }
      }

      string fname = _vals[0].get_Str();
      _script->_fout.open(_script->workPath(fname), openMode);
      return Logic();
   }
};
//=======================================================================================
//! Встроенная функция закрытия файла
//
class FunFclose : public Function
{
public:
   clock_t _start;
   FunFclose(ScriptPtr script) { _name = "fclose";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      eval(args);
      if (_vals.size() != 0) {
         TRACE_CRITICAL << "! fclose: Wrong number of arguments: " << _vals.size() << " (need 1)\n";
         return Logic();
      }
      _script->_fout.close();
      return Logic();
   }
};
//=======================================================================================
//! Встроенная функция возвращает типа переменой (как строку)
//
class FunType : public Function
{
public:
   FunType(ScriptPtr script) { _name = "type";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;
      if (!obj)
         return "???";
      return obj->run().type_str();
   }
};
//=======================================================================================
//! Встроенная функция число ссылок на этот share_ptr
// \todo не работает?
//
class FunCount : public Function
{
public:
   FunCount(ScriptPtr script) { _name = "count";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;
      if (!obj)
         return "???";
      return Float(obj->run().count());
   }
};
//=======================================================================================
//! Встроенная функция размерность переменой 
//
class FunSize : public Function
{
public:
   FunSize(ScriptPtr script) { _name = "size";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;
      if (!obj)
         return Float(0);
      return Float(obj->run().size());
   }
};
//=======================================================================================
//! Встроенная функция преобразования переменной любого типа в строку
// \todo
//
class FunStr : public Function
{
public:
   FunStr(ScriptPtr script) { _name = "str";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      eval(args);
      if (!obj)
         return "??????";
      return "";
   }
};
//=======================================================================================
//! Встроенная функция очистки массива
//
class FunClear : public Function
{
public:
   FunClear(ScriptPtr script) { _name = "clear";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      eval(args);
      if (!obj)
         return Logic(1, 0);
      obj->run().clear();
      return Logic();
   }
};
//=======================================================================================
//! Встроенная функция проверки есть ли в массиве элемент
//
class FunIs : public Function
{
public:
   FunIs(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Встроенная функция поиска элемента в массиве
//
class FunFind : public Function
{
public:
   FunFind(ScriptPtr script) { _name = "find";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Встроенная функция перемешивания элементов массива
//
class FunMix : public Function
{
public:
   FunMix(ScriptPtr script) { _name = "mix";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      eval(args);
      if (!obj)
         return Logic(1, 0);
      obj->run().mix();
      return Logic();
   }
};
//=======================================================================================
//! Встроенная функция проверки массива на пустоту
//
class FunEmpty : public Function
{
public:
   FunEmpty(ScriptPtr script) { _name = "empty";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;
      if (!obj)
         return Logic();
      if (obj->run().size())
         return Logic(1, 0);
      return Logic(0, 1);
   }
};
//=======================================================================================
//! Встроенная функция добавления в конец массива элементов
//
class FunPush : public Function
{
public:
   FunPush(ScriptPtr script) { _name = "push";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Встроенная функция добавления в начало массива элементов
//
class FunUnshift : public Function
{
public:
   FunUnshift(ScriptPtr script) { _name = "unshift";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Встроенная функция выталкивания из массива последнего элемента
//
class FunPop : public Function
{
public:
   FunPop(ScriptPtr script) { _name = "pop";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Встроенная функция выталкивания из массива первого элемента
//
class FunShift : public Function
{
public:
   FunShift(ScriptPtr script) { _name = "shift";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Встроенная функция выталкивания преобразования переменной в строку
//
class FunToString : public Function
{
public:
   FunToString(ScriptPtr script) { _name = "string";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Встроенная функция преобразования переменной в число
//
class FunFloat : public Function
{
public:
   FunFloat(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Встроенная функция конструктора графа
//
class FunGraphConstr : public Function
{
public:
   FunGraphConstr(ScriptPtr script) { _name = "GRAPH";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;
      Value graphValue((shared_ptr<ValueBase>)ValueGraph::create());
      Graph * graph = graphValue.get_Graph();
      if (args.size() > 0)
      {
         //есть имя, устанавливаем
         Value name = args[0]->run();
         if (name.type() == ValueBase::_STR) {
            string n = name.get_Str();
            graph->_graph_name = n;
         }
      }
      return graphValue;
   }
};

//=======================================================================================
//! Преобразование переменной произвольного типа в логическую переменную
//
class FunLogic : public Function
{
public:
   FunLogic(ScriptPtr script) { _name = "logic";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;
      if (!obj)
         return "???";
      Value v = obj->run(); Float f = 0;
      if (v.type() == ValueBase::_LOGIC)          // уже логическая переменная
         return v;
      if (v.type() == ValueBase::_ARRAY) 
      {
         if (v.size() > 1)                        // у массива есть 2 элемента            
            return Value(Logic(v[0]->get_Float(), v[1]->get_Float()));
         if (v.size() == 0)                       // пустой массив
            return Value(Logic(0, 0));            // возвращаем Undef
         f = v[0]->get_Float();                   // единственный элемент
      }
      if (v.type() == ValueBase::_FLOAT)
         f = v.get_Float();
      if (f == 0)
         return Value(Logic(1, 0));
      if (f == 1)
         return Value(Logic(0, 1));
      if (f == 0.5)
         return Value(Logic(0.5, 0.5));
      return Value(Logic(0, 0));
   }
};
//=======================================================================================
//! Преобразование переменной произвольного типа в массив
//
class FunArray : public Function
{
public:
   FunArray(ScriptPtr script) { _name = "array";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Преобразование переменной произвольного типа в логическую переменную
//
class FunFusNumber : public Function
{
public:
   FunFusNumber(ScriptPtr script) { _name = "fuzzy";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;
      if (!obj)
         return "???";
      Value v = obj->run(); 
      return v.get_FusNumber();
   }
};

//=======================================================================================
//! Загрузка объекта из файла
//
class FunLoad : public Function
{
public:
   FunLoad(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Добавление n безымянных узлов в граф
//
class FunAddNodes : public Function
{
public:
   FunAddNodes(ScriptPtr script) { _name = "add_nodes";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

////=======================================================================================
////! Получение числа узлов графа
////
//class FunNodes : public Function
//{
//public:
//   FunNodes(ScriptPtr script) { _name = "nodes";  _script = script; _num_calls = 0; }
//   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
//};

//=======================================================================================
//! Получение числа типов рёбер графа
//
class FunEdgesNum : public Function
{
public:
   FunEdgesNum(ScriptPtr script) { _name = "edgesNum";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Получение/установка значения в узел графа
//
class FunValue : public Function
{
public:
   FunValue(ScriptPtr script) { _name = "value";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Получение идентификатора класса графов
//
class FunClassID : public Function
{
public:
   FunClassID(ScriptPtr script) { _name = "class_id";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Получение графа, к которому принадлежит узел
//
class FunGraph : public Function
{
public:
   FunGraph(ScriptPtr script) { _name = "graph";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Функция возвращает массив последовательных элементов
//
class FunRange : public Function
{
public:
   FunRange(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Изменить все рёбра edge графа со значения val1 на val2:   GRAPH.change(edge, val1, val2)
//
class FunGraphChange : public Function
{
public:
   FunGraphChange(ScriptPtr script) { _name = "change";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0)
   {
      _num_calls++;                               // число вызовов      
#ifdef _DS_CHECK_POINTER   
      if (!obj) {
         TRACE_CRITICAL << "! .changed: No node for this function\n";
         return Logic();
      }
#endif      
#ifdef _DS_CHECK_TYPE
      if (obj->kind() != Expr::_VAR)
      {
         TRACE_CRITICAL << "! .changed: You must used variable of graph type\n";
         return Logic();
      }
#endif
      Graph * g = obj->run().get_Graph();
      if (!g)
         return "! .changed: You must used defined variable of graph type\n";

      Int edge = 0;                               // меняемый тип рера
      Logic val1 = Logic::Undef, val2 = Logic::Undef;
      if (args.size() > 0)
         edge = args[0]->run().get_Int();         // меняем это значение 
      if (args.size() > 1)
         val1 = args[1]->run().get_Logic();
      if (args.size() > 2)                        // на это
         val2 = args[2]->run().get_Logic();

      size_t cnt = 0;                             // число сделанных замен
      for (size_t i = 1; i < g->_nodes.size(); i++)
         for (size_t j = 1; j < g->_nodes.size(); j++)
            if (g->val(edge, i, j) == val1) {
               g->add(edge, i, j, val2);
               cnt++;
            }
      return Value(cnt);
   }
};

//=======================================================================================
//! Функция возвращает идентификатор объекта
//
class FunId : public Function
{
public:
   FunId(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Добавление одного безымянного узла в граф, возвращает его id
//
class FunAddNode : public Function
{
public:
   FunAddNode(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Удаление указанного числа элементов начиная с определенной позиции
//
class FunSplice : public Function
{
public:
   FunSplice(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Установить уровень логирования
//
class FunVerbose : public Function
{
public:
   FunVerbose(ScriptPtr script) { _name = "verbose";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Установить режим залочки ребра
//
class FunLocked : public Function
{
public:
   FunLocked(ScriptPtr script) { _name = "locked";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Установить режим автосортировки
//
class FunSorted : public Function
{
public:
   FunSorted(ScriptPtr script) { _name = "sorted";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Прочитать строку из файла
//
class FunGetLine : public Function
{
public:
   FunGetLine(ScriptPtr script) { _name = "getline";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Прочитать массив строк из файла
//
class FunGetLines : public Function
{
public:
   FunGetLines(ScriptPtr script) { _name = "getlines";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Прочитать весь файл в строку
//
class FunIn : public Function
{
public:
   FunIn(ScriptPtr script) { _name = "in";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Проверить, достигнут ли конец файла
//
class FunEOF : public Function
{
public:
   FunEOF(ScriptPtr script) { _name = "eof";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Проверить, открыть ли файл
//
class FunIsOpen : public Function
{
public:
   FunIsOpen(ScriptPtr script) { _name = "is_open";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Вывести данные в файл
//
class FunOut : public Function
{
public:
   FunOut(ScriptPtr script) { _name = "out";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Преобразовать строку в выражение
//
class FunEval : public Function
{
public:
   FunEval(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Закрыть файл
//
class FunClose : public Function
{
public:
   FunClose(ScriptPtr script) { _name = "close";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Разделить строку с использованием регулярного выражения
//
class FunSplit : public Function
{
public:
   FunSplit(ScriptPtr script) { _name = "split";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Найти первое вхождение подстроки в строку
//
class FunIndexOf : public Function
{
public:
   FunIndexOf(ScriptPtr script) { _name = "indexOf";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Обрезать пробельные символы
//
class FunTrim : public Function
{
public:
   FunTrim(ScriptPtr script) { _name = "trim";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Вернуть длину строки
//
class FunLength : public Function
{
public:
   FunLength(ScriptPtr script) { _name = "length";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Получить подстроку по началу и длине
//
class FunSubstr : public Function
{
public:
   FunSubstr(ScriptPtr script) { _name = "substr";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Получить подстроку по началу и концу
//
class FunSubstring : public Function
{
public:
   FunSubstring(ScriptPtr script) { _name = "substring";  _script = script; _num_calls = 0; }
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Заменить первое вхождение подстроки на другую
//
class FunReplace : public Function
{
public:
   FunReplace(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Заменить все соответствия регулярному выражению
//
class FunReplaceRegex : public Function
{
public:
   FunReplaceRegex(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Повторить строку n раз
//
class FunRepeat : public Function
{
public:
   FunRepeat(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! есть ли подстрока
//
class FunInclude : public Function
{
public:
   FunInclude(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! начинается ли строка подстрокой (true)
//
class FunStartsWith : public Function
{
public:
   FunStartsWith(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! заканчивается ли строка подстрокой (true)
//
class FunEndsWith : public Function
{
public:
   FunEndsWith(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! индекс первого вхождения подстроки, или -1, если нет
//
class FunSearch : public Function
{
public:
   FunSearch(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Получить атрибут значения
//
class FunAttr : public Function
{
public:
   FunAttr(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Получить массив узло в которые из ноды ведет ребро заданного типа (И значения)
//
class FunNodes : public Function
{
public:
   FunNodes(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Получить или проверить отношения sub, obj, get
//
class FunGet : public Function
{
public:
   FunGet(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Получить массив типов ребер (опционально - только добавленных типов ребе)
//
class FunEdges : public Function
{
public:
   FunEdges(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Привязать вершину к текущему графу
//
class FunBind : public Function
{
public:
   FunBind(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Отвязать вершину от графа
//
class FunUnbind : public Function
{
public:
   FunUnbind(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Отсортировать массив
//
class FunSort : public Function
{
public:
   FunSort(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Создать массив из n элементов
//
class FunCreate : public Function
{
public:
   FunCreate(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Развернуть KНФ и выполнить выражение
//
class FunSet : public Function
{
public:
   FunSet(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Встроенная функция конструктора Массива
//
class FunArrayConstr : public Function
{
public:
   FunArrayConstr(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Встроенная функция сложения двух графов со ребрами исключениями
//
class FunGraphAdd : public Function
{
public:
   FunGraphAdd(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Установить или получить флаг close у графа
//
class FunGraphClose : public Function
{
public:
   FunGraphClose(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};
//=======================================================================================
//! Рекурсивно удалить ребра нужного типа
//
class FunGraphClearEdges : public Function
{
public:
   FunGraphClearEdges(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

//=======================================================================================
//! Установить граф смыслов по умолчанию для текущего графа
//
class FunGraphSensesDef : public Function
{
public:
   FunGraphSensesDef(ScriptPtr script);
   Value run(vector<ExprPtr> & args, ExprPtr obj = 0);
};

#endif