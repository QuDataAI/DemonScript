/*!                           Логическая переменная


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ValueFusH
#define ValueFusH

#include "ValueBase.h"
#include "ValueFloat.h"
#include "ValueNone.h"
#include "ValueIndexRef.h"

SPTR_DEF(ValueFus);

//=======================================================================================
//! Логический тип
// 
class ValueFus : public ValueBase
{
public:
   FusNumber _val;

   ValueFus(const FusNumber & val) : _val(val) {}
   ostream& print(ostream& out) { return out << _val; }
   string   type_str() { return "fuzzy"; }
   Kind     type() { return _FUSNUMBER; }
   //! Операция равенства == 
   Logic eq(ValueBase * v)
   {
      if (v->type() != type())                  // типы не совпадают
         return Logic(1, 0);
      return _val == ((ValueFus *)v)->_val ? Logic(0, 1) : Logic(1, 0);
   }
   //! Операция неравенства != 
   Logic neq(ValueBase * v)
   {
      if (v->type() != type())                  // типы не совпадают
         return Logic(0, 1);
      return _val != ((ValueFus *)v)->_val ? Logic(0, 1) : Logic(1, 0);
   }
   //! Операция меньше < 
   Logic lt(ValueBase * v)
   {
      if (v->type() != type())                  // типы не совпадают
         return Logic(0, 0);                    // значение неопределено
      return Logic(0, 0);                       // ?
   }
   //! Операция меньше или равно <= 
   Logic lteq(ValueBase * v)
   {
      if (v->type() != type())                  // типы не совпадают
         return Logic(0, 0);                    // значение неопределено
      return Logic(0, 0);                       // ?
   }
   //! Операция больше > 
   Logic gt(ValueBase * v)
   {
      if (v->type() != type())                  // типы не совпадают
         return Logic(0, 0);                    // значение неопределено
      return Logic(0, 0);                       // ?   
   }
   //! Операция больше или равно >= 
   Logic gteq(ValueBase * v)
   {
      if (v->type() != type())                  // типы не совпадают
         return Logic(0, 0);                    // значение неопределено
      return Logic(0, 0);                       // ?
   }

   /*!
   Операция +
   \param v значение c которым выполняется операция
   \return результат операции
   */
   shared_ptr<ValueBase> plus(ValueBase * v)
   {            
      return shared_ptr<ValueBase>(new ValueFus(_val + v->get_FusNumber()));
   }
   /*!
   Операция -
   \param v значение c которым выполняется операция
   \return результат операции
   */
   shared_ptr<ValueBase>  minus(ValueBase * v)
   {
      return shared_ptr<ValueBase>(new ValueFus(_val - v->get_FusNumber()));
   }
   /*!
   Операция *
   \param v значение c которым выполняется операция
   \return результат операции
   */
   shared_ptr<ValueBase>  mult(ValueBase * v)
   {
      return shared_ptr<ValueBase>(new ValueFus(_val * v->get_FusNumber()));
   }
   /*!
   Операция вещественного деления /
   \param v значение c которым выполняется операция
   \return результат операции
   */
   shared_ptr<ValueBase>  div(ValueBase * v)
   {
      return shared_ptr<ValueBase>(new ValueFus(_val / v->get_FusNumber()));
   }

   /*!
   Получить Fus Number
   \return Fus Number
   */
   virtual FusNumber   get_FusNumber() { return _val; }

   void set(FusNumber  &v) { v = _val; }
   virtual string str(Int width) { return _val.to_str(); }
   bool   checkType(const FusNumber &val) { return true; }

   virtual shared_ptr<ValueBase> copy() const { return shared_ptr<ValueFus>(new ValueFus(_val)); }
   shared_ptr<ValueBase> getArrVal(const shared_ptr<ValueBase> & ptr, const shared_ptr<ValueBase> & key);
   /*!
   Установить по ключу ИЛИ индексу другое значение (с проверкой типа)
   \param key ключ значения
   \param val значение
   */
   void                  setArrVal(const shared_ptr<ValueBase> &key, const shared_ptr<ValueBase>& val);
   /*!
   Конвертировать значение в строку
   \return строковое значение
   */
   string toString();
   /*!
   Присвоить переменной Value
   \param val значение
   */
   virtual void operator=(const shared_ptr<ValueBase> &val) { _val = val->get_FusNumber(); };
};

#endif 