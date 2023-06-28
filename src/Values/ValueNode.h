/*!                           Узел графа

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ValueNodeH
#define ValueNodeH

#include "ValueBase.h"
#include "ValueStr.h"

SPTR_DEF(ValueNode);

//=======================================================================================
//! Идентификатор узла
// 
class ValueNode : public ValueBase
{
public:
   UInt                       _classID;      //!< идентификатор класса графов
   UInt                       _val;          //!< уникальный числовой идентификатор узла
   shared_ptr<ValueBase>      _graphValue;   //!< ссылка на граф, если значение получено из Graph=>$node
   ValueNode(UInt val);
   ValueNode(UInt   classID, UInt val);
   ValueNode(shared_ptr<ValueBase> graphValue, UInt val);
   ValueNode(UInt   classID, UInt val, shared_ptr<ValueBase> graphValue);

   ostream& print(ostream& out) { return out << toString(); }
   string   type_str() { return "node"; }
   Kind     type() { return _NODE; }
   //! Операция равенства == 
   Logic eq(ValueBase * v)
   {
      if (v->type() != type())                  // типы не совпадают
         return Logic(1, 0);

      return (_val == ((ValueNode *)v)->_val && _classID == ((ValueNode *)v)->_classID) ? Logic(0, 1) : Logic(1, 0);
   }
   //! Операция неравенства != 
   Logic neq(ValueBase * v)
   {
      return !eq(v);
   }
   /*!
   Операция меньше <
   \param v значение c которым выполняется операция
   \return результат операции
   */
   Logic    lt(ValueBase * v)
   {
      if (v->type() != type())                  // типы не совпадают
         return Logic::Undef;

      if (_classID < ((ValueNode *)v)->_classID)
         return Logic::True;

      if (_classID > ((ValueNode *)v)->_classID)
         return Logic::False;

      return (_val < ((ValueNode *)v)->_val) ? Logic::True : Logic::False;
   }

   void set(UInt  &v) { v = _val; }
   UInt    get_UInt()  { return _val;   }
   Int     get_Int() { TRACE_CRITICAL << "Incorrect type conversion. Use get_UInt() for Node Values"; return (Int)_val; }
   Graph*  get_Graph();
   /*!
   Установить значение узла
   \param значение узла
   */
   void  set_Node(const ValueNode &val);
   /*!
   Получить идентификатор класса графов
   \return идентификатор класса графов
   */
   UInt    get_GraphClassID() { return _classID; }
   /*!
   Получить защищенную ссылку на граф
   \return защищенная ссылка на граф
   */
   shared_ptr<ValueBase>     get_GraphValue();
   /*!
   Получить строковое имя узла
   \return строковое значение
   */
   string name();
   /*!
   Конвертировать значение в строку
   \return строковое значение
   */
   string toString();

   virtual void operator=(const UInt &val) { _val = val; }

   virtual bool    checkType(const UInt &val) { return true; }

   virtual shared_ptr<ValueBase> copy() const { return make_shared<ValueNode>(_classID, _val, _graphValue); }
   /*!
   Получить идентификатор объекта
   \return идентификатор объекта
   */
   virtual shared_ptr<ValueBase> id();
   /*!
   Присвоить переменной Value
   \param val значение
   */
   virtual void operator=(const shared_ptr<ValueBase> &val) 
   {  
      if (val->type() == ValueBase::_NODE) 
         *this = *(ValueNode *)val.get(); 
   };
   /*!
   Операция +
   \param v значение c которым выполняется операция
   \return результат операции
   */
   shared_ptr<ValueBase> plus(ValueBase * v)
   {
      return shared_ptr<ValueBase>(new ValueStr(toString() + v->get_Str()));
   }
   /*!
   Привязать node к текущему графу
   */
   void bind();
   /*!
   Отвязать node от текущего графа
   */
   void unbind();
};

#endif