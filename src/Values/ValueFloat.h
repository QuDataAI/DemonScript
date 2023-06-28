/*!                           Вещественная переменная


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ValueFloatH
#define ValueFloatH

#include "ValueBase.h"

SPTR_DEF(ValueFloat);

//=======================================================================================
//! Вещественный тип
// 
class ValueFloat : public ValueBase
{
   Float _val;
public:
   ValueFloat(Float val) : _val(val) {}
   /*!
      Вывести в поток out текущее значение
      \param поток в который выводим значение
      \return результирующий поток
   */
   ostream& print(ostream& out) { return out << _val; }
   /*!
      Тип значения в строковом виде
      \return тип значения в строковом виде
   */
   string   type_str() { return "float"; }
   /*!
      Тип значения в числовом виде
      \return тип значения в числовом виде
   */
   Kind     type() { return _FLOAT; }
   /*!
      Проверить соответсвие типа значения вещественному типу
      \return результат проверки
   */
   bool    checkType(const Float &val) { return true; }
   /*!
      Операция равенства ==
      \param v значение c которым выполняется операция
      \return результат операции
   */
   Logic eq(ValueBase * v);
   /*!
      Операция неравенства != 
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   Logic neq(ValueBase * v);
   /*!
      Операция меньше < 
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   Logic lt(ValueBase * v);
   /*!
      Операция меньше или равно <= 
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   Logic lteq(ValueBase * v);
   /*!
      Операция больше > 
      \param v значение c которым выполняется операция 
      \return результат операции
   */ 
   Logic gt(ValueBase * v);
   /*!
      Операция больше или равно >= 
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   Logic gteq(ValueBase * v);
   /*!
      Операция +
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   shared_ptr<ValueBase> plus(ValueBase * v)
   {
      return shared_ptr<ValueBase>(new ValueFloat(_val + ((ValueFloat *)v)->_val));
   }
   /*!
      Операция -
      \param v значение c которым выполняется операция 
      \return результат операции
   */ 
   shared_ptr<ValueBase>  minus(ValueBase * v)
   {
      return shared_ptr<ValueBase>(new ValueFloat(_val - ((ValueFloat *)v)->_val));
   }
   /*!
      Операция *
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   shared_ptr<ValueBase>  mult(ValueBase * v)
   {
      return shared_ptr<ValueBase>(new ValueFloat(_val * (((ValueFloat *)v)->_val)));
   }
   /*!
      Операция вещественного деления /
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   shared_ptr<ValueBase>  div(ValueBase * v)
   {
      return shared_ptr<ValueBase>(new ValueFloat(_val / (((ValueFloat *)v)->_val)));
   }
   /*!
   Операция целочисленного деления //
   \param v значение c которым выполняется операция
   \return результат операции
   */
   shared_ptr<ValueBase>  divInt(ValueBase * v);
   /*!
      Присвоить вещественное значение
      \param val вещественное значение
   */
   void operator=(const Float &val) { _val = val; }
   /*!
   Присвоить переменной Value
   \param val значение
   */
   void operator=(const shared_ptr<ValueBase> &val) { _val = val->get_Float(); };
   /*!
      Инкрементировать значение переменной
   */
   void operator ++ (int v)   { _val = _val + 1.; }
   /*!
      Декриментировать значение переменной
   */
   void operator -- (int v)   { _val = _val - 1.; }
   /*!
      Операция %
      \param v значение c которым выполняется операция
      \return результат операции
   */
   shared_ptr<ValueBase>  mod(const shared_ptr<ValueBase> &val);
   /*!
      Установить вещественное значение в параметр v
      \param[out] v параметр в который будет установлено текущее значение 
   */
   void set(Float &v) { v = _val; }
   /*!
      Получить вещественное значение
      \return вещественное значение
   */
   Float  get_Float() { return _val; }
   /*!
      Получить целочисленное беззнаковое значение
      \return целочисленное беззнаковое значение
   */
   UInt    get_UInt() { return (UInt)_val; }
   /*!
      Получить целочисленное знаковое значение
      \return целочисленное знаковое значение
   */
   Int     get_Int() { return (Int)_val; }
   /*!
      Получить логическое значение
      \return логическое значение
   */
   Logic   get_Logic() { return _val>0.0?(Logic::True):(Logic::False); }
   /*!
      Установить вещественное значение
      \param вещественное значение
   */
   void  set_Float(const Float &val) { _val = val; }
   /*!
      Получить строковое значение
      \return строковое значение
   */
   string  get_Str() { return toString(); }
   /*!
      Получить копию значения
      \return копия значения
   */
   shared_ptr<ValueBase> copy() const { return make_shared<ValueFloat>(_val); }
   /*!
      Конвертировать значение в строку
      \return строковое значение
   */
   string toString();
   /*!
      Конвертировать значение в число
      \return числовое значение если конвертация возможна, иначе None
   */
   shared_ptr<ValueBase> toFloat();
   /*!
   Получить Fus Number
   \return Fus Number
   */
   virtual FusNumber   get_FusNumber();
};

#endif 