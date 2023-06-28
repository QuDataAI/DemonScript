#include "Node.h"
#include "Graph.h"
#include "ValueBase.h"
#include "ValueGraph.h"

/*static*/ EdgeList Node::dummy;

//! Меньше для ребер
bool EdgeLess(const Edge &x, const Edge &y)
{
   // Мы используем эту сортировку исключительно для поиска, не для визуализации - нам не нужно сложное условие.
   //if (x._valGraph2 && y._valGraph2 && x._valGraph2->eq(y._valGraph2.get()).isFalse())
   //   return x._valGraph2->get_Graph()->name() < y._valGraph2->get_Graph()->name();

   return x._n2 < y._n2;
}

//! Меньше для ребер по именам
bool EdgeLessName(const Edge &x, const Edge &y)
{
   if ((!x._valGraph1.lock() && !x._valGraph2) || (!y._valGraph1.lock() && !y._valGraph2))
      return x._n2 < y._n2;

   string nameX = (x._valGraph2) ? x._valGraph2->get_Graph()->node_name(x._n2) : x._valGraph1.lock()->get_Graph()->node_name(x._n2);
   string nameY = (y._valGraph2) ? y._valGraph2->get_Graph()->node_name(y._n2) : y._valGraph1.lock()->get_Graph()->node_name(y._n2);

   return nameX < nameY;
}

//! Меньше для списков ребер
bool EdgeListLess(const EdgeList &x, const EdgeList &y)
{
   return x._kind < y._kind;
}

void EdgeList::sort(Int mode)
{
   if (_sort == mode)
      return;

   if (mode == Node::sortTargetNode)
   {
      std::sort(_edges.begin(), _edges.end(), EdgeLess);
   }
   else if (mode == Node::sortTargetName)
   {
      std::sort(_edges.begin(), _edges.end(), EdgeLessName);
   }
   _sort = mode;
}

void EdgeList::setOwner(shared_ptr<ValueGraph>  &valGraph1)
{
   for (size_t i = 0; i < _edges.size(); i++)
   {
      _edges[i]._valGraph1 = valGraph1;
   }
}
//=======================================================================================
//                                    Node
//=======================================================================================
// Добавить в отношение knd объект n2 со значением отношения val
//
Int Node::add(Int knd, UInt n2, Logic val, shared_ptr<ValueGraph> valGraph1, shared_ptr<ValueGraph> valGraph2/* = 0*/)
{
   static int changeID = 0;
   valGraph2 = valGraph1 == valGraph2 ? 0 : valGraph2;
   EdgeList & lst = (knd >= 0) ? edgesOut(knd, true) : edgesIn(-knd, true);
   vector<Edge> & vec = lst._edges;
   Graph* n1Graph = valGraph1->get_Graph();
   for (UInt i = 0; i < vec.size(); i++)
      if (vec[i]._n2 == n2 && vec[i]._valGraph2 == valGraph2)
      {  // такой узел уже есть
         if (vec[i]._val == val)
            return 0;                                 // ничего не меняем         
         if (n1Graph && !n1Graph->_edges_blocked)
         {    // если не заблокирован - меняем
            if (n1Graph->locked())
            {
               TRACE << "? Node::add> You are trying to change edge in locked graph" << endl;
               n1Graph->_edges_blocked = 1;
            }
            else if (!val.isUndef())
            {
               if (n1Graph->verbose() > 0 && knd >= 0)
               {                  
                  string n2Name = valGraph2 ? (valGraph2->get_Graph()->name() + "." + valGraph2->get_Graph()->node_name(n2)) : n1Graph->node_name(n2);
                  TRACE << "? Node::add> You change value of edge "
                     << n1Graph->node_name(id()) << " " << n1Graph->edge_name(knd) << " " << n2Name << " from " << vec[i]._val << " to "
                     << val << " in graph " << n1Graph->name() << "(id:" << changeID++ << ")" << endl;
               }
               vec[i]._val = val;
            }
            else
            {
               if (n1Graph->verbose() > 0)
               {
                  string n2Name = valGraph2 ? (valGraph2->get_Graph()->name() + "." + valGraph2->get_Graph()->node_name(n2)) : n1Graph->node_name(n2);
                  TRACE << "? Node::add> You deleted edge "
                     << n1Graph->node_name(id()) << " " << n1Graph->edge_name(knd) << " " << n2Name << " from " << vec[i]._val << " to "
                     << val << endl;
               }
               vec.erase(vec.begin() + i); // Будет сразу же ретурн, нет смысла откатывать i назад
               if (vec.size() == 0)
                  clearEdge(knd);
            }
            n1Graph->_edges_changes++;
            return 1;
         }
         else return 0;
      }
   if (!val.isUndef())
   {
      if (n1Graph->verbose() > 1 && knd>0)
      {
         string n2Name = valGraph2 ? (valGraph2->get_Graph()->name() + "." + valGraph2->get_Graph()->node_name(n2)) : n1Graph->node_name(n2);
         TRACE_STREAM(outTxt);
         outTxt << "Node::add: " << n1Graph->node_name(id()) + " ";
         if (val == Logic::False)
         {
            outTxt << "!";
         }         
         outTxt << n1Graph->edge_name(knd) << " " << n2Name;
         if (val != Logic::True && val != Logic::False)
            outTxt << " = " + val;
         outTxt << " (graph: \"" + n1Graph->name() << "\", id:" << changeID++ << ")";
         outTxt << endl;
      }
      vec.push_back(Edge((knd >= 0) ? knd : -knd, id(), n2, val, valGraph1, valGraph2));       // добавляем ребро
      lst._sort = sortNone;      
      if (n1Graph->autoSort())
         sort();
   }
   return 1;
}

EdgeList & Node::edgesHelp(vector<EdgeList> &edges, int kind, bool add /*= false*/)
{
   //static EdgeList dummy;
   if (edges.size() == 0)
   {
      if (add)
      {
         edges.push_back(dummy);
         edges.back()._kind = kind;
         return edges[0];
      }
      return dummy;
   }

   size_t left = 0;
   size_t right = edges.size();
   size_t center = (left + right) / 2;

   while (left != right)
   {
      center = (left + right) / 2;
      if (edges[center]._kind == kind)
         return edges[center];
      else if (edges[center]._kind < kind && left != center)
         left = center;
      else
         right = center;
   }

   if (edges[center]._kind == kind)
      return edges[center];

   if (add)
   {
      size_t index = center;
      if (edges[center]._kind < kind)
         index++;
      edges.insert(edges.begin() + index, dummy);
      edges[index]._kind = kind;

      return edges[index];
   }
   return dummy;
}

EdgeList & Node::edgesOut(int kind, bool add/*=false*/)
{
   return edgesHelp(_edgesOut, kind, add);
}

EdgeList & Node::edgesIn(int kind, bool add /*= false*/)
{
   return edgesHelp(_edgesIn, kind, add);
}

void Node::clearEdgeHelp(vector<EdgeList> &edges, int kind)
{
   size_t left = 0;
   size_t right = edges.size();
   size_t center = (left + right) / 2;

   if (edges.size() == 0)
      return;

   while (left != right)
   {
      center = (left + right) / 2;
      if (edges[center]._kind == kind)
      {
         edges.erase(edges.begin() + center);
         return;
      }
      else if (edges[center]._kind < kind && left != center)
         left = center;
      else
         right = center;
   }

   if (edges[center]._kind == kind)
   {
      edges.erase(edges.begin() + center);
      return;
   }
}

void Node::clearEdge(int kind)
{
   if (kind >= 0)
      clearEdgeHelp(_edgesOut, kind);
   else
      clearEdgeHelp(_edgesIn, -kind);
}

void Node::sort()
{
   // Списки ребер держатся отсортированными всегда (см. метод добавления)
   //if (!_sortLists)
   //{
   //   _sortLists = true;
   //   std::sort(_edgesOut.begin(), _edgesOut.end(), EdgeListLess);
   //   std::sort(_edgesIn.begin(), _edgesIn.end(), EdgeListLess);
   //}
   for (size_t i = 0; i < _edgesOut.size(); i++)
   {
      _edgesOut[i].sort(sortTargetNode);
   }
   for (size_t i = 0; i < _edgesIn.size(); i++)
   {
      _edgesIn[i].sort(sortTargetNode);
   }
}

void Node::setOwner(shared_ptr<ValueGraph>  &valGraph1)
{
   for (size_t i = 0; i < _edgesOut.size(); i++)
   {
      _edgesOut[i].setOwner(valGraph1);
   }
   for (size_t i = 0; i < _edgesIn.size(); i++)
   {
      _edgesIn[i].setOwner(valGraph1);
   }
}