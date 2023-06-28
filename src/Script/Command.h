/*!            Command - Прототип команды скрипта

Каждая команда может быть (это различные наследники, см. ниже):
* Expr           - выражение, вычисляктся его значение
* var var = Expr - объявление пременной ("= Expr" может отсутствовать)
* const c1,c2,.. - объявление констант (имён объектов, отношений и смыслов)
* out Expr       - вычислить выражение и вывести его значение в поток (на консоль)
* return Expr    - остановка и возврат значения из функции
* if Expr : Code - если выражение Expr выполняется последовательность комманд Code
* for Variable in List : Code

В конце модуля объявленно множество классов команд, начледников Command.

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef CommandH
#define CommandH

#include "Value.h"
#include "Expr.h"
#include "Code.h"

//=======================================================================================
// Предварительное объявление классов:
class Script;        //!< скрипт
class Variable;      //!< переменная скрипта

SPTR_DEF(Command)

//=======================================================================================
//! Прототип команды скрипта
class Command
{
   size_t _sourceID = 0;                                  //!< идентификатор скрипта в котором текущая команда расположена
   size_t _line     = 0;                                  //!< номер строки в скрипте, где текущая команда расположена
public:
   //типы прерывания кода
   enum BreakType {
      WAS_NONE      = 0,       //!< ничего
      WAS_RETURN    = 1,       //!< была команда выхода из функции
      WAS_BREAK     = 2,       //!< была команда выхода из цикла
      WAS_CONTINUE  = 3        //!< была команда выхода из текущей итерации цикла
   };
   Command(size_t sourceID, size_t line) :_sourceID(sourceID), _line(line) {}
   virtual ~Command() {}
   /*!
   Выполнить команду
   \ret код возврата из команды
   \retVal возвращаемое значение команды
   \script скрипт исполнения
   */
   virtual void   run(Int &ret, Value &retVal, ScriptPtr script = 0) = 0;  //!< Выполнить комманду                                                          
   virtual ostream & print(ostream & out, ScriptPtr script = 0) = 0;       //!< Вывести в поток out
   //! Вывести информацию о вычислениях команды
   virtual ostream & out_info(ostream & out, UInt shift = 0) { return out; }
   size_t  sourceID() { return _sourceID; }               //!< идентификатор скрипта в котором текущая команда расположена
   size_t  line() { return _line; }                       //!< номер строки в скрипте, где текущая команда расположена
};
//=======================================================================================
//! Команда, запускающая простое выражение
//
class Command_expr : public Command
{
   ExprPtr _expr;                    //!< выражение
public:
   Command_expr(ExprPtr  expr, size_t sourceID, size_t line): Command(sourceID, line) { _expr = expr; }
   void run(Int &ret, Value &retVal, ScriptPtr script = 0);             //!< Выполнить комманду   
   ostream & print(ostream & out, ScriptPtr script = 0); //!< Вывести в поток out 
};
//=======================================================================================
//! Команда объявления констант
//
class Command_const : public Command
{
public:
   Command_const(size_t sourceID, size_t line) : Command(sourceID, line) {}
   vector<string>  _consts;        //!< список имён констант
   void    run(Int &ret, Value &retVal, ScriptPtr script = 0) {}         //!< Выполнить var, вернёт ret=0
   ostream & print(ostream & out, ScriptPtr script = 0);   //!< Вывести в поток out
};
//=======================================================================================
//! Команда объявления связей
//
class Command_edges : public Command
{
public:
   Command_edges(size_t sourceID, size_t line) : Command(sourceID, line) {}
   vector<string>  _edges;        //!< список имён cвязей
   void    run(Int &ret, Value &retVal, ScriptPtr script = 0) {} //!< Выполнить var, вернёт ret=0
   ostream & print(ostream & out, ScriptPtr script = 0);   //!< Вывести в поток out
};
//=======================================================================================
//! Команда объявления связей
//
class Command_nodes : public Command
{
   ExprPtr _graphExpr = 0;       //!< выражение, вычисляющее граф, для которго нужно добавить узлы
public:
   Command_nodes(ExprPtr  graphExpr, size_t sourceID, size_t line) :Command(sourceID, line), _graphExpr(graphExpr){}
   vector<string>  _nodes;        //!< список имён констант
   void      run(Int &ret, Value &retVal, ScriptPtr script = 0);         //!< Выполнить var, вернёт ret=0
   ostream & print(ostream & out, ScriptPtr script = 0);   //!< Вывести в поток out
};
//=======================================================================================
//! Команда объявления переменной
//
class Command_var : public Command
{
public:
   vector<ExprPtr>  _exprs;         //!< список выражений

   Command_var(size_t sourceID, size_t line) : Command(sourceID, line) { }
   void     run(Int &ret, Value &retVal, ScriptPtr script = 0); //!< Выполнить  var, вернёт ret=0
   ostream & print(ostream & out, ScriptPtr script = 0);   //!< Вывести в поток out
};

//=======================================================================================
//! Команда объявления статической переменной
//
class Command_static : public Command
{
public:
   vector<ExprPtr> _exprs;          //!< список выражений
   Bool            _wasRun = false; //!< статические выражения выполняем только один раз

   Command_static(size_t sourceID, size_t line) : Command(sourceID, line) { }
   void     run(Int &ret, Value &retVal, ScriptPtr script = 0); //!< Выполнить  var, вернёт ret=0
   ostream & print(ostream & out, ScriptPtr script = 0);   //!< Вывести в поток out
};
//=======================================================================================
//! Команда вывода на консоль
//
class Command_out : public Command
{
public:
   vector<ExprPtr>  _exprs;         //!< список выражений
   Int             _fout;          //!< выводить в файл
   Bool            _carRet = true; //!< нужно ли переводить коретку после вывода

   Command_out(size_t sourceID, size_t line) :Command(sourceID, line) { _fout = 0; }
   void      run(Int &ret, Value &retVal, ScriptPtr script = 0);      //!< Выполнить out, вернёт ret=0
   ostream & print(ostream & out, ScriptPtr script = 0);//!< Вывести в поток out 
};
//=======================================================================================
//! Команда возврата из функции
//
class Command_return : public Command
{
public:
   ExprPtr _expr;                    //!< выражение
   Int     _isdef;                   //!< была команда isdef

   Command_return(size_t sourceID, size_t line) : Command(sourceID, line) { _expr = 0; _isdef = 0; }
   Command_return(ExprPtr  expr, size_t sourceID, size_t line) : Command(sourceID, line)  { _expr = expr; _isdef = 0; }
   void     run(Int &ret, Value &retVal, ScriptPtr script = 0);//!< Выполнить  return, вернёт  ret=1 
   ostream & print(ostream & out, ScriptPtr script = 0);//!< Вывести в поток out комманду
};
//=======================================================================================
//! Команда прерывания цикла
//
class Command_break : public Command
{
public:
   Command_break(size_t sourceID, size_t line) : Command(sourceID, line) {}
   void    run(Int &ret, Value &retVal, ScriptPtr script = 0); //!<  Выполнить break, вернёт ret=2 
   //!< Вывести в поток out комманду
   ostream & print(ostream & out, ScriptPtr script = 0) { return out << "break"; }
};

//=======================================================================================
//! Команда продолжения цикла
//
class Command_continue : public Command
{
public:
   Command_continue(size_t sourceID, size_t line) : Command(sourceID, line) {}
   void    run(Int &ret, Value &retVal, ScriptPtr script = 0);//!<  Выполнить continue, вернёт ret=3 
                                              //!< Вывести в поток out комманду
   ostream & print(ostream & out, ScriptPtr script = 0) { return out << "continue"; }
};

//=======================================================================================
//! Команда возврата из функции
//
class Command_include : public Command
{
public:
   Command_include(size_t sourceID, size_t line) :Command(sourceID, line) {}
   string _fname;                  //!< имя файла
   void     run(Int &ret, Value &retVal, ScriptPtr script = 0) {}//!< Выполнить include, вернёт  ret=0
   ostream & print(ostream & out, ScriptPtr script = 0);//!< Вывести в поток out комманду
};
//=======================================================================================
//! Команда условного оператора
//
class Command_if : public Command
{
   ExprPtr _expr;                    //!< выражение
   CodePtr _codeIf;                  //!< код после if, который выполняется если выражение истинно
   CodePtr _codeFalse;               //!< код, который выполняется если выражение ложно
   CodePtr _codeUndef;               //!< код, который выполняется если выражение не определено
   CodePtr _codeElse;                //!< код, который выполняется если выражение не истинно 
                                     //!< и не ложно если указан   _codeFalse
                                     //!< и определено если указан _codeUndef

   UInt _num_oks;                    //!< сколько раз сработал if
public:
   Command_if(ExprPtr expr,
              CodePtr codeIf,
              CodePtr codeFalse,
              CodePtr codeUndef,
              CodePtr codeElse,
              size_t  sourceID, size_t line) :
              Command(sourceID, line),
      _expr(expr),
      _codeIf(codeIf),
      _codeFalse(codeFalse),
      _codeUndef(codeUndef),
      _codeElse(codeElse),
      _num_oks(0) {}
   void      run(Int &ret, Value &retVal, ScriptPtr script = 0);//!< Выполнить комманду
   ostream & print(ostream & out, ScriptPtr script = 0);//!< Вывести в поток out
                                                      //! Вывести информацию о вычислениях команды if
   ostream & out_info(ostream & out, UInt shift = 0);
};
//=======================================================================================
//! Команда цикла while
//
class Command_while : public Command
{
public:
   ExprPtr     _expr;                //!< выражение условия цикла
   CodePtr     _code;                //!< код после for

   Command_while(CodePtr code, size_t sourceID, size_t line) : Command(sourceID, line) { _expr = 0; _code = code; }
   void     run(Int &ret, Value &retVal, ScriptPtr script = 0);//!< Выполнить комманду while
   ostream & print(ostream & out, ScriptPtr script = 0);   //!< Вывести в поток out
};
//=======================================================================================
//! Команда цикла for
// Существуют следующие сценарии работы команды:
// 1. Итерация по значениям массива / узлам графа
//    for varLeft in Array / Graph
//    где varLeft - значение элемента массива/узел графа
// 2. Итерация по ключам мэпа
//    for varLeft in Map
//    где varLeft - ключ мэпа
// 3. Итерация по ключам и значениям массива/мэпа
//    for varLeft,varRight in Array / Map
//    где varLeft - ключ/индекс, а varRight - значение
// 4. Итерация по узлам и значениям узлов графа
//    for varLeft,varRight in Graph
//    где varLeft - узел графа, а varRight - значение узла графа
// 5. Итерация по узлам текущего графа, удовлетворяющим выражению 
//    for varLeft in varLeft.isa.men
//    где varLeft - узел графа, а varRight - значение узла графа
class Command_for : public Command
{
public:
   VariablePtr _var1;                 //!< первая переменнная цикла
   VariablePtr _var2;                 //!< вторая переменнная цикла
   VariablePtr _var3;                 //!< третяя переменнная цикла
   VariablePtr _var4;                 //!< четвертая переменнная цикла
   VariablePtr _varRight;             //!< переменная цикла после запятой
   ExprPtr     _expr;                 //!< выражение условия цикла
   CodePtr     _code;                 //!< код после for
   Bool        _iterGraphExpr = false;//!< по узлам текущего графа, удовлетворяющим выражению 

   Command_for(CodePtr code, size_t sourceID, size_t line) :Command(sourceID, line) { _var1 = 0;  _var2 = 0; _var3 = 0; _var4 = 0; _expr = 0; _code = code; }
   void      run(Int &ret, Value &retVal, ScriptPtr script = 0);             //!< Выполнить комманду for
   bool      runFuncIter(Int &ret, Value &retVal, ScriptPtr script = 0);     //!< Запустить итератор по результату работы функции
   void      runGraphExprIter(Int &ret, Value &retVal, ScriptPtr script = 0);    //!< Запустить итератор по узлам текущего графа
   void      runGraphExprIterNodes(Int &ret, Value &retVal, ScriptPtr script = 0);    //!< Запустить итератор по узлам текущего графа
   void      runGraphExprIterEdges(Int &ret, Value &retVal, ScriptPtr script = 0);    //!< Запустить итератор по ребрам текущего графа
   void      runValIter(Int &ret, Value &retVal, ScriptPtr script = 0);      //!< Запустить итератор по результату выполнения выражения
   void      runValIterArr(  Value &iterVal, Int &ret, Value &retVal, ScriptPtr script = 0); //!< Запустить итератор по результату выполнения выражения для типа ARRAY
   void      runValIterMap(  Value &iterVal, Int &ret, Value &retVal, ScriptPtr script = 0); //!< Запустить итератор по результату выполнения выражения для типа MAP
   void      runValIterGraph(Value &iterVal, Int &ret, Value &retVal, ScriptPtr script = 0); //!< Запустить итератор по результату выполнения выражения для типа GRAPH
   void      runValIterGraphNodes(Value &iterVal, Int &ret, Value &retVal, ScriptPtr script = 0); //!< Запустить итератор по результату выполнения выражения для типа GRAPH
   void      runValIterGraphEdges(Value &iterVal, Int &ret, Value &retVal, ScriptPtr script = 0); //!< Запустить итератор по результату выполнения выражения для типа GRAPH
   ostream & print(ostream& out, ScriptPtr script = 0);    //!< Вывести в поток out
};

//=======================================================================================
//! Команда добавления глобальной переменной в локальное пространство видимимости
//
class Command_global : public Command
{
public:
   Command_global(size_t sourceID, size_t line) : Command(sourceID, line) {}
   //!<  Выполнить команду
   void      run(Int &ret, Value &retVal, ScriptPtr script = 0) {}
   ostream & print(ostream & out, ScriptPtr script = 0) { return out << "global"; }
};

#endif