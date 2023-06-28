#include "ValueArr.h"
#include "ValueManager.h"
#include "Value.h"
#include "Function.h"
#include "Stack/StackFrame.h"

ostream& ValueArr::print(ostream& out)
{
   out << "[";
   for (UInt i = 0; i < _val.size(); i++) {
      _val[i]->print(out);
      if (i + 1 < _val.size()) out << ",";
   }
   return out << "]";
}

shared_ptr<ValueBase> ValueArr::getArrVal(const shared_ptr<ValueBase> & ptr, const shared_ptr<ValueBase> & key)
{
   Int index = key->get_Int();
   Int fixedIndex = (index >= 0) ? index : (Int)_val.size() + index;
   if (fixedIndex < 0 || fixedIndex >= _val.size())
      return 0;

   return _val[fixedIndex];
}

string ValueArr::toString()
{
   string arrStr = "[";
   for (UInt i = 0; i < _val.size(); i++) {
      arrStr += _val[i]->toString();
      if (i + 1 < _val.size()) 
         arrStr+=",";
   }
   arrStr += "]";
   return arrStr;
}

Logic ValueArr::eq(ValueBase * v)
{
   if (v->type() != type())                                 // типы не совпадают
      return Logic::False;

   vector< shared_ptr<ValueBase> > &rightArr = ((ValueArr *)v)->_val;

   if (size() != rightArr.size())                           // размеры не совпадают
      return Logic::False;

   for (UInt i = 0; i < size(); i++)
   {
      if (_val[i]->eq(rightArr[i].get()) != Logic::True)    // один из элементов не совпал
         return Logic::False;
   }

   return Logic::True;
}

Logic ValueArr::neq(ValueBase * v)
{
   return !eq(v);
}

void ValueArr::set(vector<shared_ptr<ValueBase>>& arr)
{
   arr.clear();
   for (UInt i = 0; i < _val.size(); i++)
      arr.push_back(_val[i]);
}

shared_ptr<ValueBase> ValueArr::plus(ValueBase * v)
{
   if (v->type() != ValueBase::_ARRAY)
      return shared_ptr<ValueArr>(new ValueArr(_val));

   shared_ptr<ValueArr> a = shared_ptr<ValueArr>(new ValueArr(_val));

   vector< shared_ptr<ValueBase> > &rightArr = ((ValueArr *)v)->_val;

   for (UInt i = 0; i < rightArr.size(); i++)
      a->_val.push_back(rightArr[i]);
   return a;
}

void ValueArr::mix()
{
   for (UInt num = 0; num < 10 * _val.size(); num++) {
      UInt i1 = rand() % _val.size();
      UInt i2 = rand() % _val.size();
      shared_ptr<ValueBase> v = _val[i1];
      _val[i1] = _val[i2]; _val[i2] = v;
   }
}

bool ValueArr::is(const shared_ptr<ValueBase>& val)
{
   return (find(val)>=0);
}

int ValueArr::find(const shared_ptr<ValueBase>& val)
{
   for (UInt i = 0; i < _val.size(); i++)
   {
      if (_val[i]->eq(val.get()) == Logic(true))
         return (int)i;
   }
   return -1;
}

void ValueArr::push(const shared_ptr<ValueBase>& val)
{
   _val.push_back(ValueManager::getValue(val));
}

void ValueArr::push()
{
   _val.push_back(ValueManager::createValue());
}

void ValueArr::unshift(const shared_ptr<ValueBase>& val)
{
   _val.insert(_val.begin(), ValueManager::getValue(val));
}

shared_ptr<ValueBase> ValueArr::pop()
{
   if (size() == 0)
      return 0;

   shared_ptr<ValueBase> val = _val.back();
   _val.pop_back();

   return val;
}

shared_ptr<ValueBase> ValueArr::shift()
{
   if (size() == 0)
      return 0;

   shared_ptr<ValueBase> val = _val[0];
   _val.erase(_val.begin());

   return val;
}

shared_ptr<ValueBase> ValueArr::splice(Int start, Int deleteCount)
{
   vector< shared_ptr<ValueBase> > deletedArr;
   UInt currentSize = size();
   if (start < 0)
   {
      start = (Int)currentSize + start;
   }
   if (start < 0)
   {
      start = 0;
   }
   if (start >= (Int)currentSize)
      return ValueManager::createValue(deletedArr);
   if (start + deleteCount > currentSize)
   {
      deleteCount = (Int)currentSize - start;
   }
   if (deleteCount == 0)
      return ValueManager::createValue(deletedArr);

   for (size_t i = start; i < start + deleteCount && i<_val.size(); i++)
   {
      deletedArr.push_back(_val[i]);
   }
   _val.erase(_val.begin() + start, _val.begin() + start + deleteCount);

   return ValueManager::createValue(deletedArr);
}

void ValueArr::setArrVal(const shared_ptr<ValueBase> &key, const shared_ptr<ValueBase>& val)
{
   Int index = key->get_Int();
   if (index >= _val.size())
      return;

   ValueManager::changeValue(_val[index], val);
}

FusNumber ValueArr::get_FusNumber()
{
   if (_val.size() > 3)
   {
      return FusNumber(_val[0]->get_Float(), _val[1]->get_Float(), _val[2]->get_Float(), _val[3]->get_Float());
   }
   else if (_val.size() > 2)
   {
      return FusNumber(_val[0]->get_Float(), _val[1]->get_Float(), _val[1]->get_Float(), _val[2]->get_Float());
   }
   else if (_val.size() > 1)
   {
      return FusNumber(_val[0]->get_Float(), _val[0]->get_Float(), _val[1]->get_Float(), _val[1]->get_Float());
   }
   else if (_val.size() > 0)
   {
      return FusNumber(_val[0]->get_Float(), _val[0]->get_Float(), _val[0]->get_Float(), _val[0]->get_Float());
   }

   return FusNumber(0, 0, 0, 0);
}

bool ValueArr::ValueArrDemonComparator::operator()(const shared_ptr<ValueBase>& a, const shared_ptr<ValueBase>& b)
{
   _compareFn->_codeCommon->_local_variables[0]->_val = Value(a);
   _compareFn->_codeCommon->_local_variables[1]->_val = Value(b);
   return _compareFn->run().get_Float() < 0;
}


void ValueArr::sort(FunLambdaPtr compareFn)
{
   if (compareFn)
   {
      compareFn->_vals.resize(2);
      std::sort(_val.begin(), _val.end(), ValueArrDemonComparator(compareFn));
   }
   else
   {      
      std::sort(_val.begin(), _val.end(), [](const shared_ptr<ValueBase> &a, const shared_ptr<ValueBase> &b) -> bool
      {
         if (a->type() != b->type())
            return a->type() < b->type();

         return a->lt(b.get()) == Logic::True;
      });
   }   
}

shared_ptr<ValueBase> ValueArr::copyDeep()
{
   shared_ptr<ValueArr> res = make_shared<ValueArr>();
   for (size_t i = 0; i < _val.size(); i++)
   {
      res->push(_val[i]->copyDeep());
   }
   return (shared_ptr<ValueBase>)res;
}

