/*!                           Ребро графа


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ValueEdgeH
#define ValueEdgeH

#include "ValueBase.h"

SPTR_DEF(ValueEdge);

//=======================================================================================
//! Идентификатор ребра
// 
class ValueEdge : public ValueBase
{
   Int _val;
public:
   ValueEdge(Int val) :  _val(val) {}
   ostream& print(ostream& out) { return out << toString(); }
   string   type_str() { return "edge"; }
   Kind     type() { return _EDGE; }
   //! Операция равенства == 
   Logic eq(ValueBase * v)
   {
      if (v->type() != type())                  // типы не совпадают
         return Logic(1, 0);
      return _val == ((ValueEdge *)v)->_val ? Logic(0, 1) : Logic(1, 0);
   }
   //! Операция неравенства != 
   Logic neq(ValueBase * v)
   {
      if (v->type() != type())                  // типы не совпадают
         return Logic(0, 1);
      return _val != ((ValueEdge *)v)->_val ? Logic(0, 1) : Logic(1, 0);
   }
   /*!
   Операция меньше <
   \param v значение c которым выполняется операция
   \return результат операции
   */
   Logic lt(ValueBase * v)
   {
      if (v->type() != type())                  // типы не совпадают
         return Logic::False;
      return _val < ((ValueEdge *)v)->_val ? Logic::True : Logic::False;
   }

   void set(Int  &v) { v = _val; }
   Int  get_Int() { return _val; }
   /*!
   Конвертировать значение в строку
   \return строковое значение
   */
   string toString();

   void operator=(const Int &val) { _val = val; }
   /*!
   Присвоить переменной Value
   \param val значение
   */
   virtual void operator=(const shared_ptr<ValueBase> &val) { _val = val->get_Int(); };

   virtual bool    checkType(const Int &val) { return true; }
   virtual shared_ptr<ValueBase> copy() const { return make_shared<ValueEdge>(_val); }
};

#endif