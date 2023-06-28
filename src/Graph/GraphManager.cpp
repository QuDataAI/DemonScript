#include "GraphManager.h"
#include "GraphClass.h"
#include "Graph.h"
#include "Value.h"

void GraphManager::init()
{
   // 0-й идентификатор зарезервирован и не является действительным
   addEdgeName("UNKNOWN_EDGE");
   addEdgeName("isa");
   addEdgeName("attr");
   addEdgeName("has");
   addEdgeName("obj");
   addEdgeName("sub");
   addEdgeName("unit");
}

void GraphManager::currentGraphValue(Value* graph)
{
   _currentGraph = graph;
}

Value * GraphManager::currentGraphValue()
{
   if (_currentGraph->type() != ValueBase::_GRAPH)
   {
      TRACE_CRITICAL << "GRAPH has not Graph type!" << endl;
      return 0;
   }
   return _currentGraph;
}

Graph* GraphManager::currentGraph()
{
   if (_currentGraph->type() != ValueBase::_GRAPH)
   {
      TRACE_CRITICAL << "GRAPH has not Graph type!" << endl;
      return 0;
   }
   return _currentGraph->get_Graph();
}

UInt GraphManager::currentGraphClass()
{
   if (_currentGraph->type() != ValueBase::_GRAPH)
   {
      TRACE_CRITICAL << "GRAPH has not Graph type!" << endl;
      return 0;
   }
   return currentGraph()->classID();
}

Int GraphManager::getNewClassID()
{
   UInt newClassID = (UInt)_graphClasses.size();
   _graphClasses.push_back(make_shared<GraphClass>(newClassID));
   return (Int)newClassID;
}

void GraphManager::registerGraph(Graph * graph)
{
   UInt classID = graph->classID();
   if (wrongClass(classID))
      return;
   _graphClasses[classID]->registerGraph(graph);
}

void GraphManager::unregisterGraph(Graph * graph)
{
   UInt classID = graph->classID();
   if (classID >= _graphClasses.size())
      return;
   _graphClasses[classID]->unregisterGraph(graph);
}

UInt GraphManager::addNode(UInt classID, const string & name)
{
   UInt id = 0;

   if (wrongClass(classID))
      return id;

   //проверяем не существует ли узел с таким имененем в других классах
   for (size_t i = 0; i < _graphClasses.size(); i++)
   {
      if (classID == i)
         continue;
      id = _graphClasses[i]->node(name);
      if (id > 0)
      {
         TRACE_ERROR << "GraphManager::addNode node " << name << " already exists in other class " << to_string(i) << "\n";
         return id;
      }
   }
   id = _graphClasses[classID]->addNode(name);
   return id;
}

void GraphManager::addNodeName(const string & name)
{
   _nodesNames.insert(name);
}

bool GraphManager::isNodeName(const string & name)
{
   return _nodesNames.find(name) != _nodesNames.end();
}

UInt GraphManager::addNodes(UInt classID, UInt n)
{
   if (wrongClass(classID))
      return 0;

   return _graphClasses[classID]->addNodes(n);
}

void GraphManager::deleteNodes(UInt classID)
{
   if (wrongClass(classID))
      return;

   return _graphClasses[classID]->deleteNodes();
}

Int GraphManager::addEdgeName(const string & name)
{
   string sname = name;
   if (name[0] == '@')
   {
      sname = name.substr(1, name.size() - 1);
   }
   Int id  = (Int)_edges.size();
   auto it = _edgeNames.find(sname);
   if (it == _edgeNames.end()) {
      _edgeNames.insert(std::pair<string, Int>(sname, id));
      _edges.push_back(sname);
   }
   else 
   {
      if (it->second >= EDGE_RESERVED_MAX)
      {
         TRACE_CRITICAL << "!!! GraphManager::add_edge> edge with name: " << sname << " is already added" << endl;
      }      
      id = it->second;
   }
   return (Int)id;
}

string GraphManager::edgeName(Int edgeID)
{
   edgeID = edgeID < 0 ? -edgeID : edgeID;
   if (wrongEdge(edgeID))
   {
      TRACE_ERROR << "!!! GraphManager::edgeName> Conflict id of edge:" << edgeID << endl;
      return "???";
   }
   return _edges[edgeID];
}

string GraphManager::nodeName(UInt classID, UInt nodeID)
{   
   if (wrongClass(classID))
      return "???";

   return _graphClasses[classID]->nodeName(nodeID);
}

const map<string, UInt>& GraphManager::nodeNames(UInt classID)
{
   return _graphClasses[classID]->nodeNames();
}

Int GraphManager::edge(const string & name)
{
   if (name.size() == 0)
   {
      TRACE_CRITICAL << "!!! GraphManager::edge> Edge name is empty: " << name << "\n";
      return 0;
   }
   string sname = name;
   if (name[0] == '@')
   {
      sname = name.substr(1, name.size() - 1);
   }

   auto it = _edgeNames.find(sname);
   if (it == _edgeNames.end()) {
      TRACE_CRITICAL << "!!! GraphManager::edge> Unknown edge name: " << sname << "\n";
      return 0;
   }
   return it->second;
}

UInt GraphManager::edges()
{
   return (UInt)(_edges.size() - 1);
}

const map<string, Int>& GraphManager::edgeNames()
{
   return _edgeNames;
}

UInt GraphManager::node(UInt classID, const string & name)
{
   if (wrongClass(classID))
      return 0;

   return _graphClasses[classID]->node(name);
}

UInt GraphManager::node(const string & name)
{
   return node(currentGraph()->classID(), name);
}

UInt GraphManager::nodes(UInt classID)
{
   if (wrongClass(classID))
      return 0;

   return _graphClasses[classID]->nodes();
}

Bool GraphManager::isEdgeName(const string & name)
{
   return _edgeNames.find(name) != _edgeNames.end();
}

Bool GraphManager::addEdge(UInt classID, Int knd, UInt xID, UInt yID, Logic val, Graph* yGraph/* = 0*/)
{
   if (wrongClass(classID))
      return false;

   return _graphClasses[classID]->addEdge(knd, xID, yID, val, yGraph);
}

void GraphManager::nodeValue(UInt classID, Graph* graph, UInt id, shared_ptr<ValueBase> val)
{
   if (wrongClass(classID))
      return;

   return _graphClasses[classID]->nodeValue(graph, id, val);
}

void GraphManager::setAttr(UInt classID, UInt obNodeID, Graph * obGraph, UInt attrNodeID, Graph * attrGraph, const shared_ptr<ValueBase>& attrValue)
{
   if (wrongClass(classID))
      return;

   return _graphClasses[classID]->setAttr(obNodeID, obGraph, attrNodeID, attrGraph, attrValue);
}

Bool GraphManager::wrongClass(UInt classID)
{
   return (classID >= _graphClasses.size());
}

Bool GraphManager::wrongEdge(Int edgeID)
{
   return edgeID <= 0 || edgeID >= (Int)_edges.size();
}

Bool GraphManager::wrongNode(UInt classID, UInt nodeID)
{
   if (wrongClass(classID))
      return true;

   return _graphClasses[classID]->wrongNode(nodeID);
}

Bool GraphManager::close(UInt classID, Bool val)
{
   if (wrongClass(classID))
      return true;

   return _graphClasses[classID]->close(val);
}

void GraphManager::clear()
{
   _graphClasses.clear();
   _edgeNames.clear();
   _edges.clear();
   _currentGraph = NULL;
}

CurrentGraph::CurrentGraph()
{
   save();
}

CurrentGraph::CurrentGraph(const Value& newGraphVal)
{
   save();   
   set(newGraphVal);
}

CurrentGraph::~CurrentGraph()
{
   restore();
}

void CurrentGraph::save()
{
   _savedGraph = GraphManager::instance().currentGraphValue()->ptr();
}

void CurrentGraph::set(const Value& newGraphVal)
{
   if (newGraphVal.ptr()->type() != ValueBase::_GRAPH)
   {
      TRACE_CRITICAL << "Incorret type of new graph" << endl;
      return;
   }
   *GraphManager::instance().currentGraphValue() = newGraphVal.ptr();
}

void CurrentGraph::restore()
{
   if (_savedGraph)
   {
      *GraphManager::instance().currentGraphValue() = _savedGraph;
      _savedGraph = 0;
   }      
}

