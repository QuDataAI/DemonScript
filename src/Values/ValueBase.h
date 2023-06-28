/*!                           Полиморфные переменные


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ValueBaseH
#define ValueBaseH

#include "OTypes.h"
#include "Logic.h"
#include "FusNumber.h"
#include "Graph.h"

#include <math.h>
#include <ostream>
#include <string>
#include <vector>
#include <memory>

class ValueNode;
class ValueBase;
SPTR_DEF(FunLambda);
typedef shared_ptr<ValueBase> ValueBasePtr;

//=======================================================================================
//! Прототип всех типов значений
// 
class ValueBase
{
public:   
   /// Перечень типов
   enum  Kind    
   { 
      _NONE,         //!< неопределенный тип
      _FLOAT,        //!< вещественное число 
      _STR,          //!< строка
      _LOGIC,        //!< логическое значение
      _NODE,         //!< номер узла в графе
      _EDGE,         //!< ребро графа
      _ARRAY,        //!< массив
      _GRAPH,        //!< граф
      _FILE,         //!< файл
      _FUSNUMBER,    //!< нечеткое число
      _MAP,          //!< мэп              
      _LAMBDA,       //!< лямбда-выражение
      _SVG,          //!< описание картинки в формате SVG
      _ANY           //!< произвольный тип
   };
   /// Тип присваивания значения. Значения массива и графа присваиваются по ссылке, остальные по значению
   enum  SetType 
   { 
      ST_COPY,       //!< копируем значение и присваем его в новую переменную
      ST_REFERENCE   //!< копируем ссылку на значение и присваиваем в новую переменную
   };
   /*!
      Вывести в поток out текущее значение
      \param поток в который выводим значение
      \return результирующий поток
   */
   virtual ostream& print(ostream& out) = 0;
   /*!
      Тип значения в строковом виде
      \return тип значения в строковом виде
   */
   virtual string   type_str() = 0;
   /*!
      Конвертация целочисленного значения типа в строковое представление
      \param type целочисленное значение типа
      \return тип в строковом виде
   */
   static string   type_str(Kind type);
   /*!
      Тип значения в числовом виде
      \return тип значения в числовом виде
   */
   virtual Kind     type() = 0;
   /*!
      Тип присваивания значения
      \return присваивания значения
   */
   virtual SetType  setType() const { return ST_COPY; }
   /*!
      Операция логического отрицания ! 
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   virtual Logic    not_() { return Logic(); }
   /*!
      Операция равенства ==
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   virtual Logic    eq(ValueBase * v);
   /*!
      Операция неравенства != 
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   virtual Logic    neq(ValueBase * v);
   /*!
      Операция меньше < 
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   virtual Logic    lt(ValueBase * v);
   /*!
      Операция меньше или равно <= 
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   virtual Logic    lteq(ValueBase * v);
   /*!
      Операция больше > 
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   virtual Logic    gt(ValueBase * v);
   /*!
      Операция больше или равно >= 
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   virtual Logic    gteq(ValueBase * v);
   /*!
      Логическое ИЛИ
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   virtual Logic or_(ValueBase * v);
   /*!
      Логическое И
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   virtual Logic and_(ValueBase * v);
   /*!
   Логическая импликация
   \param v значение c которым выполняется операция
   \return результат операции
   */
   virtual Logic impl(ValueBase * v);
   /*!
      Операция +
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   virtual shared_ptr<ValueBase>  plus(ValueBase * v) { return 0; }
   /*!
      Операция -
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   virtual shared_ptr<ValueBase>  minus(ValueBase * v) { return 0; }
   /*!
      Операция *
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   virtual shared_ptr<ValueBase>  mult(ValueBase * v) { return 0; }
   /*!
      Операция вещественного деления /
      \param v значение c которым выполняется операция 
      \return результат операции
   */
   virtual shared_ptr<ValueBase>  div(ValueBase * v) { return 0; }
   /*!
   Операция целочисленного деления //
   \param v значение c которым выполняется операция
   \return результат операции
   */
   virtual shared_ptr<ValueBase>  divInt(ValueBase * v) { return 0; }
   /*!
   Операция %
   \param v значение c которым выполняется операция
   \return результат операции
   */
   virtual shared_ptr<ValueBase>  mod(const shared_ptr<ValueBase> &val) { return 0; }
   /*!
      Установить вещественное значение в параметр v
      \param[out] v параметр в который будет установлено текущее значение 
   */
   virtual void set(Float  &v) { v = 0; }
   /*!
      Установить целочисленное беззнаковое значение в параметр v
      \param[out] v параметр в который будет установлено текущее значение 
   */
   virtual void set(UInt   &v) { v = 0; }
   /*!
      Установить целочисленное знаковое значение в параметр v
      \param[out] v параметр в который будет установлено текущее значение 
   */
   virtual void set(Int   &v) { v = 0; }
   /*!
      Установить строковое значение в параметр v
      \param[out] v параметр в который будет установлено текущее значение 
   */
   virtual void set(string &v) { v = "???"; }
   /*!
      Установить логическое значение в параметр v
      \param[out] v параметр в который будет установлено текущее значение 
   */  
   virtual void set(Logic  &v) { v = Logic(); }
   /*!
      Установить массив значений в параметр v
      \param[out] v параметр в который будет установлено текущий массив значений 
   */  
   virtual void set(vector< shared_ptr<ValueBase> > & arr) { arr.clear(); }
   /*!
      Получить вещественное значение
      \return вещественное значение
   */
   virtual Float   get_Float() { return 0.0f; }
   /*!
      Получить целочисленное беззнаковое значение
      \return целочисленное беззнаковое значение
   */
   virtual UInt    get_UInt() { return 0; }
   /*!
   Получить идентификатор класса графов
   \return идентификатор класса графов
   */
   virtual UInt    get_GraphClassID() { return 0; }
   /*!
      Получить целочисленное знаковое значение
      \return целочисленное знаковое значение
   */
   virtual Int     get_Int() { return 0; }
   /*!
      Получить строковое значение
      \return строковое значение
   */
   virtual string  get_Str() { return ""; }
   /*!
      Получить логическое значение
      \return логическое значение
   */
   virtual Logic   get_Logic() { return Logic(); }
   /*!
   Получить Fus Number
   \return Fus Number
   */
   virtual FusNumber   get_FusNumber() { return FusNumber(0,0,0,0); }
   /*!
      Получить граф
      \return граф
   */
   virtual Graph*  get_Graph() { return 0; }
   /*!
   Получить лямбда функцию
   \return лямбда функция
   */
   virtual FunLambdaPtr  get_Lambda() { return 0; }
   /*!
   Получить защищенную ссылку на граф
   \return защищенная ссылка на граф
   */
   virtual shared_ptr<ValueBase> get_GraphValue() { return 0; }
   /*!
      Установить вещественное значение
      \param вещественное значение
   */
   virtual void  set_Float(const Float &val) {}
   /*!
      Установить значение узла
      \param значение узла
   */
   virtual void  set_Node(const ValueNode &val) {}
   /*!
      Размерность данных
      \return размерность данных
   */
   virtual UInt size() { return 1; }
   /*!
      Проверить содержится ли переменная val в текущей переменной
      \param val переменная которую ищем
      \return содержится ли переменная val в текущей переменной
   */
   virtual bool is(const shared_ptr<ValueBase> &val) { return eq(val.get()).isTrue(); }
   /*!
      Найти переменную val в текущей переменной и вернуть ее индекс
      \param val переменная которую ищем
      \return индекс переменной если нашли, иначе -1
   */
   virtual int find(const shared_ptr<ValueBase> &val) { return -1; }
   /*!
      Добавить значение в конец (массива)
      \param val добавляемое значение
   */
   virtual void push(const shared_ptr<ValueBase> &val) {}
   /*!
      Добавить значение в начало (массива)
      \param val добавляемое значение
   */
   virtual void unshift(const shared_ptr<ValueBase> &val) {}
   /*!
      Получить значение последнего элемента и удалить его (из массива)
      \return последний элемент (в случае отсутствия вернет 'None')
   */
   virtual shared_ptr<ValueBase> pop() { return 0; }
   /*!
      Получить значение первого элемента и удалить его (из массива)
      \return первый элемент (в случае отсутствия вернет 'None')
   */
   virtual shared_ptr<ValueBase> shift() { return 0; }
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
   virtual shared_ptr<ValueBase> splice(Int start, Int deleteCount) { return 0; }
   /*!
      Получить значение поля объекта, хранящегося в переменной
      \param name имя поля в переменной
      \return значение поля
   */
   virtual shared_ptr<ValueBase> field(shared_ptr<ValueBase> ptr, ValueBase *name) { return 0; }
   /*!
      Конвертировать значение в строку
      \return строковое значение
   */
   virtual string toString() { return ""; }
   /*!
      Конвертировать значение в число
      \return числовое значение если конвертация возможна, иначе None
   */
   virtual shared_ptr<ValueBase> toFloat() { return 0; }
   /*!
      Очистить значения (для массива)
   */
   virtual void clear() {}
   /*!
      Случайно перемшать данные (для массива)
   */
   virtual void mix() {}
   /*!
      Проверить истинность значения
   */
   virtual bool is_true() { return false; }
   /*!
      Присвоить вещественное значение
      \param val вещественное значение
   */
   virtual void operator=(const Float &val) {}
   /*!
      Присвоить целочисленное беззнаковое значение
      \param val целочисленное беззнаковое значение
   */
   virtual void operator=(const UInt &val) {}
   /*!
      Присвоить целочисленное знаковое значение
      \param val целочисленное знаковое значение
   */
   virtual void operator=(const Int &val) {}
   /*!
      Присвоить строковое значение
      \param val строковое значение
   */
   virtual void operator=(const string &val) {}
   /*!
      Присвоить строковое значение
      \param val строковое значение
   */
   virtual void operator=(const char* &val) {}
   /*!
      Присвоить логическое значение
      \param val логическое значение
   */
   virtual void operator=(const Logic &val) {}
   /*!
      Присвоить переменной граф
      \param val переменной граф
   */
   virtual void operator=(const Graph &val) {}
   /*!
   Присвоить переменной Value
   \param val значение
   */
   virtual void operator=(const shared_ptr<ValueBase> &val) = 0;
   /*!
      Инкрементировать значение переменной
   */
   virtual void operator ++ (int) {}
   /*!
      Декриментировать значение переменной
   */
   virtual void operator -- (int) {}
   /*!
   Установить по ключу ИЛИ индексу другое значение (с проверкой типа)
   \param key ключ значения
   \param val значение
   */
   virtual void setArrVal(const shared_ptr<ValueBase> &key, const shared_ptr<ValueBase>& val) { }
   /*!
   Получить значение по ключу (для мэпа)
   \param index индекс значения
   \return значение
   */
   virtual shared_ptr<ValueBase> getArrVal(const shared_ptr<ValueBase> & ptr, const shared_ptr<ValueBase> & key) { return 0; }
   /*!
      Получить копию значения
      \return копия значения
   */
   virtual shared_ptr<ValueBase> copy() const = 0;
   /*!
   Получить копию значения с копированием всех элементов контейнера
   \return копия значения
   */
   virtual shared_ptr<ValueBase> copyDeep() { return copy(); }
   /*!
      Проверить соответсвие типа значения вещественному типу
      \return результат проверки
   */
   virtual bool    checkType(const Float &val)                 { return false; }
   /*!
      Проверить соответсвие типа значения целому беззнаковому типу
      \return результат проверки
   */
   virtual bool    checkType(const UInt &val)                  { return false; }
   /*!
      Проверить соответсвие типа значения целому знаковому типу
      \return результат проверки
   */
   virtual bool    checkType(const Int &val)                   { return false; }
   /*!
      Проверить соответсвие типа значения строковому типу
      \return результат проверки
   */
   virtual bool    checkType(const string &val)                { return false; }
   /*!
      Проверить соответсвие типа значения строковому типу
      \return результат проверки
   */
   virtual bool    checkType(const char* val)                  { return false; }
   /*!
      Проверить соответсвие типа значения логическому типу
      \return результат проверки
   */
   virtual bool    checkType(const Logic &val)                 { return false; }
   /*!
      Проверить соответсвие типа значения графовому типу
      \return результат проверки
   */
   virtual bool    checkType(const Graph &val)                 { return false; }
   /*!
      Проверить соответсвие типа значения узловому типу
      \return результат проверки
   */
   virtual bool    checkType(const Node &val)                  { return false; }
   /*!
   Проверить соответсвие типа значения узловому типу
   \return результат проверки
   */
   virtual bool    checkType(const FusNumber &val) { return false; }
   /*!
      Проверить соответсвие типа значения указанному типу
      \return результат проверки
   */
   virtual bool    checkType(const shared_ptr<ValueBase> &val) { return false; }
   /*!
      Получить текстовое представление величины
      \param width 
      \return текстовое представление величины
   */
   virtual string str(Int width) { return "unknown"; }
   /*!
      Получить идентификатор объекта
      \return идентификатор объекта
   */
   virtual shared_ptr<ValueBase> id() { return 0; }

};

/* 
   Макрос выполняет проверку равенства типа текущего значения со значением "v"
   и в случае несовпадения выдает ошибку и возвращает "res"
*/
#define CHECK_TYPES(v, res) if (v->type() != type()) {TRACE_WARNING << "Attempt to compare values with different types" << endl; return res;}

#endif 