#include "Graph.h"
#include "GraphInvariants.h"
#include "Value.h"
#include "GraphManager.h"
#include "Senses.h"
#include "Expr.h"
#include "Sys/Utils.h"
#include "Sys/ThirdParty/VF2/VF2.h"


UInt Graph::_getEdgeCnt = 0;   
UInt Graph::_setEdgeCnt = 0;   

//=======================================================================================
//                                  Graph
//=======================================================================================

Graph::Graph():   
   _manager(GraphManager::instance())
{
   _edges_blocked = 0;
   _changes = _edges_changes = 0;
   _graph_name = "GRAPH";
   _classID = _manager.getNewClassID();
   _manager.registerGraph(this);
}

Graph::Graph(const Graph & graph):
   _manager(GraphManager::instance())
{
   copy(graph);
}

Graph::~Graph()
{
   _manager.unregisterGraph(this);
}

// Очистить (полностью) текущий граф, кроме $UNKNOWN_NODE, @UNKNOWN_EDGE
//
Graph & Graph::clear()
{
   _nodes.clear();
   _edges_blocked = 0;
   _changes = _edges_changes = 0;
   _classID = 0;
   _axioms.clear();
   updated();
   return *this;
}
//=======================================================================================
// Скопировать в текущий граф, граф graph
//
Graph & Graph::copy(const Graph & graph)
{
   _manager.unregisterGraph(this);
   _nodes         = graph._nodes;
   _changes       = graph._changes; 
   _edges_blocked = graph._edges_blocked;
   _edges_changes = graph._edges_changes;
   _graph_name    = graph._graph_name;
   _classID       = graph._classID; 
   _verbose       = graph._verbose;
   _autoSort      = graph._autoSort;
   _close         = graph._close;
   if (graph._valAttrGraph)
   {
      _valAttrGraph = dynamic_pointer_cast<ValueGraph>(graph._valAttrGraph->copy());
   }
   _valDefSensesGraph = graph._valDefSensesGraph;
   setOwner();
   _manager.registerGraph(this);
   _changes++;
   updated();
   return *this;
}

Graph & Graph::copyScheme(const Graph & graph)
{
   _manager.unregisterGraph(this);
   _changes       = graph._changes; 
   _edges_blocked = graph._edges_blocked;
   _edges_changes = graph._edges_changes;
   _graph_name    = graph._graph_name;
   _classID       = graph._classID;
   _verbose       = graph._verbose;
   _autoSort      = graph._autoSort;

   _nodes.clear();
   for (auto node : graph._nodes)
   {
      _nodes.push_back(node.id());
   }

   _manager.registerGraph(this);
   updated();
   return *this;
}

//=======================================================================================
// Идентификатор узла по имени или 0
//
UInt Graph::node(const string & name)
{  
   return _manager.node(_classID, name);
}

Int  Graph::edge(const string &name)
{
   return _manager.edge(name);
}

string Graph::edge_name(Int id)
{
   return _manager.edgeName(id);
}

string  Graph::node_name(UInt id)
{
   return _manager.nodeName(_classID, id);
}

void Graph::nodeValue(UInt id, shared_ptr<ValueBase> val, Bool change/* = true*/)
{
   if (wrong_node(id)) {
      TRACE_ERROR << "!!! Graph::node_value> Unknown node id" << id << "\n";
      return;
   }
   if (_nodes[id].value() && !change)
      return;

   _nodes[id].value(val);
}

shared_ptr<ValueBase> Graph::nodeValue(UInt id)
{
   if (wrong_node(id)) {
      TRACE_ERROR << "!!! Graph::node_value> Unknown node id" << id << "\n";
      return 0;
   }
   return _nodes[id].value();
}

Bool Graph::set_attr(UInt obNodeID, UInt attrNameNodeID, Graph* attrNameGraph, Bool change, const shared_ptr<ValueBase> &attrValue)
{
   shared_ptr<ValueGraph>    attrNameGraphVal = attrNameGraph->_valGraph.lock();
   UInt                      attrValueNodeID = 0;
   shared_ptr<ValueGraph>    attrValueGraphVal;

   Graph* attrGr = attrGraph();

   if (!find_attr_node(obNodeID, attrNameNodeID, attrNameGraphVal, true, attrValueNodeID, attrValueGraphVal))
   {
      //атрибута еще нет, добавляем

      //создаем узел, который будет хранить атрибут      
      UInt attrGraphClass = _valAttrGraph->_val.classID();
      attrValueNodeID = _manager.addNodes(attrGraphClass, 1);

      //устанавливаем связи с атрибутом
      add(ReservedEdgeIDs::EDGE_ATTR, obNodeID,   attrValueNodeID, Logic::True, &_valAttrGraph->_val);
      attrGr->add(ReservedEdgeIDs::EDGE_ISA,  attrValueNodeID, attrNameNodeID, Logic::True, attrNameGraph == attrGr ? 0 : attrNameGraph);

      //_manager.addEdge(_classID, ReservedEdgeIDs::EDGE_ATTR, obNodeID, (UInt)attrValueNodeID, Logic::True);
      //_manager.addEdge(_classID, ReservedEdgeIDs::EDGE_ISA, (UInt)attrValueNodeID, attrNodeID, Logic::True, attrGraph == this ? NULL : attrGraph);      

      //устаналиваем новое значение
      attrGr->nodeValue((UInt)attrValueNodeID, attrValue);
   }
   else
   {
      if (change)
      {
         //меняем значение
         attrGr->nodeValue((UInt)attrValueNodeID, attrValue);
      }
   }   
   return true;
}

shared_ptr<ValueBase> Graph::get_attr(UInt obNodeID, UInt attrNameNodeID, Graph* attrNameGraph, Bool localSearch)
{
   shared_ptr<ValueGraph>    attrNameGraphVal   = attrNameGraph->_valGraph.lock();
   UInt                      attrValueNodeID    = 0;
   shared_ptr<ValueGraph>    attrValueGraphVal;

   if (!find_attr_node(obNodeID, attrNameNodeID, attrNameGraphVal, localSearch, attrValueNodeID, attrValueGraphVal))
   {
      //атрибута нет
      return 0;
   }

   //получаем значение
   return attrValueGraphVal->_val.nodeValue((UInt)attrValueNodeID);
}

Bool Graph::find_attr_node(UInt obNodeID, UInt attrNodeID, shared_ptr<ValueGraph> attrGraphVal, Bool localSearch, UInt & attrValueNodeID, shared_ptr<ValueGraph> &attrValueGraphVal)
{
   Graph* attrGraph = attrGraphVal ? attrGraphVal->get_Graph() : this;

   if (wrong_node(obNodeID) || attrGraph->wrong_node(attrNodeID)) {
      TRACE_ERROR << "!!! Graph::find_attr_node> Unknown names of nodes ("
         << obNodeID << "," << attrNodeID << ")\n";
      return false;
   }

   Node & nx = _nodes[obNodeID];
   Int    attrKnd = ReservedEdgeIDs::EDGE_ATTR;
   Int    isaKnd  = ReservedEdgeIDs::EDGE_ISA;

   queue<ValueGraphPtrNodeID>  open;                        // очередь для дальнейших переходов
   set<Node*>        close;                                 // список исследованных узлов

   open.push(ValueGraphPtrNodeID(_valGraph.lock(), &nx));   // помещаем стартовый узел

   while (!open.empty())
   {                                              // пока очередь не пуста
      ValueGraphPtrNodeID n = open.front();       // берём первый путь
      open.pop();                                 // и удаляем его
      close.insert(n._node);                      // запоминаем, что в нём были

      vector<Edge> &edgesAttrKnd = n._node->edgesOut(attrKnd)._edges;
      for (auto &edgeAttr: edgesAttrKnd)
      {
         ValueGraphPtr graphN2Ptr = edgeAttr._valGraph2 ? edgeAttr._valGraph2 : n._valGraph;
         Graph *         graphN2  = graphN2Ptr->get_Graph();
         Node &         attrNode  = graphN2->_nodes[edgeAttr._n2];
         vector<Edge> &attrIsaEdges = attrNode.edgesOut(isaKnd)._edges;
         if (n._valGraph == attrGraphVal)
         {
            //поиск внутри графа
            for (auto &edgeAttrIsa : attrIsaEdges)
            {
               if (edgeAttrIsa._n2 == attrNodeID)
               {
                  attrValueNodeID   = edgeAttr._n2;
                  attrValueGraphVal = graphN2Ptr;
                  return true;
               }

            }
         }
         else
         {
            //поиск во внешних ссылках
            for (auto &edgeAttrIsa : attrIsaEdges)
            {
               if (edgeAttrIsa._n2 == attrNodeID && edgeAttrIsa._valGraph2 == attrGraphVal)
               {
                  attrValueNodeID   = edgeAttr._n2;
                  attrValueGraphVal = graphN2Ptr;
                  return true;
               }
            }
         }
      }

      if (localSearch)
         return false; // попросили искать только в текущем узле

      //атрибут в текущем узле не найден, поднимаемся вверх по отношению isa
      vector<Edge> &edgesIsaKnd = n._node->edgesOut(isaKnd)._edges;
      for (auto &edgeIsa : edgesIsaKnd)
      {
         ValueGraphPtr graphN2Ptr = edgeIsa._valGraph2 ? edgeIsa._valGraph2 : n._valGraph;
         Graph *         graphN2 = graphN2Ptr->get_Graph();
         Node *         isaNode  = &graphN2->_nodes[edgeIsa._n2];
         if (close.find(isaNode) == close.end())
            open.push(ValueGraphPtrNodeID(graphN2Ptr, isaNode));           // добавляем узел в кандидаты для поиска атрибута
      }
   }

   return false;
}

void Graph::attr_list(UInt obNodeID, Bool localSearch, ValueArr& attrs)
{
   if (wrong_node(obNodeID)) {
      TRACE_ERROR << "!!! Graph::attr_list> Unknown names of nodes ("
         << obNodeID << ")\n";
      return;
   }

   Node & nx = _nodes[obNodeID];
   Int    attrKnd = ReservedEdgeIDs::EDGE_ATTR;
   Int    isaKnd  = ReservedEdgeIDs::EDGE_ISA;

   queue<ValueGraphPtrNodeID>  open;                        // очередь для дальнейших переходов
   set<Node*>                  close;                       // список исследованных узлов
   set<Node*>                  closeAttrNodes;              // список исследованных атрибутов

   open.push(ValueGraphPtrNodeID(_valGraph.lock(), &nx));   // помещаем стартовый узел

   while (!open.empty())
   {                                              // пока очередь не пуста
      ValueGraphPtrNodeID n = open.front();       // берём первый путь
      open.pop();                                 // и удаляем его
      close.insert(n._node);                      // запоминаем, что в нём были

      vector<Edge> &edgesAttrKnd = n._node->edgesOut(attrKnd)._edges;
      for (auto &edgeAttr : edgesAttrKnd)
      {
         ValueGraphPtr graphN2Ptr = edgeAttr._valGraph2 ? edgeAttr._valGraph2 : n._valGraph;
         Graph *         graphN2 = graphN2Ptr->get_Graph();
         Node &         attrNode = graphN2->_nodes[edgeAttr._n2];
         vector<Edge> &attrIsaEdges = attrNode.edgesOut(isaKnd)._edges;
         for (auto &edgeAttrIsa : attrIsaEdges)
         {
            ValueGraphPtr valGraph2 = edgeAttrIsa._valGraph2 ? edgeAttrIsa._valGraph2 : n._valGraph;
            Node  * nodeN2 = &valGraph2->_val._nodes[edgeAttrIsa._n2];
            if (closeAttrNodes.find(nodeN2) == closeAttrNodes.end())
            {
               closeAttrNodes.insert(nodeN2);
               //attrs.push(make_shared<ValueNode>(valGraph2, edgeAttrIsa._n2));   // этот атрибут еще не добавляли

               if (edgeAttrIsa._valGraph2 == _valGraph.lock())
                  attrs.push(make_shared<ValueNode>(_classID, edgeAttrIsa._n2));
               else
                  attrs.push(make_shared<ValueNode>(edgeAttrIsa._valGraph2, edgeAttrIsa._n2));
            }
         }
      }

      if (localSearch)
         return; // попросили искать только в текущем узле

      //поднимаемся вверх по отношению isa и собираем атрибуты там
      vector<Edge> &edgesIsaKnd = n._node->edgesOut(isaKnd)._edges;
      for (auto &edgeIsa : edgesIsaKnd)
      {
         ValueGraphPtr valGraph2 = edgeIsa._valGraph2 ? edgeIsa._valGraph2 : n._valGraph;
         Node  * nodeN2 = &valGraph2->_val._nodes[edgeIsa._n2];   // узел в который направлен граф
         if (close.find(nodeN2) == close.end())
            open.push(ValueGraphPtrNodeID(valGraph2, nodeN2));           // добавляем узел в кандидаты для поиска атрибута
      }
   }

   return;
}

void Graph::node_list(UInt obNodeID, Int edgeKind, ValueArr &attrs, Logic value)
{
   if (wrong_node(obNodeID)) {
      TRACE_ERROR << "!!! Graph::node_list> Unknown names of nodes ("
         << obNodeID << ")\n";
      return;
   }

   Node & n = _nodes[obNodeID];
   EdgeList edges = n.edgesOut(edgeKind);
   for (auto e : edges._edges)
   {
      if (!value.isUndef() && e._val != value)
         continue;

      if (e._valGraph2)
         attrs.push(make_shared<ValueNode>(e._valGraph2, e._n2));
      else
         attrs.push(make_shared<ValueNode>(_classID, e._n2));
   }
}

Graph * Graph::attrGraph()
{
   if (!_valAttrGraph)
   {
      //пора создать атрибутный граф
      _valAttrGraph = ValueGraph::create();
      _valAttrGraph->_val._graph_name = _graph_name + "_ATTR";
   }
   return &_valAttrGraph->_val;
}

shared_ptr<ValueGraph> Graph::attrGraphValue()
{
   return _valAttrGraph;
}

void Graph::sort()
{
   for (size_t i = 0; i < _nodes.size(); i++)
   {
      _nodes[i].sort();
   }
}

void Graph::addAxiom(ExprPtr ex)
{
   _axioms.push_back(Axiom(_axioms.size()));
   Axiom &axiom = _axioms.back();
   axiom.parse(ex);
}

void Graph::checkAxioms()
{
   CurrentGraph currentGraph;                            // запоминаем текущий граф
   currentGraph.set(Value(_valGraph.lock()));
   for (auto &axiom : _axioms)
      axiom.check();
}

//=======================================================================================
// Добавить новый узел с именем name в граф
//
void Graph::add_node(UInt id)
{
   if (_nodes.size() != id)
      return;

   if (close())
   {
      TRACE_CRITICAL << ":Adding node to clodes graph!\n";
   }

   _nodes.push_back(Node(id));
}

void Graph::setOwner()
{
   shared_ptr<ValueGraph> xGraphVal(_valGraph.lock());

   for (size_t i = 0; i < _nodes.size(); i++)
   {
      _nodes[i].setOwner(xGraphVal);
   }
}

void Graph::updated()
{
   if (_invariants)
   {
      _invariants->outdated = true;
   }
}

Bool Graph::wrong_id(Int k, UInt x, UInt y)
{
   return (wrong_kind(k) || 
           wrong_node(x) ||
           wrong_node(y));
}

Bool Graph::wrong_kind(Int k)
{
   return _manager.wrongEdge(k);
}

Bool Graph::wrong_node(UInt id)
{
   return _manager.wrongNode(_classID,id);
}

void Graph::add(Int knd, UInt xID, UInt yID, Logic val, Graph* yGraph/* = 0*/)
{
   _setEdgeCnt++;

   yGraph = yGraph ? yGraph : this;

   //проверяем корректность типа связи
   if (wrong_kind(knd)) {
      TRACE_ERROR << "!!! Graph::add> Unknown edge " << knd << " in graph X";
      return;
   }

   //проверяем корректность узла x
   if (wrong_node(xID)) {
      TRACE_ERROR << "!!! Graph::add> Unknown x node " << xID << "\n";
      return;
   }

   bool externalLink = false;

   if (yGraph != this)
   {
      externalLink = true;
      //узел y внешний, следовательно во внешнем графе проверяем наличие отношения knd
      if (yGraph->wrong_kind(knd)) {
         TRACE_ERROR << "!!! Graph::add> Unknown edge " << knd << " in graph Y\n";
         return;
      }
   }

   //проверяем корректность узла y
   if (yGraph->wrong_node(yID)) {
      TRACE_ERROR << "!!! Graph::add> Unknown y node " << yID << "\n";
      return;
   }

   shared_ptr<ValueGraph> xGraphVal(        _valGraph.lock());
   shared_ptr<ValueGraph> yGraphVal(yGraph->_valGraph.lock());

   //добавляем ребро в граф
   if (_nodes[xID].add(knd, yID, val, xGraphVal, yGraphVal))
   {
      _changes++;
      updated();
   }
      


   if (externalLink)
      yGraphVal->_val._nodes[yID].add(-knd, xID, val, yGraphVal, xGraphVal);                   // обратное ребро
   else
      _nodes[yID].add(-knd, xID, val, xGraphVal);
}

void Graph::add(const Edge & edge)
{
   add(edge._knd, edge._n1, edge._n2, edge._val, edge._valGraph2 ? &edge._valGraph2->_val : 0);
}

void Graph::setInvariants()
{
   if (!_invariants)
   {
      _invariants = make_shared<GraphInvariants>();
   }

   if (!_invariants->outdated)
   {
      return;
   }

   //число вершин
   _invariants->vertexes = (int)_nodes.size() - 1;

   //число ребер / степени вершин
   int edges = 0;
   vector<int>& vertexesDegree = _invariants->vertexesDegree;
   vector<int>& kindTypes      = _invariants->kindTypes;
   
   vertexesDegree.clear();
   kindTypes.clear();

   for (auto &node : _nodes)
   {
      vector<EdgeList>& edgesOut = node.edgesOut();
      for (auto &edgeList : edgesOut)
      {
         int degree = (int)edgeList._edges.size();
         edges += degree;
         vertexesDegree.push_back(degree);         
         kindTypes.push_back(edgeList._kind);
      }
   }
   
   std::sort(vertexesDegree.begin(), vertexesDegree.end());
   std::sort(kindTypes.begin(), kindTypes.end());

   _invariants->edges = edges;

   _invariants->outdated = false;
}

Bool Graph::isomorphic(Graph* g)
{   
   //для оптимизации сначала проверим инварианты
   this->setInvariants();
   g->setInvariants();

   if (!(*this->_invariants == *g->_invariants))
      return false;

   bool res = false;
   if (this->_invariants->edges < 1000)
   {
      //если ребер в графе мало, быстрее проверить перебором
      res = isomorphicSimple(g);
   }
   else
   {
      //иначе проверяем изоморфизм с помощью алгоритма VF2
      //исходники позаимствованы с ресурса https://github.com/xysmlx/VF2
      res = VF2::instance().isomorphic(this, g);
   }
   
   return res;
}

Bool Graph::isomorphicSimple(Graph* g)
{
   typedef pair<int, int> prii;
   
   int nodes = this->_invariants->vertexes;

   //перестановки
   static vector<vector<vector<int>>> perms;

   //генерируем недостающие перестановки
   while (perms.size() < nodes)
   {
      perms.resize(perms.size() + 1);
      int nodesCur = (int)perms.size();
      vector<vector<int>>& all_perm = perms[nodesCur - 1];
      vector<int> arr_perm;
      int i = 1;
      while (i <= nodesCur) {
         arr_perm.push_back(i);
         i += 1;
      }
      Utils::permutations(arr_perm, all_perm, 0);
   }

   //перестановки для текущего числа вершин
   vector<vector<int>>& permCur = perms[(int)(nodes - 1)];
   int edges = (int)GraphManager::instance().edges() + 1;
   int firstEdge = ReservedEdgeIDs::EDGE_RESERVED_MAX;

   //проверяем ребра графов с учетом перестановок
   for (auto perm : permCur)                                
   {
      UInt x = 0;
      while (x < nodes)                                              // переберем номера всех пар вершин
      {
         UInt y = 0;
         while (y < nodes)
         {
            bool ok   = true;
            for (int e = firstEdge; e < edges; e++)                 // по рёбрам между двух вершин
            {
               if (val(e, x + 1, y + 1, this, this, false) != g->val(e, perm[x], perm[y], g, g, false))
               {
                  ok = false;       
                  break;                                             // ребро у графов не совпало
               }
            }
            if (!ok)
               break;
            y++;
         }
         if (y < nodes)
            break;
         x++;
      }
      if (x >= nodes)
         return true;
   }

   return false;
}

void Graph::clear_edges(Int knd, UInt xID, bool recursion)
{
   EdgeList &list = _nodes[xID].edgesOut(knd);
   EdgeList recursionList;
   if (recursion)
   {
      recursionList = list;
   }

   shared_ptr<ValueGraph> GraphVal(_valGraph.lock());

   // Удаляем обратные ребра
   for(auto e : list._edges)
   { 
      if (e._valGraph2)
         e._valGraph2->_val._nodes[e._n2].add(-knd, e._n1, Logic::Undef, e._valGraph2, GraphVal);
      else
         _nodes[e._n2].add(-knd, e._n1, Logic::Undef, GraphVal);
   }

   // Удаляем прямые ребра
   _nodes[xID].clearEdge(knd);

   // Рекурсия
   for (auto e : recursionList._edges)
   {
      if (e._valGraph2)
         e._valGraph2->_val.clear_edges(knd, e._n2, recursion);
      else
         clear_edges(knd, e._n2, recursion);
   }
}

////=======================================================================================
//// Добавить ребро типа knd между узлами x и y с истинностью val
////
//void Graph::add(Int knd, UInt x, UInt y, Logic val)
//{
//   if ( wrong_id(knd, x, y) ) {
//      TRACE_ERROR << "!!! Graph::add> Unknown names of nodes or edge ("
//           << node_name(x) << "." << edge_name(knd) << "." << node_name(y) << "): "
//           << !(knd <= 0 || knd >= (Int)_edge_names.size()) << ","
//           << !(x == 0 || x >= _node_names.size()) << ","
//           << !(y == 0 || y >= _node_names.size()) << "\n";
//      return;
//   }
//   if( _nodes[x].add( knd, y, val, this) )
//      _changes++;
//   _nodes[y].add(-knd, x, val);                   // обратное ребро
//}
//=======================================================================================
// Удалить все узлы, сохранив справочник типов рёбер
//
void Graph::delete_nodes()
{
   _nodes.clear();
   _changes++;
   updated();
}
//=======================================================================================
// Убрать все рёбра (но не из справочника)
//
void Graph::clear_edges()
{
   for(UInt i=0; i < _nodes.size(); i++)
   {
      Node  & n = _nodes[i];
      n.clear();
   }
}
//=======================================================================================
void Graph::print_edge(EdgeList edgeList, ostream & out, Int show_inverse, Int cnt, Int show_value /*= 1*/)
{
   edgeList.sort(Node::sortTargetName); // Список приехал сюда по значению, можно портить.

   for (UInt i = 0; i < edgeList._edges.size(); i++)
   {
      Edge & e = edgeList._edges[i];
      Graph* graphN1 = this;
      Graph* graphN2 = e._valGraph2 ? &e._valGraph2->_val : this;
      string name = (graphN1 != graphN2) ? (graphN2->_graph_name + ".") : "";
      name += graphN2->node_name(e._n2);
      if (show_value == 1)
      {
         shared_ptr<ValueBase> nodeValue = graphN2->nodeValue(e._n2);
         if (nodeValue)
         {
            name += "(" + nodeValue->toString() + ")";
         }
      }
      if (e._val._p0 == 0 && e._val._p1 == 1)
         out << name;
      else if (e._val._p0 == 1 && e._val._p1 == 0)
         out << "!" << name;
      else
         out << name << e._val;
      if (!show_inverse && cnt == 1)
         out << (i + 1 == edgeList._edges.size() ? "]" : ",");
      else
         out << (i + 1 == edgeList._edges.size() ? "],\n" : ",");
   }
   if (!edgeList._edges.size())
   {
      if (!show_inverse && cnt == 1) out << " ]";  // одно ребро         
      else                           out << " ],\n";  // много рёбер
   }
}
//=======================================================================================
// Вывести объект c идентификатором id в поток out
//
void Graph::print_obj(UInt id, ostream & out, Int show_inverse, Int show_value /*= 1*/)
{
   Node & n = _nodes[id];
   n.sort();
   out << "   " << node_name(id);
   //map<Int, vector<Edge> >::iterator it = n._edges.begin();
   UInt cnt = n.edgesOut().size();
   //for (; it != n._edges.end(); it++) 
   //   if(it->first > 0)    
   //      cnt++;

   if (!show_inverse && cnt == 0) {               // нет прямых рёбер
      out << " {}";
      return;
   }   
   if (!show_inverse && cnt == 1) out << " { ";  // одно ребро         
   else                           out << "{\n";  // много рёбер

   if (show_value == 1 && n.value())
   {
      out << ((!show_inverse && cnt == 1) ? " " : "      ") << "__value__: " << n.value()->toString() << ",\n";
   }

   for (size_t i = 0; i < n.edgesOut().size(); i++)
   {
      out << ((!show_inverse && cnt == 1) ? " " : "      ") << edge_name(n.edgesOut()[i]._kind) << ": [";
      print_edge(n.edgesOut()[i], out, show_inverse, (Int)cnt, show_value);
   }

   if (show_inverse)
   {
      for (size_t i = 0; i < n.edgesIn().size(); i++)
      {
         out << "      i_" << edge_name(n.edgesIn()[i]._kind) << ": [";
         print_edge(n.edgesIn()[i], out, show_inverse, (Int)cnt, show_value);
      }
   }

   out << "   }";   
}
//=======================================================================================
// Вывести модель в поток out
//
ostream &  Graph::print(ostream & out, Int show_inverse, Int show_value /*= 1*/)
{
   out << _graph_name << " {\n";
   //выводим узлы в отсортированном виде
   auto &node_names = _manager.nodeNames(_classID);
   bool first = true;
   for (auto &node : node_names)
   {
      if (node.second == 0)
         continue;
      if (!first)
      {
         out << ", \n";         
      }
      print_obj(node.second, out, show_inverse, show_value);
      first = false;
   }   
   out << "\n}\n";
   return out;
}
//=======================================================================================
//
string del_prefix(const string &name)
{
   if(!name.size()) return "";

   if (name[0] == '$' || name[0] == '@')
   {
      if (name[1] == '$' || name[1] == '@')
         return name.substr(2);
      else
         return name.substr(1);
   }
   return name;
}
//=======================================================================================
// Вывести граф в поток out в dot формате
//
ostream & Graph::print_dot(ostream & out, vector<Int> *edgesFilter/* = NULL*/, bool showLoop/* = true*/, int mode/* = 0*/)
{
   //http://www.graphviz.org/doc/info/attrs.html#k:style

   static vector<string> styles = { "solid", "dashed", "dotted", "bold" };

   //определяем, есть ли внешние связи с другими графами
   //и в таком случае рисуем несколько графов через кластера

   enum DotMode {
      DOT_MODE_EXTENDED = 0,  //!< развернутый - атрибуты узлов показываются как есть в графе(Masha - attr-> 20 - isa->Senses = >age)
      DOT_MODE_SHORT    = 1   //!< свернутый   - атрибуты узлов показываются в компактном виде (Masha -attr-> age:20)
   };

   struct GraphNode {
      GraphNode(Graph* graph, UInt nodeID) :_graph(graph), _nodeID(nodeID) {}
      Graph* _graph;
      UInt   _nodeID;
   };

   struct DotGraphNode {
      DotGraphNode() {}
      DotGraphNode(UInt id, string text) :_id(id), _text(text) {}
      UInt   _id;
      string _text;
   };

   struct DotGraph {
      DotGraph(){}
      DotGraph(const string & text):_text(text) {}
      string                   _text;
      map<UInt, DotGraphNode>  _nodeIDtoDotNode;
      vector<string>           _edges;
   };

   UInt maxDotGraphNodeID = 0;
   map<Graph*, DotGraph> graphToDotGraph;
   vector<string>        externalEdges;
   stringstream graphText;
   graphText << "label = \"" << name() << "\";";
   graphToDotGraph[this] = DotGraph(graphText.str());
   DotGraph &curDotGraph = graphToDotGraph[this];

   for (UInt i = 1; i < _nodes.size(); i++)
   {
      Node & n = _nodes[i];
      stringstream nodeText;
      nodeText << maxDotGraphNodeID << " [label=\"";
      if (n.value())
      {
         nodeText << n.value()->toString();
      }      
      else
      {
         nodeText << del_prefix(node_name(i));
      }
      nodeText << "\"];";
      curDotGraph._nodeIDtoDotNode[i] = DotGraphNode(maxDotGraphNodeID++, nodeText.str());
   }

   for (UInt i = 1; i < _nodes.size(); i++)
   {
      Node & n = _nodes[i];
      bool   isAttrAndShortMode = false;
      if (mode == DOT_MODE_SHORT && (n.edgesIn(ReservedEdgeIDs::EDGE_ATTR)._edges.size() > 0))
      {
         //этот узел содержит атрибут и в данном режиме исходящие из него связи не отображаем
         isAttrAndShortMode = true;
      }

      for (size_t i = 0; i < n.edgesOut().size(); i++)
      {
         UInt styleIndex = 0;

         if (edgesFilter && (*edgesFilter).size() > 0)
         {
            bool skipEdge = true;
            UInt edgeIndex = 0;
            for (auto edge : *edgesFilter)
            {
               if (edge == n.edgesOut()[i]._kind)
               {
                  skipEdge = false;
                  break;
               }
               edgeIndex++;
            }
            if (skipEdge)
               continue;

            styleIndex = edgeIndex % styles.size();
         }

         vector<Edge> &edges = n.edgesOut()[i]._edges;
         for (auto &edge : edges)
         {
            if (!showLoop && edge._n1 == edge._n2)
               continue;
            Graph *n2Graph    = edge._valGraph2 ? &edge._valGraph2->_val : this;
            string node2Name = n2Graph->node_name(edge._n2);
            if (isAttrAndShortMode)
            {               
               DotGraph     &dotGraph     = graphToDotGraph[this];
               DotGraphNode &dotGraphNode = dotGraph._nodeIDtoDotNode[i];
               stringstream nodeText;
               nodeText << dotGraphNode._id << " [label=\"" << node2Name;
               shared_ptr<ValueBase> nodeVal = nodeValue(i);
               if (nodeVal)
                  nodeText << ": " << nodeVal->toString() << "\"];";
               dotGraphNode._text = nodeText.str();
               break;
            }
            else
            {   
               Graph *n2Graph = edge._valGraph2 ? &edge._valGraph2->_val : this;
               bool externalEdge = false;
               if (n2Graph != this)
               {
                  //внешняя ссылка, проверяем есть ли уже такой граф и если нет, добавляем
                  externalEdge = true;
                  if (graphToDotGraph.find(n2Graph) == graphToDotGraph.end())
                  {
                     stringstream graphText;
                     graphText << "label = \"" << n2Graph->name() << "\";";
                     graphToDotGraph[n2Graph] = DotGraph(graphText.str());
                  }

                  DotGraph &dotGraphN2 = graphToDotGraph[n2Graph];
                  //проверяем есть ли уже узел, и если нет добавляем
                  if (dotGraphN2._nodeIDtoDotNode.find(edge._n2) == dotGraphN2._nodeIDtoDotNode.end())
                  {
                     stringstream nodeText;
                     nodeText << maxDotGraphNodeID << " [label=\"" << n2Graph->node_name(edge._n2) << "\"];";
                     dotGraphN2._nodeIDtoDotNode[edge._n2] = DotGraphNode(maxDotGraphNodeID++, nodeText.str());
                  }
               }

               DotGraph     &dotGraph1     = graphToDotGraph[this];
               DotGraphNode &dotGraphNode1 = dotGraph1._nodeIDtoDotNode[edge._n1];
               DotGraph     &dotGraph2     = graphToDotGraph[n2Graph];
               DotGraphNode &dotGraphNode2 = dotGraph2._nodeIDtoDotNode[edge._n2];

               //проверяем есть ли входящий узел с таким же значением из такого же ребра
               bool twoSide = false;
               auto edgesIn = n.edgesIn(n.edgesOut()[i]._kind)._edges;
               for (auto &edgesInIt : edgesIn)
               {
                  if (edgesInIt._n2 == edge._n2 && edgesInIt._val == edge._val)
                  {
                     twoSide = true;
                     break;
                  }
                  if (twoSide)
                     break;
               }

               if (twoSide && edge._n1 > edge._n2)
               {
                  //для двухсторонних связей, добавляем только одну
                  continue;
               }                  

               //добавляем связь
               stringstream edgeText;
               edgeText << dotGraphNode1._id << "->" << dotGraphNode2._id << " [";
               if (twoSide)
               {
                  edgeText << "dir=\"none\",";
               }
               //устанавливаем стиль линии
               edgeText << "style=\"" << styles[styleIndex] << "\",";
               //устанавливаем надпись
               edgeText << "label=\"";
               string edgeName = del_prefix(edge_name(edge._knd));
               if (edge._val == Logic::True)
                  edgeText << edgeName << "\"];";
               else if (edge._val == Logic::False)
                  edgeText << "!" << edgeName << "\", color=red];";
               else
                  edgeText << edgeName << "(" << edge._val.to_str() << ")\", style=dashed];";
                  
               if (externalEdge)
                  externalEdges.push_back(edgeText.str());
               else
                  dotGraph2._edges.push_back(edgeText.str());
            }
         }
      }
   }

   out << "digraph G{\n";

   if (graphToDotGraph.size() == 1)
   {
      //используется монографный режим
      //выводим все узлы
      for (auto &dotGraph : graphToDotGraph)
      {
         for (auto &dotGraphNode : dotGraph.second._nodeIDtoDotNode)
         {
            out << "   " << dotGraphNode.second._text << "\n";
         }
      }
      //выводим все связи
      for (auto &dotGraph : graphToDotGraph)
      {
         for (auto &dotGraphEdge : dotGraph.second._edges)
         {
            out << "   " << dotGraphEdge << "\n";
         }
      }
      for (auto &dotGraphEdge : externalEdges)
      {
         out << "   " << dotGraphEdge << "\n";
      }
   }
   else
   {
      UInt clasterID = 0;
      for (auto &dotGraph : graphToDotGraph)
      {
         out << "   subgraph cluster_" << clasterID << " {\n";
         out << "      " << dotGraph.second._text << "\n";
         if (dotGraph.first == this)
         {
            //локальный граф
            out << "      node [style = filled];\n";
         }
         else
         {
            //внешний граф
            out << "      style = filled; color = lightgrey;\n";
            out << "      node [style = filled; color = white];\n";
         }
         //выводим внутренние связи графа
         for (auto &dotGraphEdge : dotGraph.second._edges)
         {
            out << "      " << dotGraphEdge << "\n";
         }
         //выводим узлы графа
         for (auto &dotGraphNode : dotGraph.second._nodeIDtoDotNode)
         {
            out << "      " << dotGraphNode.second._text << "\n";
         }
         out << "   }\n";
         clasterID++;
      }
      //выводим внешние связи
      for (auto &dotGraphEdge : externalEdges)
      {
         out << "   " << dotGraphEdge << "\n";
      }
   }

   out << "}\n";
   return out;
}
//=======================================================================================
// Записать в txt граф в виде текста
//
void Graph::get_str(string & txt)
{
   txt = "";
   for (UInt i = 1; i < _nodes.size(); i++) {
      Node & n = _nodes[i];
      for (size_t k = 0; k < n.edgesOut().size(); k++)
      { 
         vector<Edge> & edges = n.edgesOut()[k]._edges;
         for(UInt j = 0; j < edges.size(); j++){
            Edge e  = edges[j];
            if(e._val.isTrue())
               txt += node_name(e._n1) + " " + edge_name(e._knd) + " " + node_name(e._n2) + " . ";
            else if (e._val.isFalse())
               txt += node_name(e._n1) + "  not " + edge_name(e._knd) + " " + node_name(e._n2) + " . ";
            else 
               txt += node_name(e._n1) + " " + edge_name(e._knd) + e._val.to_str() + " " + node_name(e._n2) + " . ";
         }
      }   
   }
      
}
//=======================================================================================
// Получить значение ребра knd(x,y) между узлами x и y
//
Logic Graph::val(Int knd, UInt x, UInt y, Graph* xGraph /*= NULL*/, Graph* yGraph /*= NULL*/, bool safe /*= true*/)
{
   _getEdgeCnt++;

   xGraph = xGraph ? xGraph : this;
   yGraph = yGraph ? yGraph : this;

   if (safe)
   {
      if (xGraph->wrong_kind(knd))
      {
         TRACE_ERROR << "!!! Graph::val> Unknown name of edge " << knd << " ("
            << x << " " << knd << " " << y << ") in graph " << xGraph->name() << "\n";
         return Logic();
      }
      if (xGraph->wrong_node(x))
      {
         TRACE_ERROR << "!!! Graph::val> Unknown name of node " << x << " ("
            << x << " " << knd << " " << y << ") in graph " << xGraph->name() << "\n";
         return Logic();
      }
      if (yGraph->wrong_node(y))
      {
         TRACE_ERROR << "!!! Graph::val> Unknown name of node " << y << " ("
            << x << " " << knd << " " << y << ") in graph " << yGraph->name() << "\n";
         return Logic();
      }
   }
   Node & nx = _nodes[x];
   EdgeList &lst = knd >= 0 ? nx.edgesOut(knd) : nx.edgesIn(-knd);
   vector<Edge> &nodes = lst._edges;
   if (lst._sort == Node::sortTargetNode && nodes.size() > _lengthForBinary)
   {
      // Бинарный поиск
      size_t left = 0;
      size_t right = nodes.size();
      size_t center = (left + right) / 2;

      while (left != right)
      {
         center = (left + right) / 2;
         if (nodes[center]._n2 == y && (xGraph == yGraph || (nodes[center]._valGraph2 && nodes[center]._valGraph2->get_Graph() == yGraph)))
            break;
         else if (nodes[center]._n2 < y && left != center)
            left = center;
         else
            right = center;
      }

      if (nodes[center]._n2 == y && (xGraph == yGraph || (nodes[center]._valGraph2 && nodes[center]._valGraph2->get_Graph() == yGraph)))
      {
         return nodes[center]._val;
      }
   }
   else
   {
      // Линейный поиск
      if (xGraph == yGraph)
      {
         //внутренний поиск
         for (UInt i = 0; i < nodes.size(); i++) {
            if (nodes[i]._n2 == y)
               return nodes[i]._val;                    // нашли y
         }
      }
      else
      {
         //поиск внешних связей
         for (UInt i = 0; i < nodes.size(); i++) {
            if (nodes[i]._n2 == y && nodes[i]._valGraph2 && nodes[i]._valGraph2->get_Graph() == yGraph)
               return nodes[i]._val;                    // нашли y
         }
      }
   }
   return Logic();                                // неопределённое значение
}
//=======================================================================================
// Есть ли ребро типа kind (с любым значением) от узла x в узел y
//
Logic Graph::is_edge(Int knd, UInt x, UInt y, Graph* xGraph/* = NULL*/, Graph* yGraph/* = NULL*/)
{
   if (xGraph->wrong_kind(knd))
   {
      TRACE_ERROR << "!!! Graph::is_edge> Unknown name of edge " << knd << " ("
         << x << " " << knd << " " << y << ") in graph " << xGraph->name() << "\n";
      return Logic();
   }
   if (xGraph->wrong_node(x))
   {
      TRACE_ERROR << "!!! Graph::is_edge> Unknown name of node " << x << " ("
         << x << " " << knd << " " << y << ") in graph " << xGraph->name() << "\n";
      return Logic();
   }
   if (yGraph->wrong_node(y))
   {
      TRACE_ERROR << "!!! Graph::is_edge> Unknown name of node " << y << " ("
         << x << " " << knd << " " << y << ") in graph " << yGraph->name() << "\n";
      return Logic();
   }
   Node & nx = _nodes[x];
   EdgeList &lst = knd >= 0 ? nx.edgesOut(knd) : nx.edgesIn(-knd);
   vector<Edge> &nodes = lst._edges;

   if (lst._sort == Node::sortTargetNode && nodes.size() > _lengthForBinary)
   {
      // Бинарный поиск
      size_t left = 0;
      size_t right = nodes.size();
      size_t center = (left + right) / 2;

      while (left != right)
      {
         center = (left + right) / 2;
         if (nodes[center]._n2 == y && (xGraph == yGraph || (nodes[center]._valGraph2 && nodes[center]._valGraph2->get_Graph() == yGraph)))
            break;
         else if (nodes[center]._n2 < y && left != center)
            left = center;
         else
            right = center;
      }

      if (nodes[center]._n2 == y && (xGraph == yGraph || (nodes[center]._valGraph2 && nodes[center]._valGraph2->get_Graph() == yGraph)))
      {
         return Logic(0, 1);
      }
   }
   else
   {
      // Линейный поиск
      if (xGraph == yGraph)
      {
         for (UInt i = 0; i < nodes.size(); i++) {
            if (nodes[i]._n2 == y)
               return Logic(0, 1);                    // нашли y
         }
      }
      else
      {
         for (UInt i = 0; i < nodes.size(); i++) {
            if (nodes[i]._n2 == y && nodes[i]._valGraph2 && nodes[i]._valGraph2->get_Graph() == yGraph)
               return Logic(0, 1);                    // нашли y
         }
      }
   }
   return Logic::False;                          // не нашли y
}
//=======================================================================================
// Получить список рёбер objs типа knd, выходящих из узла x 
//
void Graph::get_out(const string & knd, Node & x,  vector<Edge> &objs)
{
   Int r = edge(knd);
   objs = x.edgesOut(r)._edges;                   // копируем значения! Если нет отношения - будет пустой вектор
}
//=======================================================================================
// Получить список рёбер objs типа knd, входящих в узел x 
//
void Graph::get_in(const string & knd, Node & x,  vector<Edge> &objs)
{
   Int r = edge(knd);
   objs = x.edgesIn(r)._edges;                   // копируем значения! Если нет отношения - будет пустой вектор
}

void Graph::get_out(vector<Edge>& objs)
{
   for (auto &node : _nodes)
   {
      for (auto &edgeList : node.edgesOut())
      {
         for (auto &edge : edgeList._edges)
         {
            objs.push_back(edge);
         }
      }
   }
}

void Graph::get_in(vector<Edge>& objs)
{
   for (auto &node : _nodes)
   {
      for (auto &edgeList : node.edgesIn())
      {
         for (auto &edge : edgeList._edges)
         {
            objs.push_back(edge);
         }
      }
   }
}

//=======================================================================================
// Есть ли путь от x к y по рёбрам типа knd
//
Logic Graph::path(vector<Int> & knds, Node  & nx, Node & ny, bool loop, bool back)
{
   priority_queue<GraphNode> open;                // очередь для дальнейших переходов
   map<Node*, Logic>         close;               // список исследованных узлов
   map<Logic, int>           best;                // истинности найденных путей

   open.push(GraphNode(this, &nx, 0, Logic::True));// помещаем стартовый узел

   while (!open.empty()) {                        // пока очередь не пуста
      GraphNode n = open.top();                   // берём самый истинный путь      
      open.pop();                                 // и удаляем его
      close[n._node] = n._path;                   // запоминаем, что в нём были

      for (auto knd : knds)
      {
         EdgeList & it = n._node->edgesOut(knd);
         if (it._edges.size() == 0)
            continue;                                // из nz ребро knd не выходит   

         for (auto &edge: it._edges)                     // по всем выходящим рёбрам knd:
         {
            Graph * graphN2 = edge._valGraph2 ? edge._valGraph2->get_Graph() : n._graph;// граф в который направлено ребро
            Node  * nodeN2 = &graphN2->_nodes[edge._n2];

            if (!loop && nodeN2 == n._node)          // игнорируем петли
               continue;

            if (!back && nodeN2 == n._from)          // игнорируем обратные рёбра
               continue;

            //вычисляем логическое И вдоль пути:
            GraphNode ni(graphN2, nodeN2, n._node, n._path & edge._val, n._len + 1);
            //cout << ni._node->id() << ":" << ni._path << endl;

            if (ni._node == &ny) {                   // нашли целевой узел ny
                                                     //cout <<  "ni._node == &ny"  << ni._node->id() << ":" << ni._path << endl;
               if (ni._path == Logic::True)          // и путь к нему истинный
                  return Logic::True;
               best[ni._path] = 1;                   // запоминаем логическое значение
            }

            map<Node*, Logic>::iterator it_w = close.find(ni._node);
            if (it_w == close.end())                 // ещё в нём не были            
               open.push(ni);                        // будем дальше исследовать
            else if (it_w->second < ni._path) {      // были и он короче (истиннее) 
               open.push(ni);                        // надо повторно пройти
               it_w->second = ni._path;

            }
         }
      }
   }

   if (best.empty())
      return Logic::Undef;                        // пути нет

   Logic res = Logic::False;                      // ищем самый "истинный"
   map<Logic, int>::iterator it = best.begin();
   for (; it != best.end(); it++)
      if (res < it->first)
         res = it->first;
   return res;
}

//=======================================================================================
//
Logic Graph::path(Int knd, Node & nx, Node & ny, bool loop, bool back)
{
   priority_queue<GraphNode> open;                // очередь для дальнейших переходов
   map<Node*, Logic>         close;               // список исследованных узлов
   map<Logic, int>           best;                // истинности найденных путей

   open.push(GraphNode(this, &nx, 0, Logic::True));// помещаем стартовый узел

   while (!open.empty()) {                        // пока очередь не пуста
      GraphNode n = open.top();                   // берём самый истинный путь      
      open.pop();                                 // и удаляем его
      close[n._node] = n._path;                   // запоминаем, что в нём были

      EdgeList & it = n._node->edgesOut(knd);
      if (it._edges.size() == 0)
         continue;                                // из nz ребро knd не выходит   

      for (UInt i = 0; i < it._edges.size(); i++)// по всем выходящим рёбрам knd:
      {
         ValueGraphPtr graphN2Ptr = it._edges[i]._valGraph2;
         Graph * graphN2 = graphN2Ptr ? graphN2Ptr->get_Graph() : n._graph;
         Node  * nodeN2  = &graphN2->_nodes[it._edges[i]._n2];

         if (!loop && nodeN2 == n._node)          // игнорируем петли
            continue;

         if (!back && nodeN2 == n._from)          // игнорируем обратные рёбра
            continue;

         //вычисляем логическое И вдоль пути:
         GraphNode ni(graphN2, nodeN2, n._node, n._path & it._edges[i]._val, n._len + 1);
         //cout << ni._node->id() << ":" << ni._path << endl;

         if (ni._node == &ny) {                   // нашли целевой узел ny
                                                  //cout <<  "ni._node == &ny"  << ni._node->id() << ":" << ni._path << endl;
            if (ni._path == Logic::True)          // и путь к нему истинный
               return Logic::True;
            best[ni._path] = 1;                   // запоминаем логическое значение
         }

         map<Node*, Logic>::iterator it_w = close.find(ni._node);
         if (it_w == close.end())                 // ещё в нём не были            
            open.push(ni);                        // будем дальше исследовать
         else if (it_w->second < ni._path) {      // были и он короче (истиннее) 
            open.push(ni);                        // надо повторно пройти
            it_w->second = ni._path;

         }
      }
   }

   if (best.empty())
      return Logic::Undef;                        // пути нет

   Logic res = Logic::False;                      // ищем самый "истинный"
   map<Logic, int>::iterator it = best.begin();
   for (; it != best.end(); it++)
      if (res < it->first)
         res = it->first;
   return res;
}
//=======================================================================================
Logic Graph::get(Int knd, Node & nx, Node & ny, bool loop /*= true*/, bool back /*= true*/)
{
   queue<GraphNode> open;                         // очередь для дальнейших переходов
   map<Node*, Logic>         close;               // список исследованных узлов

   open.push(GraphNode(this, &nx, 0, Logic::True));// помещаем стартовый узел

   while (!open.empty()) 
   {                                              // пока очередь не пуста
      GraphNode n = open.front();                 // берём первый путь
      open.pop();                                 // и удаляем его
      close[n._node] = Logic::True;               // запоминаем, что в нём были

      EdgeList & it2 = n._node->edgesOut(knd);
      for (auto &edge2 : it2._edges)
      {
         Graph * graphN2 = edge2._valGraph2 ? edge2._valGraph2->get_Graph() : n._graph;// граф в который направлено ребро
         Node  * nodeN2 = &graphN2->_nodes[edge2._n2];
         if (nodeN2 == &ny)
         {
            if (edge2._val == Logic::True)
               return Logic::True;                         // Интересующее нас ребро есть и оно true
            else if (edge2._val == Logic::False)
               return Logic::False;                        // Интересующее нас ребро есть и оно false
         }
         // Иначе - продолжаем
      }

      EdgeList & it1 = n._node->edgesOut(ReservedEdgeIDs::EDGE_ISA);
      if (it1._edges.size() == 0)
         continue;                                // из nz ребро isa не выходит   

      for (auto &edge1 : it1._edges)               // по всем выходящим рёбрам isa:
      {
         if (edge1._val != Logic::True)
            continue;                             // Нас интересуют только истинные ребра isa

         Graph * graphN2 = edge1._valGraph2 ? edge1._valGraph2->get_Graph() : n._graph;// граф в который направлено ребро
         Node  * nodeN2  = &graphN2->_nodes[edge1._n2];

         if (!loop && nodeN2 == n._node)          // игнорируем петли
            continue;

         if (!back && nodeN2 == n._from)          // игнорируем обратные рёбра
            continue;

         //вычисляем логическое И вдоль пути:
         GraphNode ni(graphN2, nodeN2, n._node, n._path & edge1._val, n._len + 1);
         //cout << ni._node->id() << ":" << ni._path << endl;

         map<Node*, Logic>::iterator it_w = close.find(ni._node);
         if (it_w == close.end())                 // ещё в нём не были            
            open.push(ni);                        // будем дальше исследовать
      }
   }

   return Logic::False;
}
//=======================================================================================
void Graph::getAll(Int knd, Node &nx, shared_ptr<ValueArr> out, bool loop /*= true*/, bool back /*= true*/)
{
   queue<ValueGraphPtrNodeID> open;               // очередь для дальнейших переходов
   set<Node*>                 close;              // список исследованных узлов
   set<Node*>            prohibited;              // запретные узлы - встретили явный false в них

   open.push(ValueGraphPtrNodeID(_valGraph.lock(), &nx));   // помещаем стартовый узел

   while (!open.empty())
   {                                              // пока очередь не пуста
      ValueGraphPtrNodeID n = open.front();       // берём первый путь
      open.pop();                                 // и удаляем его
      close.insert(n._node);                      // запоминаем, что в нём были

      EdgeList & it2 = n._node->edgesOut(knd);
      for (auto &edge2 : it2._edges)
      {
         ValueGraphPtr graphTempPtr = edge2._valGraph2 ? edge2._valGraph2 : n._valGraph;

         if (edge2._val == Logic::True && prohibited.find(&graphTempPtr->_val._nodes[edge2._n2]) == prohibited.end())
         {
            shared_ptr<ValueNode> temp = make_shared<ValueNode>(graphTempPtr, edge2._n2);
            if (out->find(temp) < 0)
               out->push(temp);
         }
         else if (edge2._val == Logic::False)
         {
            // Наткнулись на запрет, дальше не ищем в эту сторону 
            prohibited.insert(&graphTempPtr->_val._nodes[edge2._n2]);                      // запоминаем, что в нём были
         }
      }

      EdgeList & it1 = n._node->edgesOut(ReservedEdgeIDs::EDGE_ISA);
      if (it1._edges.size() == 0)
         continue;                                // из nz ребро isa не выходит   

      for (auto &edge1 : it1._edges)              // по всем выходящим рёбрам isa:
      {
         if (edge1._val != Logic::True)
            continue;                             // Нас интересуют только истинные ребра isa

         ValueGraphPtr graphN2Ptr = (edge1._valGraph2 ? edge1._valGraph2 : n._valGraph);// граф в который направлено ребро
         
         Node  * nodeN2 = &graphN2Ptr->_val._nodes[edge1._n2];

         if (!loop && nodeN2 == n._node)          // игнорируем петли
            continue;

         if (close.find(nodeN2) == close.end()) // ещё в нём не были            
            open.push(ValueGraphPtrNodeID(graphN2Ptr, nodeN2));                  // будем дальше исследовать
      }
   }
}
//=======================================================================================
Logic Graph::isa(Node & nx, Node & ny)
{
   return get(ReservedEdgeIDs::EDGE_ISA, nx, ny);
}
//=======================================================================================
// Есть ли общий узел при движении по рёбрам knd от узлов x,y
//
Logic Graph::common(vector<Int> & knds, Node &nx, Node &ny)
{
   queue<UInt>    open1, open2;                   // очереди для дальнейших переходов
   map<UInt, UInt> close1, close2;                  // список исследованных узлов

   open1.push(nx.id());                           // помещаем стартовый узел
   open2.push(ny.id());

   while (!open1.empty() || !open2.empty())
   {
      if (!open1.empty()) {
         UInt n1 = open1.front();  open1.pop();   close1[n1] = 1;
         if (close2.find(n1) != close2.end())
            return Logic(0, 1);                   // нашли          

         Node & nz = get(n1);
         for (UInt i = 0; i < knds.size(); i++) 
         {
            EdgeList & e = nz.edgesOut(knds[i]);
            for (UInt i = 0; i < e._edges.size(); i++)
               if (close1.find(e._edges[i]._n2) == close1.end()// ещё в нём не были
                  && e._edges[i]._val.p1() > 0) // есть не нулевая вероятность
                  open1.push(e._edges[i]._n2);
         }
      }

      if (!open2.empty()) {
         UInt n2 = open2.front();  open2.pop();   close2[n2] = 1;
         if (close1.find(n2) != close1.end())
            return Logic(0, 1);                   // нашли          

         Node z = get(n2);
         for (UInt i = 0; i < knds.size(); i++) 
         {
            EdgeList & e = z.edgesOut(knds[i]);
            for (UInt i = 0; i < e._edges.size(); i++)
               if (close2.find(e._edges[i]._n2) == close2.end()// ещё в нём не были
                  && e._edges[i]._val.p1() > 0) // есть не нулевая вероятность
                  open2.push(e._edges[i]._n2);
         }
      }
   }
   return Logic(1, 0);           // общего узла нет
}

UInt Graph::count(UInt x, Int knd)
{
   if (wrong_node(x))
   {
      TRACE_CRITICAL << "wrong node id" << endl;
      return 0;
   }

   if (wrong_kind(knd<0 ? -knd : knd))
   {
      TRACE_CRITICAL << "wrong node id" << endl;
      return 0;
   }

   Node & node = get(x);

   EdgeList & e = (knd >= 0) ? node.edgesOut(knd) : node.edgesIn(-knd);

   return e._edges.size();
}

UInt Graph::count(UInt x, Int knd, Logic val)
{
   if (wrong_node(x))
   {
      TRACE_CRITICAL << "wrong node id" << endl;
      return 0;
   }

   if (wrong_kind(knd<0?-knd:knd))
   {
      TRACE_CRITICAL << "wrong node id" << endl;
      return 0;
   }

   Node & node = get(x);

   EdgeList & e = (knd >= 0) ? node.edgesOut(knd) : node.edgesIn(-knd);

   if (e._edges.size() == 0)
      return 0;

   UInt cnt = 0;
   for (UInt i = 0; i < e._edges.size(); i++)
   {
      if (e._edges[i]._val == val)
         cnt++;
   }
   return cnt;
}

Logic Graph::load(string & fileName, Int mode/* = 0*/)
{
   string fileExt = Utils::fileExt(fileName);
   if (fileExt == "dic")
      return loadFromSenses(fileName, mode);

   return Logic::False;
}

Logic Graph::loadFromSenses(string & fileName, Int mode/* = 0*/)
{
   enum LoadMode {
      LOAD_MODE_NODES_AND_EDGES_ONLY = 0,    //грузим только узлы и ребра
      LOAD_MODE_NODES_VALUES_EDGES   = 1,    //грузим узлы, их значения и ребра 
   };

   Senses sensesModule;
   if (!sensesModule.load(fileName.c_str()))
   {
      TRACE_ERROR << "can't open file " << fileName << endl;
      return Logic::False;
   }

   //TODO нужно ли? clear();      //очищаем текущий граф 

   vector<Sense> &senses = sensesModule._senses;

   vector<UInt>  correspond;      // массив соответствия идентификаторов смыслов из Sense, идентификаторам текущего графа
   correspond.resize(sensesModule._senses.size());
   GraphManager &manager = _manager;

   for (size_t i = 0; i<senses.size(); i++)
   {
      correspond[i] = manager.addNode(_classID, "$" +  senses[i]._name);
   }

   for (size_t xID = 0; xID<senses.size(); xID++)
   {
      Sense &sense = senses[xID];

      //добавляем ребра
      for (int rID = 0; rID < ReservedEdgeIDs::EDGE_RESERVED_MAX; rID++)
      {
         for (auto yID : sense._relations[(ReservedEdgeIDs)rID])
         {
            add(rID, correspond[xID], correspond[yID._senseId], yID._val);
         }
      }
      Node &node = _nodes[correspond[xID]];

      //добавляем атрибриты
      for (auto & attr : sense._attrs)
      {
         if (attr._type == AttributeInSense::attributeFloat)
         {
            GraphManager::instance().setAttr(_classID, correspond[xID], this, correspond[attr._id], this, ValueManager::createValue(Float(attr._valueFloat)));
         }
         else if (attr._type == AttributeInSense::attributeArraySenses)
         {
            shared_ptr<ValueArr> attrArr = make_shared<ValueArr>();
            for (auto & attrArrItem : attr._valuesUInt)
            {
               attrArr->_val.push_back(ValueManager::createValue(ValueNode(_valGraph.lock(), correspond[attrArrItem])));
            }
            GraphManager::instance().setAttr(_classID, correspond[xID], this, correspond[attr._id], this, attrArr);
         }
         else if (attr._type == AttributeInSense::attributeArrayFloat)
         {
            shared_ptr<ValueArr> attrArr = make_shared<ValueArr>();
            for (auto & attrArrItem : attr._valuesFloat)
            {
               attrArr->_val.push_back(ValueManager::createValue(Float(attrArrItem)));
            }
            GraphManager::instance().setAttr(_classID, correspond[xID], this, correspond[attr._id], this, attrArr);
         }
         else if (attr._type == AttributeInSense::attributeLogic)
         {
            GraphManager::instance().setAttr(_classID, correspond[xID], this, correspond[attr._id], this, ValueManager::createValue(attr._valueLogic));
         }
         else if (attr._type == AttributeInSense::attributeFusNumber)
         {
            GraphManager::instance().setAttr(_classID, correspond[xID], this, correspond[attr._id], this, ValueManager::createValue(attr._valueFusNumber));
         }
      }
      //добавляем значения узлов, если нужно
      if (mode == LOAD_MODE_NODES_VALUES_EDGES)
      {      
         shared_ptr<ValueMap> map = make_shared<ValueMap>();
         addSenseFieldToMap(map, "name",   sense._name);
         addSenseFieldToMap(map, "en",     sense._en);
         addSenseFieldToMap(map, "ru",     sense._ru);
         addSenseFieldToMap(map, "def_en", sense._def_en);
         addSenseFieldToMap(map, "def_ru", sense._def_ru);
         node.value(map);
      }
   }

   return Logic::True;
}

void Graph::addSenseFieldToMap(shared_ptr<ValueMap>& map, const char * name, const string & valStr)
{
   ValueMapKey key(make_shared<ValueStr>(name));
   map->_val[key] = make_shared<ValueStr>(valStr);
}

void Graph::addSenseFieldToMap(shared_ptr<ValueMap>& map, const char * name, const vector<string>& valStrArr)
{
   if (valStrArr.size() == 0 || valStrArr[0].size() == 0)
      return;

   ValueMapKey key(make_shared<ValueStr>(name));

   shared_ptr<ValueArr> mapValArr = make_shared<ValueArr>();
   for (auto &txt : valStrArr)
   {
      mapValArr->push(make_shared<ValueStr>(txt));
   }
   map->_val[key] = mapValArr;
}

//=======================================================================================

shared_ptr<ValueGraph> Graph::operator-(const Graph & y)
{  
   ValueGraphPtr valGraph = ValueGraph::create();

   if (classID() != y.classID())
      return valGraph;

   Graph &res = valGraph->_val;

   res.copyScheme(*this);

   Graph &x = *this;

   for (size_t xNodeIndex = 0; xNodeIndex < x._nodes.size(); xNodeIndex++)
   {
      if (xNodeIndex >= y._nodes.size())
      {
         //такого узла в y нет, поэтому копируем все значения из x
         Node & xNode = x._nodes[xNodeIndex];
         for (auto &xEdgeType : xNode.edgesOut())
         {
            auto &xEdgeVec = xEdgeType._edges;
            for (auto &xEdge : xEdgeVec)
               res.add(xEdge);
         }
         for (auto &xEdgeType : xNode.edgesIn())
         {
            auto &xEdgeVec = xEdgeType._edges;
            for (auto &xEdge : xEdgeVec)
               res.add(xEdge);
         }
         continue;
      }
         
      Node & xNode = x._nodes[xNodeIndex];
      Node & yNode = (Node &)y._nodes[xNodeIndex];

      for (auto xEdgeType : xNode.edgesOut())
      {
         //if (yNode._edges.find(xEdgeType.first) == yNode._edges.end())
         if (yNode.edgesOut(xEdgeType._kind)._edges.size() == 0)
         {
            //связей с таким типом в y нет, копируем все из x
            for (auto &xEdge : xEdgeType._edges)
               res.add(xEdge);
            continue;
         }

         auto xEdgeVec = xEdgeType._edges;
         auto yEdgeVec = yNode.edgesOut(xEdgeType._kind)._edges;

         for (auto xEdge : xEdgeVec)
         {
            bool ok = true;
            for (auto &i : yEdgeVec)
            {
               if ( i._knd == xEdge._knd && i._n1 == xEdge._n1 && i._n2 == xEdge._n2 && i._val == xEdge._val )
               {
                  ok = false;
                  break;
               }
            }
            if (ok)
            {
               //в y такого ребра нет, берем из x
               res.add(xEdge);
            }
         }
      }
   }

   valGraph->_val.setOwner();

   return valGraph;
}

shared_ptr<ValueGraph> Graph::operator+(const Graph & y)
{
   ValueGraphPtr valGraph = ValueGraph::create();

   if (classID() != y.classID())
      return valGraph;

   Graph &res = valGraph->_val;

   //копируем все из текущего графа
   valGraph->_val = *this;

   Graph &x = *this;
   //затем копируем связи из графа y, которые отсутствуют в результирующем
   for (size_t yNodeIndex = 0; yNodeIndex < y._nodes.size(); yNodeIndex++)
   {
      if (yNodeIndex >= res._nodes.size())
      {
         //такого узла в res нет, поэтому копируем все значения из y
         Node & yNode = (Node &)y._nodes[yNodeIndex];
         for (auto &yEdgeType : yNode.edgesOut())
         {
            auto &yEdgeVec = yEdgeType._edges;
            for (auto &yEdge : yEdgeVec)
               res.add(yEdge);
         }
         for (auto &yEdgeType : yNode.edgesIn())
         {
            auto &yEdgeVec = yEdgeType._edges;
            for (auto &yEdge : yEdgeVec)
               res.add(yEdge);
         }
         continue;
      }

      Node & yNode   = (Node &)y._nodes[yNodeIndex];
      Node & resNode = res._nodes[yNodeIndex];

      for (auto yEdgeType : yNode.edgesOut())
      {
         if (resNode.edgesOut(yEdgeType._kind)._edges.size() == 0)
         {
            //связей с таким типом в res нет, копируем все из y
            for (auto &yEdge : yEdgeType._edges)
               res.add(yEdge);
            continue;
         }

         auto yEdgeVec   = yEdgeType._edges;
         auto resEdgeVec = resNode.edgesOut(yEdgeType._kind)._edges;

         for (auto &yEdge : yEdgeVec)
         {
            bool ok = true;
            for (auto &i : resEdgeVec)
            {
               if (i._knd == yEdge._knd && i._n1 == yEdge._n1 && i._n2 == yEdge._n2)
               {
                  ok = false;
                  break;
               }
            }
            if (ok)
            {
               //в res такого ребра нет, берем из y
               res.add(yEdge);
            }
         }
      }
   }

   valGraph->_val.setOwner();

   return valGraph;
}

Logic operator==(const Graph & x, const Graph & y)
{
   if (x.classID() != y.classID())
      return Logic::False;

   if (x._nodes.size() != y._nodes.size())
      return Logic::False;

   for (size_t xNodeIndex = 0; xNodeIndex < x._nodes.size(); xNodeIndex++)
   {
      if (xNodeIndex >= y._nodes.size())
         return Logic::False;

      Node & xNode = (Node &)x._nodes[xNodeIndex];
      Node & yNode = (Node &)y._nodes[xNodeIndex];

      if (xNode.edgesOut().size() != yNode.edgesOut().size())
         return Logic::False;

      if (xNode.edgesIn().size() != yNode.edgesIn().size())
         return Logic::False;

      for (auto xEdgeType : xNode.edgesOut())
      {
         if (xEdgeType._edges.size() != yNode.edgesOut(xEdgeType._kind)._edges.size())
            return Logic::False;

         auto &xEdgeVec = xEdgeType._edges;
         auto &yEdgeVec = yNode.edgesOut(xEdgeType._kind)._edges;

         for (auto const &xEdge : xEdgeVec)
         {
            if (find(yEdgeVec.begin(), yEdgeVec.end(), xEdge) == yEdgeVec.end())
               return Logic::False;
         }

         for (auto const &yEdge : yEdgeVec)
         {
            if (find(xEdgeVec.begin(), xEdgeVec.end(), yEdge) == xEdgeVec.end())
               return Logic::False;
         }
      }
      for (auto xEdgeType : xNode.edgesIn())
      {
         if (xEdgeType._edges.size() != yNode.edgesIn(xEdgeType._kind)._edges.size())
            return Logic::False;

         auto xEdgeVec = xEdgeType._edges;
         auto yEdgeVec = yNode.edgesIn(xEdgeType._kind)._edges;

         for (auto &xEdge : xEdgeVec)
         {
            if (find(yEdgeVec.begin(), yEdgeVec.end(), xEdge) == yEdgeVec.end())
               return Logic::False;
         }

         for (auto &yEdge : yEdgeVec)
         {
            if (find(xEdgeVec.begin(), xEdgeVec.end(), yEdge) == xEdgeVec.end())
               return Logic::False;
         }
      }
   }

   return Logic::True;
}

Logic operator!=(const Graph & x, const Graph & y)
{
   return !(x == y);
}

//=======================================================================================
//! Вывести граф в поток out в формате ds
//
ostream & Graph::print_ds(ostream & out)
{
   out << _graph_name << " {\n";
   //выводим узлы в отсортированном виде
   auto &node_names = _manager.nodeNames(_classID);
   for (auto &node : node_names)
   {
      if (node.second == 0)
         continue;
      print_obj_ds(node.second, out);
   }
   out << "}\n";
   return out;
}
//=======================================================================================
//! Вывести объект c идентификатором id в поток out в формате ds
//
void Graph::print_obj_ds(UInt id, ostream & out)
{
   Node & n = _nodes[id];
   UInt cnt = n.edgesOut().size();

   if (cnt == 0)                // нет прямых рёбер
      return;

   for (size_t it = 0; it < n.edgesOut().size(); it++)
   {
      map<Logic, string> vals;
      for (UInt i = 0; i < n.edgesOut()[it]._edges.size(); i++)
      {
         Edge & e = n.edgesOut()[it]._edges[i];
         map<Logic, string>::iterator it_v = vals.find(e._val);
         if (it_v == vals.end()) {
            vals.insert(pair<Logic, string>(e._val, "] = " + e._val.to_str()));
            it_v = vals.find(e._val);
         }
         Graph* graphN1 = this;
         Graph* graphN2 = e._valGraph2 ? &e._valGraph2->_val : this;
         string external = (graphN1 != graphN2) ? (graphN2->_graph_name + ".") : "";

         if (it_v->second[0] != ']')
            it_v->second = ", " + it_v->second;
         it_v->second = external + graphN2->node_name(e._n2) + it_v->second;
      }

      for (map<Logic, string>::iterator it_v = vals.begin(); it_v != vals.end(); it_v++)
         out << "   " << node_name(id) << " " << edge_name(n.edgesOut()[it]._kind) << ".["
         << it_v->second << "\n";
   }
}
