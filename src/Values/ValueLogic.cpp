#include "ValueLogic.h"
#include "ValueManager.h"

Logic ValueLogic::not_()
{
   return !_val;
}

Logic ValueLogic::eq(ValueBase * v)
{
   CHECK_TYPES(v, Logic::False);
   return _val == ((ValueLogic *)v)->_val ? Logic(0, 1) : Logic(1, 0);
}

Logic ValueLogic::neq(ValueBase * v)
{
   CHECK_TYPES(v, Logic::True);
   return _val != ((ValueLogic *)v)->_val ? Logic(0, 1) : Logic(1, 0);
}

Logic ValueLogic::lt(ValueBase * v)
{
   CHECK_TYPES(v, Logic::Undef);
   return _val < ((ValueLogic *)v)->_val ? Logic::True : Logic::False;
}

Logic ValueLogic::lteq(ValueBase * v)
{
   CHECK_TYPES(v, Logic::Undef);
   return (_val < ((ValueLogic *)v)->_val || _val == ((ValueLogic *)v)->_val) ? Logic::True : Logic::False;
}

Logic ValueLogic::gt(ValueBase * v)
{    
   return !lteq(v);
}

Logic ValueLogic::gteq(ValueBase * v)
{
   return !lt(v);
}

Logic ValueLogic::or_(ValueBase * v)
{
   CHECK_TYPES(v, Logic::Undef);
   return _val | ((ValueLogic *)v)->_val;
}

Logic ValueLogic::and_(ValueBase * v)
{
   CHECK_TYPES(v, Logic::Undef);
   return _val & ((ValueLogic *)v)->_val;
}

Logic ValueLogic::impl(ValueBase * v)
{
   CHECK_TYPES(v, Logic::Undef);
   return  _val > ((ValueLogic *)v)->_val;
}

shared_ptr<ValueBase> ValueLogic::getArrVal(const shared_ptr<ValueBase> & ptr, const shared_ptr<ValueBase> & key)
{
   Int index = key->get_Int();

   if (index == 0)
      return shared_ptr<ValueBase>(new ValueFloat(_val.p0()));
   else if (index == 1)
      return shared_ptr<ValueBase>(new ValueFloat(_val.p1()));

   return 0;
}

void ValueLogic::setArrVal(shared_ptr<ValueBase>& key, const shared_ptr<ValueBase>& val)
{
   if (val->type() != ValueBase::_FLOAT)
      return;
   Float v = 0;
   val->set(v);

   int index = key->get_Int();

   if (index == 0)
      _val._p0 = v;
   else if (index == 1)
      _val._p1 = v;
}

string ValueLogic::toString()
{
   ostringstream stream;
   stream << _val;
   return stream.str();
}

shared_ptr<ValueBase> ValueLogic::toFloat()
{
   return ValueManager::createValue(_val.p1());
}
