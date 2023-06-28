#include "ValueNode.h"
#include "GraphManager.h"
#include "ValueManager.h"
#include "Value.h"

ValueNode::ValueNode(UInt val):
   _val(val)
{
   _classID = GraphManager::instance().currentGraph()->classID();
}

ValueNode::ValueNode(UInt classID, UInt val) :
   _classID(classID), 
   _val(val)
{

}

ValueNode::ValueNode(shared_ptr<ValueBase> graphValue, UInt val) :
   _graphValue(graphValue),
   _val(val),
   _classID(graphValue->get_Graph()->classID())
{

}

ValueNode::ValueNode(UInt classID, UInt val, shared_ptr<ValueBase> graphValue) :
   _classID(classID),
   _val(val), 
   _graphValue(graphValue)
{

}

Graph * ValueNode::get_Graph()
{
   if (_graphValue)
      return _graphValue->get_Graph();
   else
   {
      Graph * current = GraphManager::instance().currentGraph();
      if (current->classID() == _classID)
         return current;
      else
      {
         TRACE_CRITICAL << "current graph from different class (current id = " << current->classID() << "; value id = " << _classID << ")" << endl;
         return 0;
      }         
   }
}

void ValueNode::set_Node(const ValueNode & val)
{
   *this = val;
}

shared_ptr<ValueBase> ValueNode::get_GraphValue()
{
   if (_graphValue)
      return _graphValue;
   else
   {
      Graph * current = GraphManager::instance().currentGraph();
      if (current->classID() == _classID)
         return GraphManager::instance().currentGraphValue()->ptr();
      else
      {
         TRACE_CRITICAL << "current graph from different class (current id = " << current->classID() << "; value id = " << _classID << ")" << endl;
         return ValueNone::ptr();
      }
   }
}

string ValueNode::name()
{   
   return GraphManager::instance().nodeName(_classID, _val);
}

string ValueNode::toString()
{
   string fullName = "";

   if (_graphValue)
   {
      fullName = _graphValue->get_Graph()->name() + ".";
   }

   fullName += name();
   return fullName;
}

shared_ptr<ValueBase> ValueNode::id()
{
   return ValueManager::createValue(ValueNode(_classID, _val));
}

void ValueNode::bind()
{
   if (_classID != GraphManager::instance().currentGraph()->classID())
   {
      TRACE_CRITICAL << "ValueNode::bind() - trying to bind node to graph with different classID" << endl;
      return;
   }

   _graphValue = GraphManager::instance().currentGraph()->_valGraph.lock();
   _classID = _graphValue->get_Graph()->classID();
}

void ValueNode::unbind()
{
   if (_graphValue)
      _classID = _graphValue->get_Graph()->classID();
   _graphValue = NULL;   
}
