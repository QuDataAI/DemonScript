#include "GraphClass.h"
#include "Graph.h"

GraphClass::GraphClass(UInt & id):
   _id(id)
{
   addNode("$UNKNOWN_NODE");              // нулевой id - "пустой объект"
}

void GraphClass::registerGraph(Graph * graph)
{
   //int index = graphIndex(graph);

   //if (index >= 0)
   //   return;

   if (_graphs.find(graph) != _graphs.end())
      return;

   //_graphs.push_back(graph);
   _graphs.insert(graph);

   synhronizeGraph(graph);
}

void GraphClass::unregisterGraph(Graph * graph)
{
   _graphs.erase(graph);
   //int index = graphIndex(graph);

   //if (index < 0)
   //   return;

   //_graphs.erase(_graphs.begin() + index);
}

UInt GraphClass::addNode(string name)
{
   UInt id = node(name);
   if (id > 0)
   {
      //уже существует с таким именем
      TRACE_CRITICAL << "GraphManager::addNode node " << name << " is already added" << endl;
      return id;
   }

   id = (UInt)_nodes.size();

   _nodes.push_back(name);
   _nodeNames[name] = id;

   //теперь передаем новый узел во все графы текущего класса
   //for (size_t i = 0; i < _graphs.size(); i++)
   //{
   //   _graphs[i]->add_node(id);
   //}
   for (auto & it : _graphs)
   {
      it->add_node(id);
   }
   return id;
}

UInt GraphClass::addNodes(UInt n)
{
   UInt id = (UInt)_nodes.size();

   for (UInt i = 0; i < n; i++)
   {
      string name = /*"$#" +*/ to_string(i + id);
      addNode(name);
   }
   return nodes();
}

void GraphClass::deleteNodes()
{
   for (auto & it : _graphs)
   {
      it->delete_nodes();
   }
   _nodeNames.clear();
   _nodes.clear();
   addNode("$UNKNOWN_NODE");              // нулевой id - "пустой объект"
}

UInt GraphClass::node(const string & name)
{
   auto it = _nodeNames.find(name);

   if (it == _nodeNames.end())
      return 0;

   return it->second;
}

UInt GraphClass::nodes()
{
   return (UInt)(_nodes.size() - 1);   //0-й зарезервирован
}

string GraphClass::nodeName(const UInt id)
{
   if (wrongNode(id))
      return "???";

   return _nodes[id];
}

const map<string, UInt>& GraphClass::nodeNames()
{
   return _nodeNames;
}

Bool GraphClass::wrongNode(UInt id)
{
   return (id >= _nodes.size());
}

Bool GraphClass::addEdge(Int knd, UInt xID, UInt yID, Logic val, Graph* yGraph/* = 0*/)
{
   //for (size_t i = 0; i < _graphs.size(); i++)
   //{
   //   _graphs[i]->add(knd, xID, yID, val, yGraph);
   //}
   for (auto & it : _graphs)
   {
      it->add(knd, xID, yID, val, yGraph);
   }

   return true;
}

void GraphClass::nodeValue(Graph * graph, UInt id, shared_ptr<ValueBase> val)
{
   //for (size_t i = 0; i < _graphs.size(); i++)
   //{
   //   _graphs[i]->nodeValue(id, val, graph == _graphs[i]);
   //}
   for (auto & it : _graphs)
   {
      it->nodeValue(id, val, graph == it);
   }
}

void GraphClass::setAttr(UInt obNodeID, Graph * obGraph, UInt attrNodeID, Graph * attrGraph, const shared_ptr<ValueBase>& attrValue)
{
   //for (size_t i = 0; i < _graphs.size(); i++)
   //{
   //   _graphs[i]->set_attr(obNodeID, attrNodeID, attrGraph, obGraph == _graphs[i], attrValue);
   //}
   for (auto & it : _graphs)
   {
      it->set_attr(obNodeID, attrNodeID, attrGraph, obGraph == it, attrValue);
   }
}

Bool GraphClass::close(Bool val)
{
   for (auto & it : _graphs)
   {
      it->close(val);
   }
   return val;
}

//int GraphClass::graphIndex(Graph * graph)
//{
//   for (size_t i = 0; i < _graphs.size(); i++)
//   {
//      if (_graphs[i] == graph)
//      {
//         return (int)i;
//      }
//   }
//
//   return -1;
//}

void GraphClass::synhronizeGraph(Graph * graph)
{
   for (size_t i = graph->_nodes.size(); i < _nodes.size(); i++)
   {
      graph->add_node((UInt)i);
   }
}

