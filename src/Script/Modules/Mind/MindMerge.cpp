#include "MindMerge.h"

MindMerge::MindMerge(Value & graph1Val, MindAxioms & graph1Axioms, Value & graph2Val, MindAxioms & graph2Axioms, Value &excludeEdges, int verbose) :
   _mergeItem1(graph1Val, graph1Axioms, excludeEdges),
   _mergeItem2(graph2Val, graph2Axioms, excludeEdges),
   _verbose(verbose)
{
}

void MindMerge::init()
{
   Value &               graphVal1    = _mergeItem1._graphVal;     
   Value &               graphVal2    = _mergeItem2._graphVal;
   MindAxioms &          axioms1      = _mergeItem1._graphAxioms;
   MindAxioms &          axioms2      = _mergeItem2._graphAxioms;
   vector<MindAtomPtr> & atoms1       = axioms1._atoms;
   vector<MindAtomPtr> & atoms2       = axioms2._atoms;

   //определенные ребра графа тоже по сути являются единичными аксиомами, поэтому 
   //добавляем и их
   addSingleAtoms();

   //добавить атомы из списка исключений
   addExcludeAtoms();

   if (atoms1.size() != atoms2.size())
      return;

   for (int i = 0; i < atoms1.size(); i++)
   {
      MindAtomPtr atom1 = atoms1[i];
      MindAtomPtr atom2 = atoms2[i];
      if (atom1->_val == Logic::Undef)
         atom1->calc(graphVal1);
      if (atom2->_val == Logic::Undef)
         atom2->calc(graphVal2);
      if (atom1->_val != Logic::Undef && atom2->_val == Logic::Undef)
      {
         //атом 1 становится кандидатом на перенос в граф 2 
         _mergeItem1._waitForCopy.push_back(atom1);
      }
      else if (atom1->_val == Logic::Undef && atom2->_val != Logic::Undef)
      {
         //атом 2 становится кандидатом на перенос в граф 1
         _mergeItem2._waitForCopy.push_back(atom2);
      }
      else if (
               atom1->_val == Logic::False && atom2->_val == Logic::True 
            )
      {
         //оба атома и их следствия переносить нельзя
         _mergeItem1._waitForCancel.push_back(MindLiteral(atom1,true));
         _mergeItem2._waitForCancel.push_back(MindLiteral(atom2,false));
      }
      else if (
         atom1->_val == Logic::True  && atom2->_val == Logic::False
         )
      {
         //оба атома и их следствия переносить нельзя
         _mergeItem1._waitForCancel.push_back(MindLiteral(atom1, false));
         _mergeItem2._waitForCancel.push_back(MindLiteral(atom2, true));
      }
   }

   if (!need())
      return;

   axioms1.check();
   axioms2.check();
}

bool MindMerge::need()
{
   return (_mergeItem1._waitForCopy.size() + _mergeItem2._waitForCopy.size()) > 0;
}

void MindMerge::run()
{
   if (!need())
      return;

   //рекурсивно отменяем следствия атомов, из списка "DONT_COPY";
   while (processWaitForCancel(_mergeItem1)) {};
   while (processWaitForCancel(_mergeItem2)) {};
   //переносим не отмененные атомы в противоположный граф
   transferWaitForCopy(_mergeItem1, _mergeItem2);
   transferWaitForCopy(_mergeItem2, _mergeItem1);
}

void MindMerge::addSingleAtoms()
{
   Value &               graphVal1 = _mergeItem1._graphVal;
   Value &               graphVal2 = _mergeItem2._graphVal;
   MindAxioms &          axioms1 = _mergeItem1._graphAxioms;
   MindAxioms &          axioms2 = _mergeItem2._graphAxioms;
   Graph *                    g1 = graphVal1.get_Graph();
   Graph *                    g2 = graphVal2.get_Graph();
   vector<Edge> edges;
   g1->get_out(edges);
   for (auto &edge : edges)
   {
      MindAtomPtr edgeAtom1 = axioms1.getEdgeAtom(edge._knd, edge._n1, edge._n2, Value(edge._valGraph2));
      MindAtomPtr edgeAtom2 = axioms2.getEdgeAtom(edge._knd, edge._n1, edge._n2, Value(edge._valGraph2));
      edgeAtom1->_val = edge._val;
   }
   edges.clear();
   g2->get_out(edges);
   for (auto &edge : edges)
   {
      MindAtomPtr edgeAtom1 = axioms1.getEdgeAtom(edge._knd, edge._n1, edge._n2, Value(edge._valGraph2));
      MindAtomPtr edgeAtom2 = axioms2.getEdgeAtom(edge._knd, edge._n1, edge._n2, Value(edge._valGraph2));
      edgeAtom2->_val = edge._val;
   }
}

void MindMerge::addExcludeAtoms()
{
   Value &               graphVal1 = _mergeItem1._graphVal;
   Value &               graphVal2 = _mergeItem2._graphVal;
   MindAxioms &          axioms1 = _mergeItem1._graphAxioms;
   MindAxioms &          axioms2 = _mergeItem2._graphAxioms;
   Graph *                    g1 = graphVal1.get_Graph();
   Graph *                    g2 = graphVal2.get_Graph();
   Value &               excludeEdges1 = _mergeItem1._excludeEdges;
   Value &               excludeEdges2 = _mergeItem2._excludeEdges;
   if (excludeEdges1.type() == ValueBase::_ARRAY)
   {
      for (int i = 0; i < excludeEdges1.size(); i++)
      {
         Value excludeEdge = excludeEdges1[i];
         if (excludeEdge.type() == ValueBase::_ARRAY)
         {
            UInt node1ID = excludeEdge[0]->get_UInt();
            Int  kindID  = excludeEdge[1]->get_Int();
            if (excludeEdge.size() == 2)
            {
               for (UInt node2ID = 1; node2ID < g1->_nodes.size(); node2ID++)
               {
                  MindAtomPtr edgeAtom1 = axioms1.getEdgeAtom(kindID, node1ID, node2ID, Value());
                  MindAtomPtr edgeAtom2 = axioms2.getEdgeAtom(kindID, node1ID, node2ID, Value());
                  MindLiteral excludeLiteral(edgeAtom1, false);
                  _mergeItem1._waitForCancel.push_back(excludeLiteral);
                  if (_verbose > 0)
                  {
                     TRACE << "Mind.merge exclude literal: " << excludeLiteral.to_str(*g1, false, true) << " (graph: \"" << g1->name() << "\")" << endl;
                  }
               }
            }
            else
            {
               UInt node2ID = excludeEdge[2]->get_UInt();
               MindAtomPtr edgeAtom1 = axioms1.getEdgeAtom(kindID, node1ID, node2ID, Value());
               MindAtomPtr edgeAtom2 = axioms2.getEdgeAtom(kindID, node1ID, node2ID, Value());
               MindLiteral excludeLiteral(edgeAtom1, false);
               _mergeItem1._waitForCancel.push_back(excludeLiteral);
               if (_verbose > 0)
               {
                  TRACE << "Mind.merge exclude literal: " << excludeLiteral.to_str(*g1, false, true) << " (graph: \"" << g1->name() << "\")" << endl;
               }
            }
         }
      }
   }
   if (excludeEdges2.type() == ValueBase::_ARRAY)
   {
      for (int i = 0; i < excludeEdges2.size(); i++)
      {
         Value excludeEdge = excludeEdges2[i];
         if (excludeEdge.type() == ValueBase::_ARRAY)
         {
            UInt node1ID = excludeEdge[0]->get_UInt();
            Int  kindID  = excludeEdge[1]->get_Int();
            if (excludeEdge.size() == 2)
            {
               for (UInt node2ID = 1; node2ID < g2->_nodes.size(); node2ID++)
               {
                  MindAtomPtr edgeAtom1 = axioms1.getEdgeAtom(kindID, node1ID, node2ID, Value());
                  MindAtomPtr edgeAtom2 = axioms2.getEdgeAtom(kindID, node1ID, node2ID, Value());
                  _mergeItem2._waitForCancel.push_back(MindLiteral(edgeAtom2, false));
                  MindLiteral excludeLiteral(edgeAtom2, false);
                  if (_verbose > 0)
                  {
                     TRACE << "Mind.merge exclude literal: " << excludeLiteral.to_str(*g2, false, true) << " (graph: \"" << g2->name() << "\")" << endl;
                  }
               }
            }
            else
            {
               UInt node2ID = excludeEdge[2]->get_UInt();
               MindAtomPtr edgeAtom1 = axioms1.getEdgeAtom(kindID, node1ID, node2ID, Value());
               MindAtomPtr edgeAtom2 = axioms2.getEdgeAtom(kindID, node1ID, node2ID, Value());
               _mergeItem2._waitForCancel.push_back(MindLiteral(edgeAtom2, false));
               MindLiteral excludeLiteral(edgeAtom2, false);
               if (_verbose > 0)
               {
                  TRACE << "Mind.merge exclude literal: " << excludeLiteral.to_str(*g2, false, true) << " (graph: \"" << g2->name() << "\")" << endl;
               }
            }
         }
      }
   }
}

bool MindMerge::processWaitForCancel(MindMergeItem & mergeItem)
{
   if (mergeItem._waitForCancel.size() == 0)
      return false;

   MindLiteral cancelLiteral = mergeItem._waitForCancel.back();
   MindAtomPtr cancelAtom = cancelLiteral._atom.lock();
   Graph *g = mergeItem._graphVal.get_Graph();
   mergeItem._waitForCancel.pop_back();
   if (_verbose > 0)
   {
      TRACE << "Mind.merge cancel literal: " << cancelLiteral.to_str(*g,false,true) << " (graph: \"" << g->name() << "\")"<< endl;
   }

   vector<MindAxiomWPtr> &cancelAxioms = cancelLiteral._not ? cancelAtom->_axioms : cancelAtom->_axioms_not;

   //отменяем атом и его следствия
   cancelAtom->_val = Logic::Undef;

   //перебираем все аксиомы, в которых атом участвует
   for (auto axiom : cancelAxioms)
   {
      if (axiom.lock()->_trueLiteralIndex < 0)
         continue; // пропускаем не сработавшие аксиомы
      
      //смотрим, не сработала ли данная аксиома из-за указанного атома
      MindLiteral &trueLiteral = axiom.lock()->_literals[axiom.lock()->_trueLiteralIndex];
      if (trueLiteral._atom.lock() != cancelAtom)
      {
         //аксиома выполнилась из-за отменяемого атома, поэтому отменяем и ее вывод
         if (_verbose > 0)
         {
            TRACE << "Mind.merge cancel axiom: " << axiom.lock()->to_str(*g) << " (graph: \"" << g->name() << "\")" << endl;
         }
         if (trueLiteral._atom.lock()->_val != Logic::Undef)
         {
            mergeItem._waitForCancel.push_back(MindLiteral(trueLiteral._atom, trueLiteral._atom.lock()->_val == Logic::False));
            trueLiteral._atom.lock()->_val = Logic::Undef;
         }         
         axiom.lock()->_trueLiteralIndex = -1;
      }
   }
   return true;
}

void MindMerge::transferWaitForCopy(MindMergeItem & mergeItem1, MindMergeItem & mergeItem2)
{
   //копируем не отмененные атомы в противоположный граф
   Value & graph1V = mergeItem1._graphVal;
   Value & graph2V = mergeItem2._graphVal;
   for (auto atom : mergeItem1._waitForCopy)
   {
      if (atom->_val == Logic::Undef)
         continue;   // атом отменен, переносить уже нечего

      if (graph1V.ptr() != atom->_vg1)
         continue;   // атом не работает с текущим графом

      Value saveVg1 = atom->_vg1;
      atom->_vg1 = graph2V.ptr();
      atom->set(graph2V, atom->_val, _verbose, "Merge::");
      atom->_vg1 = saveVg1;
   }
}

