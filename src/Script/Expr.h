/*!            Expr - Прототип всех выражений скрипта

Выражение - это деревесная структура. Её узлы являются наследниками Expr
и могут быть следующих видов: ExprConst - константа, ExprVar - указатель на переменную,
ExprOp - бинарная операция типа x & y, ExprFun - вычисление функции name(expr1,expr2,...)

Объявленные в модуле классы :
class Expr;                  // Прототип  узлов  дерева выражения
class ExprConst;             // Узел, хранящий логическую константу _const
class ExprVar;               // Узел, хранящий указатель _var на переменную
class ExprOp;                // Узел, бинарной операции
class ExprFun;               // Узел, хранящий вызываемую функцию

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ExprH
#define ExprH

#include "Value.h"
#include "Script.h"
#include "Variable.h"

//=======================================================================================
// Предварительное объявление классов:
SPTR_DEF(Script)
SPTR_DEF(Function)
SPTR_DEF(Expr)

//=======================================================================================
//! Абстрактный прородитель узлов (различных типов) дерева выражения
//
class Expr
{
public:
   virtual ~Expr() {}
   //! Разновидности выражений
   enum Kind { _UNKNOWN, _VAR, _CONST, _NODE_CONST, _ARRAY, _MAP, _BIN_OP, _AND, _OR, _IMPL, _EQUALITY, _NOT, _UNDEF, _FUN, _EQ, _NEQ, _INDX, _MULTIARY, _FIELD, _LOGIC, _FUSNUMBER, _EDGE };
   virtual Value run() = 0;                        //!< Вернуть значение выражения   
   virtual ostream& print(ostream& out) = 0;       //!< Вывести в поток, в виде текста
   virtual Value set(const Value &v) { return v; } //!< установка значений
   virtual Kind  kind() { return _UNKNOWN; }       //!< Разновидность выражения
   virtual void  root(bool val) { _root = val; }   //!< Выражение является корневым (не вложено в другое)
protected:
   bool  _root = false;                            //!< Признак того, что выражение я вляется корневым
};
//=======================================================================================
//! Узел, хранящий константу _const
//
SPTR_DEF(ExprConst)
class ExprConst : public Expr
{
public:
   Value _const;                                  //!< константа, хранимая в узле

   ExprConst(const Value & c) { _const = c; }

   Kind  kind() { return _CONST; }                 //!< Разновидность выражения  
   Value run() { return _const.copy(); }           //!< Возвращает копию значения константы

                                    //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      if (_const.type() == ValueBase::_STR)       // величина строкового типа
         return out << '\"' << _const << '\"';
      if (_const.type() == ValueBase::_LOGIC)     // величина логического типа
         return out << _const;
      //return out << _const << "[" << _const.type_str() << "]";   // все отстальные типы
      return out << _const;   // все отстальные типы
   }
};

//=======================================================================================
//! Узел, хранящий имя узла графа
//
SPTR_DEF(ExprNodeConst)
class ExprNodeConst : public Expr
{
public:
   ExprNodeConst(string &name, ScriptPtr script) :_name(name), _script(script), _evaluated(false) {}
   Kind  kind() { return _NODE_CONST; }          //!< разновидность выражения  
   Value run();                                  //!< возвращает целочисленное значение узла текущего графа

   ostream & print(ostream & out)
   {
      return out << _name;
   }
protected:
   Value   _val;                                 //!< значение узла
   bool    _evaluated;                           //!< получено ли значение узла по его имени
   string  _name;                                //!< имя константы   
   ScriptPtr _script;                              //!< ссылка на скрипт для доступа к текущему графу
};

//=======================================================================================
//! Узел, хранящий указатель _var на переменную
//
SPTR_DEF(ExprVar)
class ExprVar : public Expr
{
public:
   VariablePtr _var;           //!< указатель на переменную
   ExprVar(VariablePtr v) { _var = v; _var->used(true); }

   Expr::Kind  kind() { return Expr::_VAR; }      //!< Разновидность выражения

   //! Вернуть значение выражения
   Value run()
   {
#ifdef _DS_CHECK_POINTER
      if (!_var)
         return Logic();
#endif
      return _var->val();
   }

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      if (!_var)
         return out;
      return _var->print(out);
   }

   //!< Копирование значений
   Value set(const Value & v)
   {
      *_var = v;
      return _var->val();
   }

   //!< Получить переменную
   VariablePtr variable() { return _var; }
};

//=======================================================================================
//! Узел, хранящий индекс доступа к результату выражения
//
SPTR_DEF(ExprValElement)
class ExprValElement : public Expr
{
public:
   ExprPtr  _expr;          //!< выражение
   ExprPtr  _indx;          //!< индекс доступа к элементу результата выражения

   ExprValElement(ExprPtr expr, ExprPtr indx = 0) :_expr(expr), _indx(indx) {}

   Expr::Kind  kind() { return Expr::_INDX; }      //!< Разновидность выражения

                                                   //! Вернуть значение выражения
   Value run();

   //!< Копирование значений
   Value set(const Value & v);

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      if (!_expr)
         return out;
      if (!_indx)
         return _expr->print(out);
      _expr->print(out); out << "["; _indx->print(out); out << "]";
      return out;
   }
};


//=======================================================================================
//! Узел, хранящий объект и имя поля
//
SPTR_DEF(ExprValueField)
class ExprValueField : public Expr
{
public:
   ExprPtr     _valExpr;       //!< выражение, вычисляющее значение
   ExprPtr     _fieldExpr;     //!< выражение, вычисляющее имя поля
   string      _fieldName;     //!< имя поля значения

   ExprValueField(ExprPtr valExpr, ExprPtr  fieldExpr) : _valExpr(valExpr), _fieldExpr(fieldExpr), _fieldEvaluated(false) {}
   Expr::Kind  kind() { return Expr::_FIELD; }           //!< Разновидность выражения
                                                  
   Value run();                                          //! Вернуть значение выражения

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      if (!_valExpr)
         return out;
      out << "("; _valExpr->print(out); out << ".";
      _fieldExpr->print(out) << ")";
      return out;
   }
private:
   Value     _field;                   //!< поле
   bool      _fieldEvaluated;          //!< поле кэшировано
};

//=======================================================================================
//! Узел выражения, выполняющий логическое отрицание
//
SPTR_DEF(ExprNot)
class ExprNot : public Expr
{
public:
   ExprPtr _expr;       //!< отрицаемое выражение
   ExprNot(ExprPtr expr = 0) { _expr = expr; }
   Kind  kind() { return _NOT; }

   //! Вернуть значение выражения
   Value run() { return !_expr->run(); }
   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "!(";
      if (_expr)
         _expr->print(out);
      return  out << ")";
   }
};

//=======================================================================================
//! Узел выражения, выполняющий унарное арифметическое отрицание
//
SPTR_DEF(ExprPreMinus)
class ExprPreMinus : public Expr
{
public:
   ExprPtr _expr;       //!< отрицаемое выражение
   ExprPreMinus(ExprPtr expr = 0) { _expr = expr; }

   //! Вернуть значение выражения
   Value run() { return Value(Float(0.0f - _expr->run().get_Float())); }
   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "-(";
      if (_expr)
         _expr->print(out);
      return  out << ")";
   }
};

//=======================================================================================
//! Узел выражения, выполняющий проверку на нуопределенность
//
SPTR_DEF(ExprUndef)
class ExprUndef : public Expr
{
public:
   ExprPtr _expr;       //!< отрицаемое выражение
   ExprUndef(ExprPtr expr = 0) { _expr = expr; }
   Kind  kind() { return _UNDEF; }

   //! Вернуть значение выражения
   Value run() { return _expr->run() == Logic::Undef; }
   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "?(";
      if (_expr)
         _expr->print(out);
      return  out << ")";
   }
};

//========================================================================================
//! Узел выражения, выполняющий инкремент переменной и возвращающий значение до инкремента
//
SPTR_DEF(ExprVarIncr)
class ExprVarIncr : public Expr
{
   VariablePtr _var;           //!< указатель на переменную
public:
   ExprVarIncr(VariablePtr var) { _var = var; }

   //! Вернуть значение выражения
   Value run() { return _var->val()++;}

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";
      _var->print(out);
      return  out << ")++";
   }
};

//========================================================================================
//! Узел выражения, выполняющий сложение и присваивание результата
//
SPTR_DEF(ExprVarPlusAssign)
class ExprVarPlusAssign : public Expr
{
   VariablePtr _var  = 0;           //!< указатель на переменную
   ExprPtr     _expr = 0;           //!< выражение, вычисляющее аргумент операции
public:
   ExprVarPlusAssign(VariablePtr var, ExprPtr  expr) : _var(var), _expr(expr) {}
   //! Вернуть значение выражения
   Value run();

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";
      _var->print(out);
      out << "+=";
      _expr->print(out);
      return out << ")";
   }
};

//========================================================================================
//! Узел выражения, выполняющий вычитание и присваивание результата
//
SPTR_DEF(ExprVarMinusAssign)
class ExprVarMinusAssign : public Expr
{
   VariablePtr _var  = 0;           //!< указатель на переменную
   ExprPtr     _expr = 0;          //!< выражение, вычисляющее аргумент операции
public:
   ExprVarMinusAssign(VariablePtr var, ExprPtr  expr) : _var(var), _expr(expr) {}

   //! Вернуть значение выражения
   Value run();

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";
      _var->print(out);
      out << "-=";
      _expr->print(out);
      return out << ")";
   }
};

//========================================================================================
//! Узел выражения, выполняющий умножение и присваивание результата
//
SPTR_DEF(ExprVarDotAssign)
class ExprVarDotAssign : public Expr
{
   VariablePtr _var = 0;           //!< указатель на переменную
   ExprPtr     _expr = 0;          //!< выражение, вычисляющее аргумент операции
public:
   ExprVarDotAssign(VariablePtr var, ExprPtr  expr) : _var(var), _expr(expr) {}

   //! Вернуть значение выражения
   Value run();

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";
      _var->print(out);
      out << "*=";
      _expr->print(out);
      return out << ")";
   }
};

//========================================================================================
//! Узел выражения, выполняющий вещественное деление и присваивание результата
//
SPTR_DEF(ExprVarDivFloatAssign)
class ExprVarDivFloatAssign : public Expr
{
   VariablePtr _var = 0;           //!< указатель на переменную
   ExprPtr     _expr = 0;          //!< выражение, вычисляющее аргумент операции
public:
   ExprVarDivFloatAssign(VariablePtr var, ExprPtr  expr) : _var(var), _expr(expr) {}
   //! Вернуть значение выражения
   Value run();

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";
      _var->print(out);
      out << "/=";
      _expr->print(out);
      return out << ")";
   }
};

//========================================================================================
//! Узел выражения, выполняющий целочисленное деление и присваивание результата
//
SPTR_DEF(ExprVarDivIntAssign)
class ExprVarDivIntAssign : public Expr
{
   VariablePtr _var = 0;           //!< указатель на переменную
   ExprPtr     _expr = 0;          //!< выражение, вычисляющее аргумент операции
public:
   ExprVarDivIntAssign(VariablePtr var, ExprPtr  expr) : _var(var), _expr(expr) {}

   //! Вернуть значение выражения
   Value run();

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";
      _var->print(out);
      out << "\\=";
      _expr->print(out);
      return out << ")";
   }
};


//========================================================================================
//! Узел выражения, выполняющий деление по модулю и присваивание результата
//
SPTR_DEF(ExprVarModAssign)
class ExprVarModAssign : public Expr
{
   VariablePtr _var = 0;           //!< указатель на переменную
   ExprPtr     _expr = 0;          //!< выражение, вычисляющее аргумент операции
public:
   ExprVarModAssign(VariablePtr var, ExprPtr  expr) : _var(var), _expr(expr) {}

   //! Вернуть значение выражения
   Value run();

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";
      _var->print(out);
      out << "%=";
      _expr->print(out);
      return out << ")";
   }
};


//========================================================================================
//! Узел выражения, выполняющий декримент переменной и возвращающий значение до декримента
//
SPTR_DEF(ExprVarDecr)
class ExprVarDecr : public Expr
{
   VariablePtr _var;           //!< указатель на переменную
public:
   ExprVarDecr(VariablePtr var) { _var = var; }

   //! Вернуть значение выражения
   Value run() { return _var->val()--; }

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";
      _var->print(out);
      return  out << ")--";
   }
};
//=======================================================================================
//! Прототип узла выражения, храняющего бинарную операцию
//
SPTR_DEF(ExprOp)
class ExprOp : public Expr
{
public:
   ExprPtr _lf, _rt;                          //!< левый и правый аргумет операции (ветки дерева)
   ExprOp() { _lf = _rt = 0; }
   Expr::Kind  kind() { return _BIN_OP; }     //!< Разновидность выражения

                                              //! Вернуть значение выражения
   Value run() { return Logic(); }
   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out) { return out; }

   //! Проверить типы аргументов бинарной операции
   Bool wrong_args(Value &x, Value &y)
   {
      // 20181227 - разрешил операции над нодами для "строка" + нода
      // 20190130 - разрешил то же самое с ребрами
      //if (/*x.type() == ValueBase::_NODE || */
      //    x.type() == ValueBase::_EDGE       ||
      //    /*y.type() == ValueBase::_NODE ||*/
      //    y.type() == ValueBase::_EDGE
      //   ) 
      //{
      //   TRACE_CRITICAL << "? Wrong types x,y in binary operation.\n";
      //   return true;
      //}
      return false;
   }
};
//=======================================================================================
//! Узел выражения, выполняющий сложение
//
SPTR_DEF(ExprPlus)
class ExprPlus : public ExprOp
{
public:
   ExprPlus(ExprPtr lf = 0, ExprPtr rt = 0) { _lf = lf; _rt = rt; }

   //! Вернуть значение выражения
   Value run()
   {
      Value x = _lf->run(), y = _rt->run();
#ifdef _DS_CHECK_TYPE
      if (wrong_args(x, y))
         return Float(0);
#endif 
      return Value(x + y);
   }

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "("; if (_lf) _lf->print(out); out << " + ";  if (_rt) _rt->print(out);
      return out << ")";
   }
};
//=======================================================================================
//! Узел выражения, выполняющий вычитание
//
SPTR_DEF(ExprMinus)
class ExprMinus : public ExprOp
{
public:
   ExprMinus(ExprPtr lf = 0, ExprPtr rt = 0) { _lf = lf; _rt = rt; }

   //! Вернуть значение выражения
   Value run()
   {
      Value x = _lf->run(), y = _rt->run();
#ifdef _DS_CHECK_TYPE
      if (wrong_args(x, y))
         return Float(0);
#endif 
      return x - y;
   }

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "("; if (_lf) _lf->print(out); out << " - ";  if (_rt) _rt->print(out);
      return out << ")";
   }
};
//=======================================================================================
//! Узел выражения, выполняющий умножение
//
SPTR_DEF(ExprMult)
class ExprMult : public ExprOp
{
public:
   ExprMult(ExprPtr lf = 0, ExprPtr rt = 0) { _lf = lf; _rt = rt; }

   //! Вернуть значение выражения
   Value run()
   {
      Value x = _lf->run(), y = _rt->run();
#ifdef _DS_CHECK_TYPE
      if (wrong_args(x, y))
         return Float(0);
#endif 
      return x * y;
   }

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "("; if (_lf) _lf->print(out); out << " * ";  if (_rt) _rt->print(out);
      return out << ")";
   }
};
//=======================================================================================
//! Узел выражения, выполняющий вещественное деление
//
SPTR_DEF(ExprDiv)
class ExprDiv : public ExprOp
{
public:
   ExprDiv(ExprPtr lf = 0, ExprPtr rt = 0) { _lf = lf; _rt = rt; }

   //! Вернуть значение выражения
   Value run()
   {
      Value x = _lf->run(), y = _rt->run();
#ifdef _DS_CHECK_TYPE
      if (wrong_args(x, y)) 
         return Float(0);
      if (y.type() == ValueBase::_FLOAT && y.get_Float() == 0)
      {
         TRACE_CRITICAL << "division by zero" << endl;
         return Float(0);
      }         
#endif 
      return x / y;
   }

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "("; if (_lf) _lf->print(out); out << " / ";  if (_rt) _rt->print(out);
      return out << ")";
   }
};
//=======================================================================================
//! Узел выражения, выполняющий целочисленное деление
//
SPTR_DEF(ExprDivInt)
class ExprDivInt : public ExprOp
{
public:
   ExprDivInt(ExprPtr lf = 0, ExprPtr rt = 0) { _lf = lf; _rt = rt; }

   //! Вернуть значение выражения
   Value run()
   {
      Value x = _lf->run(), y = _rt->run();
#ifdef _DS_CHECK_TYPE
      if (wrong_args(x, y))
         return Float(0);
      if (y.type() == ValueBase::_FLOAT && y.get_Float() == 0)
      {
         TRACE_CRITICAL << "division by zero" << endl;
         return Float(0);
      }
#endif 
      return x.divInt(y);
   }

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "("; if (_lf) _lf->print(out); out << " // ";  if (_rt) _rt->print(out);
      return out << ")";
   }
};
//=======================================================================================
//! Узел выражения, выполняющий деление
//
SPTR_DEF(ExprMod)
class ExprMod: public ExprOp
{
public:
   ExprMod(ExprPtr lf = 0, ExprPtr rt = 0) { _lf = lf; _rt = rt; }

   //! Вернуть значение выражения
   Value run()
   {
      Value x = _lf->run(), y = _rt->run();
#ifdef _DS_CHECK_TYPE
      if (wrong_args(x, y))
         return Float(0);
      if (y.type() == ValueBase::_FLOAT && y.get_Float() == 0)
      {
         TRACE_CRITICAL << "division by zero" << endl;
         return Float(0);
      }
#endif 
      return x % y;
   }

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "("; if (_lf) _lf->print(out); out << " % ";  if (_rt) _rt->print(out);
      return out << ")";
   }
};
//=======================================================================================
//! Узел выражения, выполняющий логическое ИЛИ
//
SPTR_DEF(ExprOr)
class ExprOr : public ExprOp, SPTR_ENABLE_FROM_THIS(ExprOr)
{
public:
   ExprOr(ExprPtr lf = 0, ExprPtr rt = 0) { _lf = lf; _rt = rt; }
   Kind  kind() { return _OR; }

   //! Вернуть значение выражения
   Value run();

   //! Установить значение выражения
   Value set(const Value &v);

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";  if (_lf) _lf->print(out); out << " | ";  if (_rt) _rt->print(out);
      return out << ")";
   }
};
//=======================================================================================
//! Узел выражения, выполняющий логическое И
//
SPTR_DEF(ExprAnd)
class ExprAnd : public ExprOp
{
public:
   ExprAnd(ExprPtr lf = 0, ExprPtr rt = 0) { _lf = lf; _rt = rt; }
   Kind  kind() { return _AND; } 
   //! Вернуть значение выражения
   Value run()
   {
      Value x = _lf->run(), y = _rt->run();
#ifdef _DS_CHECK_TYPE
      if (wrong_args(x, y) || (y.type() == ValueBase::_FLOAT && y.get_Float() == 0))
         return Float(0);
#endif 
      return  x & y;
   }

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";  if (_lf) _lf->print(out); out << " & "; if (_lf) _rt->print(out);
      return out << ")";
   }
};
//=======================================================================================
//! Узел выражения, выполняющий логическую эквивалентность
//
SPTR_DEF(ExprEq)
class ExprEq : public ExprOp
{
public:
   ExprEq(ExprPtr lf = 0, ExprPtr rt = 0) { _lf = lf; _rt = rt; }
   Kind  kind() { return _EQ; }

   //! Вернуть значение выражения
   Value run() { return _lf->run() == _rt->run(); }

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";
      if (_lf) _lf->print(out); out << " == "; if (_rt) _rt->print(out);
      out << ")";
      return out;
   }
};
//=======================================================================================
//! Узел выражения, выполняющий логическую импликацию
//
SPTR_DEF(ExprImpl)
class ExprImpl : public ExprOp, SPTR_ENABLE_FROM_THIS(ExprImpl)
{   
public:
   ExprImpl(ExprPtr lf = 0, ExprPtr rt = 0) { _lf = lf; _rt = rt; }
   Kind  kind() { return _IMPL; }

   //! Вернуть значение выражения
   Value run();

   //! Установить значение выражения
   Value set(const Value &v);

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";
      if (_lf) _lf->print(out); out << " -> "; if (_rt) _rt->print(out);
      out << ")";
      return out;
   }
};

//=======================================================================================
//! Узел выражения, выполняющий логическую эквивалентность
//
SPTR_DEF(ExprEquality)
class ExprEquality : public ExprOp
{
public:
   ExprEquality(ExprPtr lf = 0, ExprPtr rt = 0) { _lf = lf; _rt = rt; }
   Kind  kind() { return _EQUALITY; }

   //! Вернуть значение выражения
   Value run();

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";
      if (_lf) _lf->print(out); out << " <-> "; if (_rt) _rt->print(out);
      out << ")";
      return out;
   }
};

//=======================================================================================
//! Узел выражения, выполняющий мультиарный выбор
// В тернарном виде      E ? : A : B - если E истина, то A, иначе B
// В кватернарном виде   E ? : A : B : C - если E истина, то A, если E ложь, то B, иначе С
// В квантиринарном виде E ? : A : B : C : D - если E истина, то A, если E ложь, то B, если E не определено, то С, иначе D
//
SPTR_DEF(ExprMultiary)
class ExprMultiary : public Expr
{
   ExprPtr _condExpr;             //!< условие выбора (E)
   ExprPtr _secondArity;          //!< выражение, выполняемое если _condExpr - истинно (A)
   ExprPtr _thirdArity;           //!< выражение, выполняемое если (нет _forthArity и _condExpr не истинно) либо (есть _forthArity и _condExpr ложно)
   ExprPtr _forthArity;           //!< выражение, выполняемое если (нет _fifthArity и _condExpr не истинно и не ложно) либо (есть _fifthArity и _condExpr не определено)
   ExprPtr _fifthArity;           //!< выражение, выполняемое если _condExpr не истинно, не ложно и определено (например Logic(0.2,0.8))
public:
   ExprMultiary(ExprPtr condExpr,
      ExprPtr secondArity,
      ExprPtr thirdArity,
      ExprPtr forthArity = 0,
      ExprPtr fifthArity = 0
   ) :
      _condExpr(condExpr),
      _secondArity(secondArity),
      _thirdArity(thirdArity),
      _forthArity(forthArity),
      _fifthArity(fifthArity) {}

   Kind  kind() { return _MULTIARY; }

   //! Вернуть значение выражения
   Value run();

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out);
};

//=======================================================================================
//! Узел выражения, выполняющий логическую неэквивалентность
//
SPTR_DEF(ExprNeq)
class ExprNeq : public ExprOp
{
public:
   ExprNeq(ExprPtr lf = 0, ExprPtr rt = 0) { _lf = lf; _rt = rt; }
   Kind  kind() { return _NEQ; }

   //! Вернуть значение выражения
   Value run() { return _lf->run() != _rt->run(); }

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      if (_lf) _lf->print(out); out << " != "; if (_rt)  _rt->print(out);
      return out;
   }
};
//=======================================================================================
//! Узел выражения, выполняющий операцию меньше
//
SPTR_DEF(ExprLt)
class ExprLt : public ExprOp
{
public:
   ExprLt(ExprPtr lf = 0, ExprPtr rt = 0) { _lf = lf; _rt = rt; }

   //! Вернуть значение выражения
   Value run() { return _lf->run() < _rt->run(); }

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";
      if (_lf) _lf->print(out); out << " < "; if (_rt) _rt->print(out);
      return out << ")";
   }
};
//=======================================================================================
//! Узел выражения, выполняющий операцию меньше или равно
//
SPTR_DEF(ExprLtEq)
class ExprLtEq : public ExprOp
{
public:
   ExprLtEq(ExprPtr lf = 0, ExprPtr rt = 0) { _lf = lf; _rt = rt; }

   //! Вернуть значение выражения
   Value run() { return _lf->run() <= _rt->run(); }

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";
      if (_lf) _lf->print(out); out << " <= "; if (_rt) _rt->print(out);
      return out << ")";
   }
};
//=======================================================================================
//! Узел выражения, выполняющий операцию больше
//
SPTR_DEF(ExprGt)
class ExprGt : public ExprOp
{
public:
   ExprGt(ExprPtr lf = 0, ExprPtr rt = 0) { _lf = lf; _rt = rt; }

   //! Вернуть значение выражения
   Value run() { return _lf->run() > _rt->run(); }

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";
      if (_lf) _lf->print(out); out << " > "; if (_rt) _rt->print(out);
      return out << ")";
   }
};
//=======================================================================================
//! Узел выражения, выполняющий операцию больше или равно
//
SPTR_DEF(ExprGtEq)
class ExprGtEq : public ExprOp
{
public:
   ExprGtEq(ExprPtr lf = 0, ExprPtr rt = 0) { _lf = lf; _rt = rt; }

   //! Вернуть значение выражения
   Value run() { return _lf->run() >= _rt->run(); }

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";
      if (_lf) _lf->print(out); out << " >= "; if (_rt) _rt->print(out);
      return out << ")";
   }
};
//=======================================================================================
//! Узел выражения, выполняющий присваивание в переменную
//
SPTR_DEF(ExprAssign)
class ExprAssign : public ExprOp
{
public:
   ExprAssign(ExprPtr lf = 0, ExprPtr rt = 0) { _lf = lf; _rt = rt; }

   //! Вернуть значение выражения
   Value run() 
   {
      return _lf->set(_rt->run()); 
   } 

   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";
      if (_lf) _lf->print(out); out << " = "; if (_rt) _rt->print(out);
      out << ")";
      return out;
   }
};

//=======================================================================================
//! Узел, хранящий массив выражений
//
SPTR_DEF(ExprArray)
class ExprArray : public Expr
{
public:
   vector<ExprPtr >  _exprs;         //!< массив выражений

   Kind     kind() { return _ARRAY; }//!< Разновидность выражения
   Value    run();                 //!< Вернуть значения массива
   ostream & print(ostream & out); //!< Вывести функцию в поток, в виде текста программы
};
//=======================================================================================
//! Узел, хранящий массив пар для мэпа
//
SPTR_DEF(ExprMap)
class ExprMap : public Expr
{
public:
   /// выражение, вычисляющее значение пары
   class ExprMapPir {
   public:
      ExprMapPir(ExprPtr keyExpr, ExprPtr valExpr) : _keyExpr(keyExpr), _valExpr(valExpr) {}
      ExprPtr _keyExpr;              //!< выражение вычисляющее имя ключа
      ExprPtr _valExpr;              //!< выражение вычисляющее значение ключа
   };
   vector<ExprMapPir>  _exprs;     //!< массив выражений

   Kind     kind() { return _MAP; }//!< Разновидность выражения
   Value    run();                 //!< Вернуть значения массива
   ostream & print(ostream & out); //!< Вывести функцию в поток, в виде текста программы
};
//=======================================================================================
//! Узел, вызывающий функцию
/*! Функция может быть обычной fun(X,Y) или методом переменной Obj.fun(...)
В последнем случае указатель _obj должен быть не нулевой
*/
SPTR_DEF(ExprFun)
class ExprFun : public Expr
{
public:
   FunctionPtr       _fun;           //!< ссылка на демон из массива Script._functions
   vector<ExprPtr>   _args;          //!< аргументы функции (выражения)
   ExprPtr           _obj;           //!< объект к которому применяется функция

   ExprFun() { _fun = 0; _obj = 0; }
   Kind  kind() { return _FUN; }

   FunctionPtr   getFunction();         //!< Получиь функцию
   Value       run();                 //!< Вернуть значение функции   
   Value       set(const Value &v);   //!< установка значений
   ostream &   print(ostream & out); //!< Вывести массив в поток, в виде текста программы   
};

//=======================================================================================
//! Узел выражения, вычисляющий значение логической переменной
//
SPTR_DEF(ExprLogic)
class ExprLogic : public Expr
{
   ExprPtr _p0Expr;  //!< выражение, вычисляющее вероятность p0
   ExprPtr _p1Expr;  //!< выражение, вычисляющее вероятность p1
public:
   ExprLogic(ExprPtr p0Expr, ExprPtr p1Expr) :_p0Expr(p0Expr), _p1Expr(p1Expr) {}
   Kind  kind() { return _LOGIC; }
   //! Вернуть значение выражения
   Value run()
   {
      Value p0 = _p0Expr->run();
      Value p1 = _p1Expr->run();
#ifdef _DS_CHECK_TYPE
      if (p0.type() != ValueBase::_FLOAT || p1.type() != ValueBase::_FLOAT)
         return Logic();
#endif 
      return  Logic(p0.get_Float(),p1.get_Float());
   }
   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";  if (_p0Expr) _p0Expr->print(out); out << ","; if (_p1Expr) _p1Expr->print(out);
      return out << ")";
   }
};
//=======================================================================================
//! Узел выражения, вычисляющий значение логической переменной
//
SPTR_DEF(ExprFus)
class ExprFus : public Expr
{
   ExprPtr _v0Expr = 0;  //!< выражение, вычисляющее вероятность p0
   ExprPtr _c0Expr = 0;  //!< выражение, вычисляющее вероятность p1
   ExprPtr _c1Expr = 0;  //!< выражение, вычисляющее вероятность p1
   ExprPtr _v1Expr = 0;  //!< выражение, вычисляющее вероятность p1
public:
   ExprFus(ExprPtr v0Expr, ExprPtr c0Expr, ExprPtr c1Expr, ExprPtr v1Expr) :
      _v0Expr(v0Expr), _c0Expr(c0Expr), _c1Expr(c1Expr), _v1Expr(v1Expr)
   {}
   Kind  kind() { return _FUSNUMBER; }
   //! Вернуть значение выражения
   Value run()
   {
      if (!_v1Expr)
      {
         _v1Expr = _c1Expr;
         _c1Expr = _c0Expr;
      }
      Value v0 = _v0Expr->run();
      Value c0 = _c0Expr->run();
      Value c1 = _c1Expr->run();
      Value v1 = _v1Expr->run();
#ifdef _DS_CHECK_TYPE
      if (v0.type() != ValueBase::_FLOAT || c0.type() != ValueBase::_FLOAT || c1.type() != ValueBase::_FLOAT || v1.type() != ValueBase::_FLOAT)
         return FusNumber(0, 0, 0, 0);
#endif 
      return  FusNumber(v0.get_Float(), c0.get_Float(), c1.get_Float(), v1.get_Float());
   }
   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";  
      if (_v0Expr) 
         _v0Expr->print(out); 
      out << ","; 
      if (_c0Expr) 
         _c0Expr->print(out);
      out << ",";
      if (_c1Expr)
         _c1Expr->print(out);
      out << ",";
      if (_v1Expr)
         _v1Expr->print(out);
      return out << ")";
   }
};

//=======================================================================================
//! Узел выражения, вычисляющий/устанавливающий значение ребра графа 
//
SPTR_DEF(ExprEdge)
class ExprEdge : public Expr
{
   ExprPtr _node1Expr  = 0;        //!< выражение, вычисляющее узел из которого устанавливаем связь
   ExprPtr _edgeExpr   = 0;        //!< выражение, вычисляющее тип отношения
   ExprPtr _node2Expr  = 0;        //!< выражение, вычисляющее узел в который устанавливаем связь
   Logic _val = Logic::True;     //!< значение, которое устанавливаем в ребро
public:
   ExprEdge(ExprPtr node1Expr, ExprPtr edgeExpr, ExprPtr node2Expr, const Logic &val) :
      _node1Expr(node1Expr), _edgeExpr(edgeExpr), _node2Expr(node2Expr), _val(val)
   {}
   Kind  kind() { return _EDGE; }
   //! Вернуть значение выражения
   Value run();
   //! Установить значение выражения
   Value set(const Value &v);
   //! Установить ребро из узла в узел
   Value setExprEdge(Int edge, Value &node1, ExprPtr node2Expr, const Value &val);
   //! Получить значение ребра
   Logic getExprEdge(Int edge, Value &node1, ExprPtr node2Expr);
   //! Установить ребро из узла в узел
   Value setEdge(Int edge, Value &node1, Value &node2, const Value &val);
   //! Получить значения ребра
   Logic getEdge(Int edge, Value & node1, Value & node2);
   //! Вывести выражение в поток, в виде текста (программы)
   ostream & print(ostream & out)
   {
      out << "(";
      if (_node1Expr)
         _node1Expr->print(out);
      out << " ";
      if (_val == Logic::False)
         out << "!";
      if (_edgeExpr)
         _edgeExpr->print(out);
      out << " ";
      if (_node2Expr)
         _node2Expr->print(out);
      return out << ")";
   }
   friend class MindVarAxiom;
   friend class Atom;
};


#endif