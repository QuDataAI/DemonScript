/*!                           Строка


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ValueStrH
#define ValueStrH

#include "ValueBase.h"

SPTR_DEF(ValueStr);

//=======================================================================================
//! Строковый тип
// 
class ValueStr : public ValueBase
{
   string _val;
public:
   ValueStr(const string & val) : _val(val) {}
   ostream& print(ostream& out) { return out << _val; }
   string   type_str() { return "string"; }
   Kind     type() { return _STR; }
   //! Операция равенства == 
   Logic eq(ValueBase * v)
   {
      if (v->type() != type())                  // типы не совпадают
         return Logic(1, 0);
      return _val == ((ValueStr *)v)->_val ? Logic(0, 1) : Logic(1, 0);
   }
   //! Операция неравенства != 
   Logic neq(ValueBase * v)
   {
      if (v->type() != type())                  // типы не совпадают
         return Logic(0, 1);
      return _val != ((ValueStr *)v)->_val ? Logic(0, 1) : Logic(1, 0);
   }
   //! Операция меньше < 
   Logic lt(ValueBase * v)
   {
      if (v->type() != type())                  // типы не совпадают
         return Logic(0, 0);                    // значение неопределено
      return _val < ((ValueStr *)v)->_val ? Logic(0, 1) : Logic(1, 0);
   }
   //! Операция меньше или равно <= 
   Logic lteq(ValueBase * v)
   {
      if (v->type() != type())                  // типы не совпадают
         return Logic(0, 0);                    // значение неопределено
      return _val <= ((ValueStr *)v)->_val ? Logic(0, 1) : Logic(1, 0);
   }
   //! Операция больше > 
   Logic gt(ValueBase * v)
   {
      if (v->type() != type())                  // типы не совпадают
         return Logic(0, 0);                    // значение неопределено
      return _val > ((ValueStr *)v)->_val ? Logic(0, 1) : Logic(1, 0);
   }
   //! Операция больше или равно >= 
   Logic gteq(ValueBase * v)
   {
      if (v->type() != type())                  // типы не совпадают
         return Logic(0, 0);                    // значение неопределено
      return _val >= ((ValueStr *)v)->_val ? Logic(0, 1) : Logic(1, 0);
   }
   //! Операция + 
   shared_ptr<ValueBase> plus(ValueBase * v)
   {
      if (v->type() != ValueBase::_STR)
         return make_shared<ValueStr>(_val + v->toString());
      return  make_shared<ValueStr>(_val + ((ValueStr *)v)->_val);
   }

   UInt size() { return (UInt)_val.size(); }
   void set(string &v) { v = _val; }
   string get_Str() { string v;    set(v);  return v; }
   void operator=(const string &val) { _val = val; }
   /*!
   Присвоить переменной Value
   \param val значение
   */
   virtual void operator=(const shared_ptr<ValueBase> &val) { _val = val->get_Str(); };

   virtual shared_ptr<ValueBase> copy() const { return make_shared<ValueStr>(_val); }
   virtual bool    checkType(const string &val) { return true; }
   virtual bool    checkType(const char* val) { return true; }

   shared_ptr<ValueBase> getArrVal(const shared_ptr<ValueBase> & ptr, const shared_ptr<ValueBase> & key);
   void                  setArrVal(const shared_ptr<ValueBase>& key, const shared_ptr<ValueBase>& val);

   /*!
   Конвертировать значение в строку
   \return строковое значение
   */
   string toString() { return _val; }
   /*!
   Конвертировать значение в число
   \return числовое значение если конвертация возможна, иначе None
   */
   shared_ptr<ValueBase> toFloat();
};

#endif