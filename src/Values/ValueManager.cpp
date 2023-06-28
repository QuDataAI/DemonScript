#include "ValueManager.h"

shared_ptr<ValueBase> ValueManager::getValue(const shared_ptr<ValueBase>& val)
{
   return (val->setType() == ValueBase::ST_COPY) ? val->copy() : val;
}

void ValueManager::copyValueToValue(shared_ptr<ValueBase> &leftVal, const shared_ptr<ValueBase>& rightVal)
{
   leftVal = getValue(rightVal);
}
