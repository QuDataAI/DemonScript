#include "ValueRef.h"
#include "Value.h"

ValueRef::ValueRef(const Value & val) :
   _ptr(val.ptr())
{
}
