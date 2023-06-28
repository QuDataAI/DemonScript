/*!                           Тип неопределен


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ValueNoneH
#define ValueNoneH

#include "ValueBase.h"

SPTR_DEF(ValueNone);

//=======================================================================================
//! Неопределенный тип
// 
class ValueNone : public ValueBase
{
public:
   ostream& print(ostream& out) { return out << "None"; }
   string   type_str() { return "None"; }
   Kind     type() { return _NONE; }
   //! Операция равенства == 
   Logic eq(ValueBase * v)
   {
      return (v == this) ? Logic::True : Logic::Undef;
   }
   //! Операция неравенства != 
   Logic neq(ValueBase * v)
   {
      return (v == this) ? Logic::False : Logic::Undef;
   }
   //! Операция меньше < 
   Logic lt(ValueBase * v)
   {
      return Logic::Undef;
   }
   //! Операция меньше или равно <= 
   Logic lteq(ValueBase * v)
   {
      return Logic::Undef;
   }
   //! Операция больше > 
   Logic gt(ValueBase * v)
   {
      return Logic::Undef;
   }
   //! Операция больше или равно >= 
   Logic gteq(ValueBase * v)
   {
      return Logic::Undef;
   }
   //! Операция + 
   shared_ptr<ValueBase> plus(ValueBase * v)
   {
      return ptr();
   }
   //! Операция - 
   virtual shared_ptr<ValueBase>  minus(ValueBase * v)
   {
      return ptr();
   }
   //! Операция *
   virtual shared_ptr<ValueBase>  mult(ValueBase * v)
   {
      return ptr();
   }
   //! Операция /
   virtual shared_ptr<ValueBase>  div(ValueBase * v)
   {
      return ptr();
   }
   /*!
   Конвертировать значение в строку
   \return строковое значение
   */
   string toString() { return "None"; }
   virtual shared_ptr<ValueBase> copy() const { return ptr(); }
   static shared_ptr<ValueBase>  ptr();   //ссылка на пустую переменную
   /*!
   Присвоить переменной Value
   \param val значение
   */
   virtual void operator=(const shared_ptr<ValueBase> &val) {  };
private:
   static shared_ptr<ValueBase> _ptr;     //для оптимизации пустую переменную создадим один раз и будем использовать везде по ссылке
};

#endif 