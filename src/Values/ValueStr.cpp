#include "ValueStr.h"
#include "ValueManager.h"

shared_ptr<ValueBase> ValueStr::getArrVal(const shared_ptr<ValueBase> & ptr, const shared_ptr<ValueBase> & key)
{
   Int index = key->get_Int();
   Int fixedIndex = (index >= 0) ? index : (Int)_val.size() + index;
   if (fixedIndex < 0 || fixedIndex >= _val.size())
      return ValueManager::createValue("");

   return ValueManager::createValue(string(1, _val[fixedIndex]));
}

void ValueStr::setArrVal(const shared_ptr<ValueBase>& key, const shared_ptr<ValueBase>& val)
{
   if (val->type() != ValueBase::_STR)
      return;
   string v;
   val->set(v);

   if (v.size() == 0)
      return;

   int index = key->get_Int();   
   Int fixedIndex = (index >= 0) ? index : (Int)_val.size() + index;
   if (fixedIndex < 0 || fixedIndex >= _val.size())
      return;

   _val[fixedIndex] = v[0];
}

shared_ptr<ValueBase> ValueStr::toFloat()
{
   char * e;
   errno = 0;
   Float val = std::strtod(_val.c_str(), &e);
   if (*e != '\0' || errno != 0)
   {
      return 0;
   }
   return ValueManager::createValue(val);
}
