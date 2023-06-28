#include "ValueFloat.h"
#include "ValueNone.h"

shared_ptr<ValueBase> ValueNone::_ptr = 0;

shared_ptr<ValueBase> ValueNone::ptr()
{
   if (!_ptr.operator bool())
      _ptr = shared_ptr<ValueNone>(new ValueNone());
   return _ptr;
}
