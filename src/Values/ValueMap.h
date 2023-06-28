/*!                           Массив


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ValueMapH
#define ValueMapH

#include "ValueBase.h"

SPTR_DEF(ValueMap);

struct ValueMapKey
{
   shared_ptr<ValueBase> val;

   ValueMapKey(shared_ptr<ValueBase> &v) { val = v; }
   ValueMapKey(const shared_ptr<ValueBase> &v) { val = v; }

   /*!
   Операция меньше x < y
   \param x левое значение операции
   \param y правое значение операции
   \return результат операции
   */
   friend bool operator <  (const ValueMapKey &x, const ValueMapKey &y);
};

typedef map< ValueMapKey, shared_ptr<ValueBase> > ValueMapType;
//=======================================================================================
//! Массив
// 
class ValueMap : public ValueBase
{
public:
   ValueMapType _val;
   ValueMap() {}
   ValueMap(const ValueMapType & val) : _val(val) {}
   /*!
      Вывести в поток out текущее значение
      \param поток в который выводим значение
      \return результирующий поток
   */
   ostream& print(ostream& out);
   /*!
      Тип значения в строковом виде
      \return тип значения в строковом виде
   */
   string   type_str() { return "map"; }
   /*!
      Тип значения в числовом виде
      \return тип значения в числовом виде
   */
   Kind     type() { return _MAP; }
   /*!
      Тип присваивания значения
      \return присваивания значения
   */
   SetType  setType() const { return ST_REFERENCE; }
   /*!
   Операция равенства ==
   \param v значение c которым выполняется операция
   \return результат операции
   */
   Logic    eq(ValueBase * v);
   /*!
      Установить массив значений в параметр arr
      \param[out] arr параметр в который будет установлено текущий массив значений 
   */  
   void set(ValueMapType & tomap);
   /*!
      Размерность данных
      \return размерность данных
   */
   UInt size() { return (UInt)_val.size(); }
   /*!
      Операция +
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   shared_ptr<ValueBase> plus(ValueBase * v);
   /*!
      Очистить значения (для массива)
   */
   void clear() { _val.clear(); }
   /*!
      Проверить содержится ли переменная val в текущей переменной
      \param val переменная которую ищем
      \return содержится ли переменная val в текущей переменной
   */
   bool is(const shared_ptr<ValueBase> &val);
   /*!
   Найти переменную val в текущей переменной и вернуть ее ключ
   \param val переменная которую ищем
   \return ключ переменной если нашли, иначе пустая строка
   */
   bool hasKey(const shared_ptr<ValueBase> &key);
   /*!
      Найти переменную val в текущей переменной и вернуть ее ключ
      \param val переменная которую ищем
      \return ключ переменной если нашли, иначе пустая строка
   */
   shared_ptr<ValueBase> findKey(const shared_ptr<ValueBase> &val);
   /*!
   Установить по ключу ИЛИ индексу другое значение (с проверкой типа)
   \param key ключ значения
   \param val значение
   */
   virtual void setArrVal(const shared_ptr<ValueBase> &ptr, const shared_ptr<ValueBase>& val);
   /*!
      Получить значение по ключу (для мэпа)
      \param index индекс значения
      \return значение
   */
   shared_ptr<ValueBase> getArrVal(const shared_ptr<ValueBase> & ptr, const shared_ptr<ValueBase> & key);
   /*!
   Получить копию значения
   \return копия значения
   */
   shared_ptr<ValueBase> copy() const { return make_shared<ValueMap>(_val); }
   /*!
   Получить копию значения с копированием всех элементов контейнера
   \return копия значения
   */
   shared_ptr<ValueBase> copyDeep();
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