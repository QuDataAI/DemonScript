#include "ValueFus.h"

shared_ptr<ValueBase> ValueFus::getArrVal(const shared_ptr<ValueBase> & ptr, const shared_ptr<ValueBase> & key)
{
   int index = key->get_Int();

   if (index == 0)
      return shared_ptr<ValueBase>(new ValueFloat(_val.v0()));
   else if (index == 1)
      return shared_ptr<ValueBase>(new ValueFloat(_val.c0()));
   else if (index == 2)
      return shared_ptr<ValueBase>(new ValueFloat(_val.c1()));
   else if (index == 3)
      return shared_ptr<ValueBase>(new ValueFloat(_val.v1()));

   return 0;
}

void ValueFus::setArrVal(const shared_ptr<ValueBase> &key, const shared_ptr<ValueBase>& val)
{
   if (val->type() != ValueBase::_FLOAT)
      return;
   Float v = 0;
   val->set(v);

   int index = key->get_Int();

   if (index == 0)
      _val._v0 = v;
   else if (index == 1)
      _val._c0 = v;
   else if (index == 2)
      _val._c1 = v;
   else if (index == 3)
      _val._c1 = v;
}

string ValueFus::toString()
{
   ostringstream stream;
   stream << _val;
   return stream.str();
}
