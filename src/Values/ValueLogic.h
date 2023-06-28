/*!                           Логическая переменная


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ValueLogicH
#define ValueLogicH

#include "ValueBase.h"
#include "ValueFloat.h"
#include "ValueNone.h"
#include "ValueIndexRef.h"

SPTR_DEF(ValueLogic);

//=======================================================================================
//! Логический тип
// 
class ValueLogic : public ValueBase
{
public:
   Logic _val;

   ValueLogic(const Logic & val) : _val(val) {}
   ostream& print(ostream& out) { return out << _val; }
   string   type_str() { return "logic"; }
   Kind     type() { return _LOGIC; }
   //! Операция логического отрицания ! 
   Logic not_();
   //! Операция равенства == 
   Logic eq(ValueBase * v);
   //! Операция неравенства != 
   Logic neq(ValueBase * v);
   //! Операция меньше < 
   Logic lt(ValueBase * v);
   //! Операция меньше или равно <= 
   Logic lteq(ValueBase * v);
   //! Операция больше > 
   Logic gt(ValueBase * v);
   //! Операция больше или равно >= 
   Logic gteq(ValueBase * v);
   //! Логическое ИЛИ
   virtual Logic or_(ValueBase * v);
   //! Логическое И
   virtual Logic and_(ValueBase * v);
   /*!
      Логическая импликация
      \param v значение c которым выполняется операция
      \return результат операции
   */
   Logic impl(ValueBase * v);
   void set(Logic  &v) { v = _val; }
   UInt size() { return 2; }
   bool is_true() { return _val == Logic(0, 1); }
   virtual string str(Int width) { return _val.to_str(); }
   Logic  get_Logic() { Logic  v;   set(v);   return v; }
   void   set_Logic(const Logic &v) { _val = v; }
   bool   checkType(const Logic &val) { return true; }

   virtual shared_ptr<ValueBase> copy() const { return make_shared<ValueLogic>(_val); }
   shared_ptr<ValueBase> getArrVal(const shared_ptr<ValueBase> & ptr, const shared_ptr<ValueBase> & key);
   void                  setArrVal(shared_ptr<ValueBase>& key, const shared_ptr<ValueBase>& val);
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
   Присвоить переменной Value
   \param val значение
   */
   virtual void operator=(const shared_ptr<ValueBase> &val) { _val = val->get_Logic(); };
};

#endif 