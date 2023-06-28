/*!                           Контейнер универсального типа


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ValueH
#define ValueH

#include "ValueManager.h"
#include "ValueIndexRef.h"
#include "ValueRef.h"

SPTR_DEF(ValueBase);

//=======================================================================================
//! Универсальный тип
// 
class Value
{
   ValueBasePtr _ptr;
public:
   Value() {                           _ptr = ValueManager::createValue();    }
   Value(const Logic & val) {          _ptr = ValueManager::createValue(val); }
   Value(UInt   &val) {                _ptr = ValueManager::createValue(val); }
   Value(Int   &val) {                 _ptr = ValueManager::createValue(val); }
   Value(const Float &val) {           _ptr = ValueManager::createValue(val); }
   Value(const string & val) {         _ptr = ValueManager::createValue(val); }
   Value(const char *   val) {         _ptr = ValueManager::createValue(val); }
   Value(const Graph & val) {          _ptr = ValueManager::createValue(val); }
   Value(const ValueNode & val){       _ptr = ValueManager::createValue(val); }
   Value(const ValueArr & val) {       _ptr = ValueManager::createValue(val); }
   Value(const ValueFile & val)      { _ptr = ValueManager::createValue(val); }
   Value(const ValueSVG & val)       { _ptr = ValueManager::createValue(val); }
   Value(const FusNumber & val)      { _ptr = ValueManager::createValue(val); }   
   Value(const ValueIndexRef & val)  { ValueManager::copyValueToValue(_ptr, val.value()); }
   Value(const ValueRef & val);
   Value(vector<Value> & arr);
   Value(const shared_ptr<ValueBase> &val);
   ~Value();
   /*!
      Вывести в поток out текущее значение
      \param поток в который выводим значение
      \return результирующий поток
   */
   ostream& print(ostream& out) { return _ptr->print(out); }
   /*!
      Тип значения в строковом виде
      \return тип значения в строковом виде
   */
   string   type_str() { return _ptr->type_str(); }
   /*!
      Тип значения в числовом виде
      \return тип значения в числовом виде
   */
   ValueBase::Kind type() const { return _ptr->type(); }
   /*!
      Получить копию значения
      \return копия значения
   */
   Value copy() { return Value(_ptr->copy());   }
   /*!
   Получить копию значения с копированием всех элементов контейнера
   \return копия значения
   */
   Value copyDeep() { return Value(_ptr->copyDeep()); }
   /*!
      Получить указатель на значение
      \return указатель на значение
   */
   shared_ptr<ValueBase> ptr() const { return _ptr; }
   /*!
   Получить ссылку на значение
   \return ссылка на значение
   */
   ValueRef ref() const { return ValueRef(*this); }
   /*!
      Получить ссылку на значение по индексу из другого значения (ValueFloat)
      \return cсылка на значение
   */
   ValueIndexRef operator [](const Value &i);  
   /*!
   Получить ссылку на значение по индексу из другого значения (ValueFloat)
   \return cсылка на значение
   */
   ValueIndexRef operator [](const UInt &i);
   /*!
   Получить ссылку на значение по индексу из другого значения (ValueFloat)
   \return cсылка на значение
   */
   ValueIndexRef operator [](const Int &i);
   /*!
      Присвоить вещественное значение
      \param val вещественное значение
   */
   Value & operator = (Float val);
   /*!
      Присвоить целочисленное беззнаковое значение
      \param val целочисленное беззнаковое значение
   */
   Value & operator = (UInt  val);
   /*!
      Присвоить целочисленное знаковое значение
      \param val целочисленное знаковое значение
   */
   Value & operator = (Int   val);
   /*!
      Присвоить строковое значение
      \param val строковое значение
   */
   Value & operator = (const string & val);
   /*!
      Присвоить строковое значение
      \param val строковое значение
   */
   Value & operator = (const char* & val);               
   /*!
      Присвоить логическое значение
      \param val логическое значение
   */
   Value & operator = (const Logic & val);       
   /*!
      Присвоить значение
      \param val значение
   */
   Value & operator = (const Value & val);              
   /*!
      Присвоить значение из ссылки
      \param val значение из ссылки
   */
   Value & operator = (const ValueRef & val);
   /*!
      Присвоить значение
      \param val значение
   */
   Value & operator = (shared_ptr<ValueBase> & val);   
   /*!
      Операция логического отрицания !x 
      \param x значение c которым выполняется операция 
      \return результат операции
   */
   friend Logic  operator !  (const Value &x);                 
   /*!
      Операция эквивалентности x == y
      \param x левое значение операции 
      \param y правое значение операции
      \return результат операции
   */
   friend Logic  operator == (const Value &x, const Value &y); 
   /*!
      Операция неэквивалентности x != y
      \param x левое значение операции
      \param y правое значение операции
      \return результат операции
   */
   friend Logic  operator != (const Value &x, const Value &y); 
   /*!
      Операция меньше x < y
      \param x левое значение операции
      \param y правое значение операции
      \return результат операции
   */
   friend Logic  operator <  (const Value &x, const Value &y); 
   /*!
      Операция меньше или равно x <= y
      \param x левое значение операции
      \param y правое значение операции
      \return результат операции
   */
   friend Logic  operator <= (const Value &x, const Value &y); 
   /*!
      Операция больше x > y 
      \param x левое значение операции
      \param y правое значение операции
      \return результат операции
   */
   friend Logic  operator >  (const Value &x, const Value &y); 
   /*!
      Операция больше или равно x >= y
      \param x левое значение операции
      \param y правое значение операции
      \return результат операции
   */   
   friend Logic  operator >= (const Value &x, const Value &y); 
   /*!
      Операция логического ИЛИ x | y
      \param x левое значение операции
      \param y правое значение операции
      \return результат операции
   */   
   friend Logic  operator |  (const Value &x, const Value &y);
   /*!
      Операция логического И x & y
      \param x левое значение операции
      \param y правое значение операции
      \return результат операции
   */   
   friend Logic  operator &  (const Value &x, const Value &y); 
   /*!
      Операция логической импликации
      \param x левое значение операции
      \return результат операции
   */
   Logic  impl(const Value &y);
   /*!
      Операция целочисленного деления //
      \param v значение c которым выполняется операция
      \return результат операции
   */
   Value  divInt(const Value &y);
   /*!
      Операция сложения +
      \param x добавляемое значение
      \return результат операции
   */  
   Value operator + (const Value &x);
   /*!
      Операция отнимания -
      \param x отнимаемое значение
      \return результат операции
   */  
   Value operator - (const Value &x);   
   /*!
      Операция умножения *
      \param x умножаемое значение
      \return результат операции
   */
   Value operator * (const Value &x);
   /*!
      Операция деления /
      \param x делимое значение
      \return результат операции
   */
   Value operator / (const Value &x);      
   /*!
   Операция %
   \param v значение c которым выполняется операция
   \return результат операции
   */
   Value operator % (const Value &x) { return _ptr ? _ptr->mod(x.ptr()) : Value(); }
   /*!
      Операция инкрементирования ++. Значение увеличивается, но возвращается его копия до изменения.
      \return результат перед операцией 
   */
   Value operator ++ (int);                            
   /*!
      Операция декриментирования --. Значение уменьшается, но возвращается его копия до изменения.
      \return результат перед операцией 
   */
   Value operator -- (int);   
   /*!
      Установить массив значений в параметр arr
      \param[out] v параметр в который будет установлено текущий массив значений
   */
   void set(vector<Value>  &arr);
   /*!
      Проверить истинность значения
   */
   bool is_true() { return _ptr ? _ptr->is_true() : false; }
   /*!
      Получить текстовое представление величины
      \param width
      \return текстовое представление величины
   */
   string str(Int width) { return _ptr ? _ptr->str(width) : "unknown"; }
   /*!
      Размерность данных
      \return размерность данных
   */
   UInt size();
   /*!
      Проверить содержится ли переменная val в текущей переменной
      \param val переменная которую ищем
      \return содержится ли переменная val в текущей переменной
   */
   Logic is(const Value &val);
   /*!
      Найти переменную val в текущей переменной и вернуть ее индекс
      \param val переменная которую ищем
      \return индекс переменной если нашли, иначе -1
   */
   int find(const Value &val);
   /*!
      Добавить значение в конец (массива)
      \param val добавляемое значение
   */
   void push(const Value &val);
   /*!
      Добавить значение в начало (массива)
      \param val добавляемое значение
   */
   void unshift(const Value &val);
   /*!
      Получить значение последнего элемента и удалить его (из массива)
      \return последний элемент (в случае отсутствия вернет 'None')
   */
   Value pop();
   /*!
      Получить значение первого элемента и удалить его (из массива)
      \return первый элемент (в случае отсутствия вернет 'None')
   */
   Value shift();
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
   Value splice(Int start, Int deleteCount);
   /*!
      Получить значение поля объекта, хранящегося в переменной
      \param name имя поля в переменной
      \return значение поля
   */
   Value field(Value &name);
   /*!
      Конвертировать значение в строку
      \return строковое значение
   */
   string toString();
   /*!
      Конвертировать значение в число
      \return числовое значение если конвертация возможна, иначе None
   */
   Value toFloat();
   /*!
      Очистить значения (для массива)
   */
   void clear() { if (_ptr) _ptr->clear(); }
   /*!
      Случайно перемшать данные (для массива)
   */
   void mix() { if (_ptr) _ptr->mix(); }
   /*!
      Получить число ссылок на значение
   */
   Int  count() { return _ptr ? 0 : _ptr.use_count(); }
   /*!
      Вывод значения в поток
      \param out исходный поток
      \param out выводимое значение
      \return результирующий поток
   */
   friend std::ostream& operator<< (std::ostream& out, Value & val);
   /*!
      Получить вещественное значение
      \return вещественное значение
   */
   Float   get_Float() { return _ptr->get_Float(); }
   /*!
      Получить целочисленное беззнаковое значение
      \return целочисленное беззнаковое значение
   */
   UInt    get_UInt()  { return _ptr->get_UInt(); }
   /*!
      Получить целочисленное знаковое значение
      \return целочисленное знаковое значение
   */
   Int     get_Int()   { return _ptr->get_Int(); }
   /*!
      Получить строковое значение
      \return строковое значение
   */
   string  get_Str()   { return _ptr->get_Str(); }
   /*!
      Получить логическое значение
      \return логическое значение
   */
   Logic   get_Logic() { return _ptr->get_Logic(); }
   /*!
      Получить граф
      \return граф
   */
   Graph*  get_Graph() { return _ptr->get_Graph(); }
   /*!
      Получить лямбда функцию
      \return лямбда функция
   */
   FunLambdaPtr  get_Lambda() { return _ptr->get_Lambda(); }
   /*!
   Получить значение графа
   \return значение графа
   */
   Value get_GraphValue();
   /*!
      Получить идентификатор класса графов
      \return идентификатор класса графов
   */
   UInt    get_GraphClassID() { return _ptr->get_GraphClassID(); }
   /*!
   Получить Fus Number
   \return Fus Number
   */
   FusNumber   get_FusNumber() { return _ptr->get_FusNumber(); }
   /*!
   Установить вещественное значение
   \param вещественное значение
   */
   void   set_Float(const Float &val) { return _ptr->set_Float(val); }
   /*!
   Установить значение узла
   \param значение узла
   */
   void   set_Node(const ValueNode &val) { return _ptr->set_Node(val); }
   /*!
      Получить идентификатор объекта
      \return идентификатор объекта
   */
   Value    id() { return _ptr->id(); }
   /*!
      Получить ссылку на значение массива
      \return ссылка на значение массива
   */
   ValueArr* valueArr();
   /*!
   Получить ссылку на значение ребра
   \return ссылка на значение ребра
   */
   ValueEdge* valueEdge();
   /*!
   Получить ссылку на значение файла
   \return ссылка на значение файла
   */
   ValueFile* valueFile();
   /*!
   Получить ссылку на значение вещественного числа
   \return ссылка на значение вещественного числа
   */
   ValueFloat* valueFloat();
   /*!
   Получить ссылку на значение нечеткого числа
   \return ссылка на значение нечеткого числа
   */
   ValueFus* valueFus();
   /*!
   Получить ссылку на значение графа
   \return ссылка на значение графа
   */
   ValueGraph* valueGraph();
   /*!
   Получить ссылку на логическое значение 
   \return ссылка на логическое значение
   */
   ValueLogic* valueLogic();
   /*!
   Получить ссылку на значение мапы
   \return ссылка на значение мапы
   */
   ValueMap* valueMap();
   /*!
   Получить ссылку на значение узла
   \return ссылка на значение узла
   */
   ValueNode* valueNode();
   /*!
   Получить ссылку на строковое значение
   \return ссылка на строковое значение 
   */
   ValueStr* valueStr();
   /*!
   Получить ссылку на значение SVG
   \return ссылка на значение SVG
   */
   ValueSVG* valueSVG();
};

#endif