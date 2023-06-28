#include "ValueFloat.h"
#include "ValueManager.h"
#include "GraphManager.h"

Logic ValueFloat::eq(ValueBase * v)
{
   CHECK_TYPES(v, Logic::False);
   return (IS_APPROXIMATELY_EQUAL_D(_val, ((ValueFloat *)v)->_val)) ? Logic(0, 1) : Logic(1, 0);
}

Logic ValueFloat::neq(ValueBase * v)
{
   CHECK_TYPES(v, Logic::True);
   return (!(IS_APPROXIMATELY_EQUAL_D(_val, ((ValueFloat *)v)->_val))) ? Logic(0, 1) : Logic(1, 0);
}

Logic ValueFloat::lt(ValueBase * v)
{
   CHECK_TYPES(v, Logic::Undef);
   return (IS_APPROXIMATELY_LESS_D(_val, ((ValueFloat *)v)->_val)) ? Logic(0, 1) : Logic(1, 0);
}

Logic ValueFloat::lteq(ValueBase * v)
{
   return !gt(v);
}

Logic ValueFloat::gt(ValueBase * v)
{
   CHECK_TYPES(v, Logic::Undef);
   return (IS_APPROXIMATELY_MORE_D(_val, ((ValueFloat *)v)->_val)) ? Logic(0, 1) : Logic(1, 0);
}
Logic ValueFloat::gteq(ValueBase * v)
{
   return !lt(v);
}

shared_ptr<ValueBase> ValueFloat::divInt(ValueBase * v)
{
   return shared_ptr<ValueBase>(new ValueFloat(Float((int)_val / (int)(((ValueFloat *)v)->_val))));
}

shared_ptr<ValueBase> ValueFloat::mod(const shared_ptr<ValueBase>& val)
{
   if (val->type() != ValueBase::_FLOAT)
      return 0;

   Float b   = val->get_Float();
   Float res = fmod(_val, b);

   return ValueManager::createValue(Float(res));
}

string ValueFloat::toString()
{
   if (_val == (int)_val)
      return std::to_string((int)_val);
   else
      return std::to_string(_val);
}

shared_ptr<ValueBase> ValueFloat::toFloat()
{
   return ValueManager::createValue(_val);
}

FusNumber ValueFloat::get_FusNumber()
{
   return FusNumber(_val, _val, _val, _val);
}