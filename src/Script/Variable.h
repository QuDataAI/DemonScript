/*!            Variable - Переменная скрипта

Хранит логическое значение и имя переменной.
Узлы выражения ExprVar хранят указатель на переменную,
поэтому её изменение в одном месте, будет сказываться в других.

Объявленные в модуле классы:
class Variable;              // Переменная скрипта (значение и имя переменной)

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef VariableH
#define VariableH

#include "Value.h"

SPTR_DEF(Variable)

//=======================================================================================
//! Переменная скрипта. Хранит логическое значение и имя переменной.
class Variable
{
public:
   Value  _val;              //!< значение переменной
   string _name;             //!< имя переменной
   Bool   _used = false;     //!< переменная используется

   Variable(const string &name = "") { _name = name; }
   Variable(Value& val, const string& name) { _val = val; _name = name; }
   Variable(UInt   val, const string& name) { _val = val; _name = name; }
   Variable(Int    val, const string& name) { _val = val; _name = name; }
   Variable(Float  val, const string& name) { _val = val; _name = name; }
   Variable(Float  p0, Float p1, const string& name) { _val = Logic(p0, p1); _name = name; }

   Value        & val() { return _val; }          //!< значение   
   const string & name() { return _name; }        //!< имя переменной
   void           used(Bool v) { _used = v; }     //!< установить флаг использования переменной 
   Bool           used() { return _used; }        //!< получить флаг использования переменной 

   Value & operator = (const Value & val) { return _val = val; } //!< присвоить значение

                                                           //! Вывести в поток out переменную в виде текста (программы)
   ostream & print(ostream & out) { return out << _name; }
};

#endif