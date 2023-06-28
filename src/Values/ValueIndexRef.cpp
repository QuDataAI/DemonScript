#include "ValueIndexRef.h"
#include "ValueNone.h"
#include "Value.h"

shared_ptr<ValueBase> ValueIndexRef::operator->()
{
   return value();   
}

ValueIndexRef & ValueIndexRef::operator=(const Value & val)
{
   if (!_ptr.operator bool())
      return *this;

   _ptr->setArrVal(_key, val.ptr());
   return *this;
}

shared_ptr<ValueBase> ValueIndexRef::value() const 
{
   if (!_ptr.operator bool())
      return ValueNone::ptr();

   shared_ptr<ValueBase> val = _ptr->getArrVal(_ptr, _key);

   if (!val.operator bool())
      return ValueNone::ptr();

   return val;
}
