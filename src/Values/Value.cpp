#include "Value.h"
#include "ValueRef.h"

Value & Value::operator=(const Logic & val)
{ 
   ValueManager::changeValue(_ptr, val);
   return *this;
}
Value & Value::operator=(UInt  val)
{ 
   //Float floatVal = val;
   ValueManager::changeValue(_ptr, val);
   return *this;
}
Value & Value::operator =(Int   val)
{ 
   //Float floatVal = val;
   ValueManager::changeValue(_ptr, val);
   return *this;
}
Value & Value::operator =(Float val)
{ 
   ValueManager::changeValue(_ptr, val);
   return *this;
}

Value & Value::operator =(const string & val)
{ 
   ValueManager::changeValue(_ptr, val);
   return *this;
}

Value & Value::operator=(const char *& val)
{
   ValueManager::changeValue(_ptr, val);
   return *this;
}

Value & Value::operator =(const Value & val)
{
   ValueManager::copyValueToValue(_ptr, val.ptr());
   return *this;
}

Value & Value::operator=(const ValueRef & val)
{
   _ptr = val.ptr();
   return *this;
}

Value & Value::operator=(shared_ptr<ValueBase>& val)
{
   ValueManager::copyValueToValue(_ptr, val);
   return *this;
}

//! Логическое НЕ: !x
Logic  operator !  (const Value &x)
{
   return x._ptr->not_();
}
//! Эквивалентность x == y
Logic operator ==  (const Value &x, const Value &y)
{
   return x._ptr->eq(y._ptr.get());
}
//! Неэквивалентность x != y
Logic operator !=  (const Value &x, const Value &y)
{
   return x._ptr->neq(y._ptr.get());
}
//! Меньше x < y
Logic operator <  (const Value &x, const Value &y)
{
   return x._ptr->lt(y._ptr.get());
}
//! Меньше или равно x <= y
Logic operator <=  (const Value &x, const Value &y)
{
   return x._ptr->lteq(y._ptr.get());
}
//! Больше x > y
Logic operator >  (const Value &x, const Value &y)
{
   return x._ptr->gt(y._ptr.get());
}
//! Больше или равно x >= y
Logic operator >=  (const Value &x, const Value &y)
{
   return x._ptr->gteq(y._ptr.get());
}
//! 
Logic operator |  (const Value &x, const Value &y)
{
   return x._ptr-> or_ (y._ptr.get());
}
//! 
Logic operator &  (const Value &x, const Value &y)
{
   return x._ptr-> and_ (y._ptr.get());
}
Logic Value::impl(const Value & y)
{
   return _ptr->impl(y._ptr.get());
}
Value Value::divInt(const Value & y)
{
   return _ptr->divInt(y._ptr.get());
}
//! +
Value Value::operator + (const Value &x)
{
   return Value(_ptr->plus(x._ptr.get()));
}
//! -
Value  Value::operator - (const Value &x)
{
   return Value(_ptr->minus(x._ptr.get()));
}
//! *
Value  Value::operator * (const Value &x)
{
   return Value(_ptr->mult(x._ptr.get()));
}
//! /
Value  Value::operator / (const Value &x)
{
   return Value(_ptr->div(x._ptr.get()));
}

Value Value::operator++(int)
{
   Value retVal = copy();
   (*_ptr)++;
   return retVal;
}

Value Value::operator--(int)
{
   Value retVal = copy();
   (*_ptr)--;
   return retVal;
}

void Value::set(vector<Value>& arr)
{
   if (!_ptr.operator bool())
      return;
   vector< shared_ptr<ValueBase> > ptrs;
   _ptr->set(ptrs);
   arr.clear();
   for (UInt i = 0; i < ptrs.size(); i++)
      arr.push_back(ptrs[i]);
}

UInt Value::size()
{
   if (!_ptr.operator bool()) return 0;
   return _ptr->size();
}

Logic Value::is(const Value & val)
{
   if (!_ptr.operator bool())
      return Logic(false);

   return Logic(_ptr->is(val._ptr));
}

int Value::find(const Value & val)
{
   if (!_ptr.operator bool())
      return -1;

   return _ptr->find(val._ptr);
}

void Value::push(const Value & val)
{
   if (_ptr)
      _ptr->push(val._ptr);
}

void Value::unshift(const Value & val)
{
   if (_ptr)
      _ptr->unshift(val._ptr);
}

Value Value::pop()
{
   if (!_ptr.operator bool())
      return Value();

   return Value(_ptr->pop());
}

Value Value::shift()
{
   if (!_ptr.operator bool())
      return Value();

   return Value(_ptr->shift());
}

Value Value::splice(Int start, Int deleteCount)
{
   return Value(_ptr->splice(start, deleteCount));
}

Value Value::field(Value &name)
{
   return Value(_ptr->field(_ptr, name._ptr.get()));
}

string Value::toString()
{
   if (!_ptr.operator bool())
      return "";

   return _ptr->toString();
}

Value Value::toFloat()
{
   if (!_ptr.operator bool())
      return Value();

   return _ptr->toFloat();


   switch (type())
   {
   case ValueBase::_FLOAT: return *this;
   case ValueBase::_LOGIC: return get_Logic().p1();
   case ValueBase::_STR:
   {
      string str = get_Str();
      char * e;
      errno = 0;
      Float val = std::strtod(str.c_str(), &e);
      if (*e != '\0' || errno != 0)
      {
         return Value();
      }
      return Value(val);
   }
   }
   return Value();
}

Value Value::get_GraphValue()
{
   if (type() == ValueBase::_GRAPH)
      return ptr();

   auto val = _ptr->get_GraphValue();
   if (!val.operator bool())
      return Value();

   return Value(val); 
}

ValueArr * Value::valueArr()
{
   if (_ptr->type() != ValueBase::_ARRAY)
      return nullptr;

   return (ValueArr*)_ptr.get();
}

ValueEdge * Value::valueEdge()
{
   if (_ptr->type() != ValueBase::_EDGE)
      return nullptr;

   return (ValueEdge*)_ptr.get();
}

ValueFile * Value::valueFile()
{
   if (_ptr->type() != ValueBase::_FILE)
      return nullptr;

   return (ValueFile*)_ptr.get();
}

ValueFloat * Value::valueFloat()
{
   if (_ptr->type() != ValueBase::_FLOAT)
      return nullptr;

   return (ValueFloat*)_ptr.get();
}

ValueFus * Value::valueFus()
{
   if (_ptr->type() != ValueBase::_FUSNUMBER)
      return nullptr;

   return (ValueFus*)_ptr.get();
}

ValueGraph * Value::valueGraph()
{
   if (_ptr->type() != ValueBase::_GRAPH)
      return nullptr;

   return (ValueGraph*)_ptr.get();
}

ValueLogic * Value::valueLogic()
{
   if (_ptr->type() != ValueBase::_LOGIC)
      return nullptr;

   return (ValueLogic*)_ptr.get();
}

ValueMap * Value::valueMap()
{
   if (_ptr->type() != ValueBase::_MAP)
      return nullptr;

   return (ValueMap*)_ptr.get();
}

ValueNode * Value::valueNode()
{
   if (_ptr->type() != ValueBase::_NODE)
      return nullptr;

   return (ValueNode*)_ptr.get();
}

ValueStr * Value::valueStr()
{
   if (_ptr->type() != ValueBase::_STR)
      return nullptr;

   return (ValueStr*)_ptr.get();
}

ValueSVG * Value::valueSVG()
{
   if (_ptr->type() != ValueBase::_SVG)
      return nullptr;

   return (ValueSVG*)_ptr.get();
}

Value::Value(const ValueRef & val)
{
   _ptr = val.ptr();
}

Value::Value(vector<Value>& arr)
{
   vector< shared_ptr<ValueBase> > ptrs;
   for (UInt i = 0; i < arr.size(); i++)
   {
      //простые типы копируем, а сложные добавляем по ссылке
      Value insertedValue;
      insertedValue = arr[i];
      ptrs.push_back(insertedValue._ptr);
   }
   _ptr = shared_ptr<ValueArr>(new ValueArr(ptrs));
}

Value::Value(const shared_ptr<ValueBase> &val)
{
   //пустых указателей Value хранить никогда не будет!
   _ptr = val.operator bool() ? val : ValueNone::ptr();
}

Value::~Value()
{
   //только для отладки
   //_ptr = NULL;
}

ValueIndexRef Value::operator[](const Value & i)
{
   return ValueIndexRef(_ptr, i.ptr());
}

ValueIndexRef Value::operator[](const UInt & i)
{
   return ValueIndexRef(_ptr, Value(Float(i)).ptr());
}

ValueIndexRef Value::operator[](const Int & i)
{
   return ValueIndexRef(_ptr, Value(Float(i)).ptr());
}

std::ostream& operator<<(std::ostream& out, Value & val)
{
   if (!val._ptr.operator bool())
      return out << "???";

   return val.print(out);
}
