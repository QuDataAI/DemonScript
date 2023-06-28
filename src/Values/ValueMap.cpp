#include "ValueMap.h"
#include "ValueManager.h"

//! Меньше x < y
bool operator < (const ValueMapKey &x, const ValueMapKey &y)
{
   if (x.val->type() != y.val->type())
      return x.val->type() < y.val->type();

   return x.val->lt(y.val.get()) == Logic::True;
}

ostream& ValueMap::print(ostream& out)
{
   out << "{";
   bool first = true;
   for (auto &i : _val)
   {
      if (!first)
         out << ",";
      first = false;
      i.second->print(out);      
   }
   return out << "}";
}

shared_ptr<ValueBase> ValueMap::getArrVal(const shared_ptr<ValueBase> & ptr, const shared_ptr<ValueBase> & key)
{
   ValueMapKey keyWrapper(key);
   
   auto i = _val.find(keyWrapper);
   if (i == _val.end())
      return 0;

   return i->second;
}

string ValueMap::toString()
{
   string arrStr = "{";
   bool first = true;
   for (auto &i : _val)
   {
      if (!first)
         arrStr += ",";
      first = false;
      arrStr += i.first.val->toString();
      arrStr += ":";
      arrStr += i.second->toString();
   }
   arrStr += "}";
   return arrStr;
}

Logic ValueMap::eq(ValueBase * v)
{
   if (v->type() != type())                                 // типы не совпадают
      return Logic::False;

   ValueMapType &rightArr = ((ValueMap *)v)->_val;

   if (size() != rightArr.size())                           // размеры не совпадают
      return Logic::False;

   for (auto &i : _val)
   {
      auto j = rightArr.find(i.first);
      if (j == rightArr.end())
         return Logic::False; // Вообще не нашли такого
      if (i.second->eq(j->second.get()) != Logic::True)
         return Logic::False; // один из элементов не совпал
   }

   return Logic::True;
}

void ValueMap::set(ValueMapType & tomap)
{
   tomap.clear();
   for (auto &i : _val)
   {
      tomap[i.first] = i.second;
   }
}

shared_ptr<ValueBase> ValueMap::plus(ValueBase * v)
{
   if (v->type() != ValueBase::_MAP)
      return shared_ptr<ValueMap>(new ValueMap(_val));

   shared_ptr<ValueMap> a = shared_ptr<ValueMap>(new ValueMap(_val));

   ValueMapType &rightArr = ((ValueMap *)v)->_val;

   for (auto &i : rightArr)
   {
      a->_val[i.first] = i.second;
   }

   return a;
}

bool ValueMap::is(const shared_ptr<ValueBase>& val)
{
   return hasKey(val);
}

bool ValueMap::hasKey(const shared_ptr<ValueBase>& key)
{
   ValueMapKey keyWrapper(key);
   auto it = _val.find(keyWrapper);

   return it != _val.end();
}

shared_ptr<ValueBase> ValueMap::findKey(const shared_ptr<ValueBase>& val)
{
   for (auto &i : _val)
   {
      if (i.second->eq(val.get()) == Logic(true))
         return i.first.val;
   }
   return 0;
}

void ValueMap::setArrVal(const shared_ptr<ValueBase> &ptr, const shared_ptr<ValueBase>& val)
{
   ValueMapKey keyWrapper(ptr);
   auto i = _val.find(keyWrapper);

   if (i == _val.end())
   {
      _val[keyWrapper] = val;
      return;
   }

   ValueManager::copyValueToValue(i->second, val);
}

shared_ptr<ValueBase> ValueMap::copyDeep()
{
   shared_ptr<ValueMap> res = make_shared<ValueMap>();

   for (auto &i : _val)
   {
      res->_val[i.first]= i.second.get()->copyDeep();
   }

   return (shared_ptr<ValueBase>)res;
}