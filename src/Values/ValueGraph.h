/*!                           Граф


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ValueGraphH
#define ValueGraphH

#include "ValueBase.h"

SPTR_DEF(ValueGraph);
SPTR_WDEF(ValueGraph);

//=======================================================================================
//! Граф
// 
class ValueGraph : public ValueBase
{
public:
   Graph _val;
   ValueGraph() {}
   ValueGraph(const Graph & val) : _val(val) {}

   ostream& print(ostream& out) { return _val.print(out, 0); }
   string   type_str() { return "graph"; }
   Kind     type() { return _GRAPH; }
   SetType  setType() const { return ST_REFERENCE; }

   void set(Graph & g) { g = _val; }
   UInt size() { return (UInt)_val._nodes.size() - 1; }

   //! Очистить значения 
   void clear() { _val.clear_edges(); }

   static shared_ptr<ValueGraph> create();

   Graph* get_Graph() { return &_val; }
   /*!
   Получить идентификатор класса графов
   \return идентификатор класса графов
   */
   UInt    get_GraphClassID() { return _val.classID(); }

   void   set_Graph(Graph* graph) { _val = *graph; }

   bool    checkType(const Graph &val) { return true; }
   /*!
   Операция равенства ==
   \param v значение c которым выполняется операция
   \return результат операции
   */
   Logic    eq(ValueBase * v);
   /*!
   Операция неравенства !=
   \param v значение c которым выполняется операция
   \return результат операции
   */
   Logic    neq(ValueBase * v);
   /*!
   Операция +
   \param v значение c которым выполняется операция
   \return результат операции
   */
   shared_ptr<ValueBase>  plus(ValueBase * v);
   /*!
   Операция -
   \param v значение c которым выполняется операция
   \return результат операции
   */
   shared_ptr<ValueBase>  minus(ValueBase * v);
   /*!
   Получить значение поля объекта, хранящегося в переменной
   \param name имя поля
   \return значение поля
   */
   shared_ptr<ValueBase> field(shared_ptr<ValueBase> ptr, ValueBase *name);
   /*!
   Получить копию значения
   \return копия значения
   */
   shared_ptr<ValueBase> copy() const;
   /*!
   Получить значение по индексу (для массива)
   \param ptr указатель на текущий объект, так как может быть передан дальше
   \param index индекс значения
   \return значение
   */
   shared_ptr<ValueBase> getArrVal(const shared_ptr<ValueBase> & ptr, const shared_ptr<ValueBase> & key);
   /*!
   Конвертировать значение в строку
   \return строковое значение
   */
   string toString();
   /*!
   Присвоить переменной Value
   \param val значение
   */
   virtual void operator=(const shared_ptr<ValueBase> &val) {  };
};

#endif