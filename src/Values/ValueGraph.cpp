#include "ValueGraph.h"
#include "ValueManager.h"

shared_ptr<ValueGraph> ValueGraph::create()
{
   shared_ptr<ValueGraph> ptr = make_shared<ValueGraph>();
   ptr->_val.setValueGraph(ptr);
   return ptr; 
}

Logic ValueGraph::eq(ValueBase * v)
{
   if (v->type() != type())                  // типы не совпадают
      return Logic(1, 0);
   return _val == ((ValueGraph *)v)->_val;
}

Logic ValueGraph::neq(ValueBase * v)
{
   if (v->type() != type())                  // типы не совпадают
      return Logic(1, 0);
   return _val != ((ValueGraph *)v)->_val;
}

shared_ptr<ValueBase> ValueGraph::plus(ValueBase * v)
{
   if (v->type() != type())                  // типы не совпадают
      return ValueNone::ptr();

   return (shared_ptr<ValueBase>)(_val + ((ValueGraph *)v)->_val);
}

shared_ptr<ValueBase> ValueGraph::minus(ValueBase * v)
{
   if (v->type() != type())                  // типы не совпадают
      return ValueNone::ptr();

   return (shared_ptr<ValueBase>)(_val - ((ValueGraph *)v)->_val);
}

shared_ptr<ValueBase> ValueGraph::field(shared_ptr<ValueBase> ptr, ValueBase *name)
{
   if (name->type() != ValueBase::_NODE)
      return 0;

   if (_val.classID() != name->get_GraphClassID())
   {
      TRACE_CRITICAL << "field and object from different graph classes" << endl;
      return 0;
   }
   
   return ValueManager::createValue(ValueNode(ptr, name->get_UInt()));
}

shared_ptr<ValueBase> ValueGraph::copy() const
{
   shared_ptr<ValueGraph> ptr = make_shared<ValueGraph>(_val);
   ptr->_val.setValueGraph(ptr);
   return ptr;
}

shared_ptr<ValueBase> ValueGraph::getArrVal(const shared_ptr<ValueBase> & ptr, const shared_ptr<ValueBase> & key)
{
   Int index = key->get_Int();

   if (index == 0 || index >= _val._nodes.size())
   {
      TRACE_CRITICAL << "index " << index << " is out of range (nodes: " << (_val._nodes.size()-1) << ")" << endl;
      return 0;
   }
   return ValueManager::createValue(ValueNode(ptr, index));
}

string ValueGraph::toString()
{
   stringstream str;
   _val.print(str, 0);
   return str.str();
}
