/*!                           Массив


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ValueArrH
#define ValueArrH

#include "ValueBase.h"

SPTR_DEF(ValueArr);

//=======================================================================================
//! Массив
// 
class ValueArr : public ValueBase
{
public:
   vector< shared_ptr<ValueBase> > _val;
   ValueArr() {}
   ValueArr(const vector< shared_ptr<ValueBase> > & val) : _val(val) {}
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
   string   type_str() { return "array"; }
   /*!
      Тип значения в числовом виде
      \return тип значения в числовом виде
   */
   Kind     type() { return _ARRAY; }
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
   Операция неравенства !=
   \param v значение c которым выполняется операция
   \return результат операции
   */
   Logic    neq(ValueBase * v);
   /*!
      Установить массив значений в параметр arr
      \param[out] arr параметр в который будет установлено текущий массив значений 
   */  
   void set(vector< shared_ptr<ValueBase> > & arr);
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
      Случайно перемшать данные (для массива)
   */
   void mix();
   /*!
      Проверить содержится ли переменная val в текущей переменной
      \param val переменная которую ищем
      \return содержится ли переменная val в текущей переменной
   */
   bool is(const shared_ptr<ValueBase> &val);
   /*!
      Найти переменную val в текущей переменной и вернуть ее индекс
      \param val переменная которую ищем
      \return индекс переменной если нашли, иначе -1
   */
   int find(const shared_ptr<ValueBase> &val);
   /*!
      Добавить значение в конец (массива)
      \param val добавляемое значение
   */
   void push(const shared_ptr<ValueBase> &val);
   /*!
   Добавить пустое значение в конец (массива)
   */
   void push();
   /*!
      Добавить значение в начало (массива)
      \param val добавляемое значение
   */
   void unshift(const shared_ptr<ValueBase> &val);
   /*!
      Получить значение последнего элемента и удалить его (из массива)
      \return последний элемент (в случае отсутствия вернет 'None')
   */
   shared_ptr<ValueBase> pop();
   /*!
      Получить значение первого элемента и удалить его (из массива)
      \return первый элемент (в случае отсутствия вернет 'None')
   */
   shared_ptr<ValueBase> shift();
   /*!
      Удалить deleteCount существующих элементов начиная с позиции start.
      Если start больше длины массива, реальный индекс будет установлен на длину массива.
      Если start отрицателен, то start будет указывать на индекс элемента с конца.
      Если deleteCount равен 0, элементы не удаляются.Если deleteCount больше количества элементов,
      оставшихся в массиве, начиная с индекса start, то будут удалены все элементы до конца массива.
      \param start начальная позиция удаляемых элементов
      \param deleteCount число удаляемых элементов
      \return массив удаленных элементов
   */
   shared_ptr<ValueBase> splice(Int start, Int deleteCount);
   /*!
   Установить по ключу ИЛИ индексу другое значение (с проверкой типа)
   \param key ключ значения
   \param val значение
   */
   virtual void setArrVal(const shared_ptr<ValueBase> &key, const shared_ptr<ValueBase>& val);
   /*!
   Получить значение по ключу (для мэпа)
   \param index индекс значения
   \return значение
   */
   virtual shared_ptr<ValueBase> getArrVal(const shared_ptr<ValueBase> & ptr, const shared_ptr<ValueBase> & key);
   /*!
   Получить копию значения
   \return копия значения
   */
   shared_ptr<ValueBase> copy() const { return make_shared<ValueArr>(_val); }
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
   Получить Fus Number
   \return Fus Number
   */
   virtual FusNumber   get_FusNumber();
   /*!
   Присвоить переменной Value
   \param val значение
   */
   virtual void operator=(const shared_ptr<ValueBase> &val) {  };
   /*!
   Отсортировать массив
   */
   void sort(FunLambdaPtr compareFn = 0);
private:
   //!< кастомная функция сравнения
   class ValueArrDemonComparator {
   public:
      ValueArrDemonComparator(FunLambdaPtr compareFn) : _compareFn(compareFn) {};
      bool operator()(const shared_ptr<ValueBase> &a, const shared_ptr<ValueBase> &b);
      FunLambdaPtr _compareFn;
   };
};

#endif