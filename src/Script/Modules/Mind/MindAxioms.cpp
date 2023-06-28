#include "MindAxioms.h"
#include "MindAxiom.h"
#include "MindAtom.h"
#include "MindState.h"


MindAxioms::MindAxioms()
{
}

MindAxioms::~MindAxioms()
{
}

void MindAxioms::add(MindState &state)
{
   MindVarAxiomPtr varAxiom = state._axiom;
   MindAxiomPtr axiom = SPTR_MAKE(MindAxiom)();
   vector<MindVarAtom>& varAtoms = varAxiom->_atoms;
   axiom->_literals.resize(varAtoms.size());
   for (size_t i = 0; i < varAtoms.size(); i++) {
      MindVarAtom & varAtom = varAtoms[i];
      MindAtomPtr      atom = getAtom(varAtom, state);
      MindLiteral      &mindLiteral = axiom->_literals[i];
      mindLiteral._atom = atom;
      mindLiteral._not  = varAtom._not;
      if (mindLiteral._not)
      {
         atom->_axioms_not.push_back(axiom);
      }
      else
      {
         atom->_axioms.push_back(axiom);
      }           
   }
   _axioms.push_back(axiom);
}

MindAtomPtr MindAxioms::getAtom(MindVarAtom & varAtom, MindState &state)
{
   // вычисляем значения аргументов
   vector<UInt> &vals = state._vals;
   MindNames & names  = state._names;
   Value & graphValue = state._graphValue;
   UInt graphClassID  = graphValue.get_GraphClassID();
   vector<UInt> atomVals;
   Value vg1, vg2;
   int  edge = varAtom._edge;
   MindDemonPtr demon = varAtom._demon;
   if (demon) {                                  // это демон
      atomVals.resize(vals.size());
      for (size_t i = 0; i < demon->_fun->_vars.size(); i++) {
         int x = demon->_vars[i];
         if (x > 0)
         {
            atomVals[i] = vals[x];
         }            
      }
   }
   else
   {
      UInt v1, v2; Graph *g1, *g2;
      if (varAtom._x1 > 0) {
         v1 = vals[varAtom._x1]; g1 = 0;
      }
      else {
         Value & c = names._consts[-varAtom._x1];
         v1 = c.get_UInt();
         g1 = (graphClassID == c.get_GraphClassID()) ? graphValue.get_Graph() : c.get_Graph();
      }
      if (varAtom._x2 > 0) {
         v2 = vals[varAtom._x2]; g2 = 0;
      }
      else {
         Value & c = names._consts[-varAtom._x2];
         v2 = c.get_UInt();
         g2 = (graphClassID == c.get_GraphClassID()) ? 0 : c.get_Graph();
      }
      if (g1)
         vg1 = Value(g1->_valGraph.lock());
      if (g2)
         vg2 = Value(g2->_valGraph.lock());
      atomVals.resize(2);
      atomVals[0] = v1;
      atomVals[1] = v2;
   }

   //сначала пытаемся найти уже добавленный атом, если нет создаем новый
   for (auto &atom : _atoms)
   {
      if (atom->_edge     == edge       &&
         atom->_vals      == atomVals   &&
         atom->_vg1.ptr() == vg1.ptr()  &&
         atom->_vg2.ptr() == vg2.ptr()  &&
         atom->_demon     == demon
         )
      {
         //атом уже добавлен, используем его
         return atom;
      }
   }
   //создаем новый атом
   MindAtomPtr newAtom = SPTR_MAKE(MindAtom)(edge, demon, atomVals, vg1, vg2);
   _atoms.push_back(newAtom);
   return newAtom;
}

MindAtomPtr MindAxioms::getEdgeAtom(int edge, UInt n1, UInt n2, Value &vg2)
{
   vector<UInt> atomVals;
   atomVals.push_back(n1);
   atomVals.push_back(n2);

   //сначала пытаемся найти уже добавленный атом, если нет создаем новый
   for (auto &atom : _atoms)
   {
      if (atom->_edge == edge                   &&
         atom->_demon == 0                      &&
         atom->_vals  == atomVals               &&
         atom->_vg1.type() == ValueBase::_NONE  &&
         atom->_vg2.ptr()  == vg2.ptr()
         )
      {
         //атом уже добавлен, используем его
         return atom;
      }
   }
   //создаем новый атом
   MindAtomPtr newAtom = SPTR_MAKE(MindAtom)(edge, nullptr, atomVals, Value(), vg2);
   _atoms.push_back(newAtom);
   return newAtom;
}

void MindAxioms::check()
{   
   for (auto axiom : _axioms)
   {
      int trueLiteralIndex = -1;
      int falseLiterals = 0;
      for (int i=0; i<axiom->_literals.size();i++)
      {
         MindLiteral &literal = axiom->_literals[i];
         Logic atomVal = literal._atom.lock()->_val;
         Logic literalVal = literal._not ? !literal._atom.lock()->_val : literal._atom.lock()->_val;
         if (literalVal == Logic::False)
         {
            falseLiterals++;
         }
         else if (literalVal == Logic::True)
         {
            if (trueLiteralIndex >= 0)
            {
               //больше одного True, аксиома не сработала
               trueLiteralIndex = -1;
               continue;
            }
            trueLiteralIndex = i;
         }
      }
      if (trueLiteralIndex >= 0 && falseLiterals == axiom->_literals.size() - 1)
      {
         //аксиома сработала (F F F F T)
         axiom->_trueLiteralIndex = trueLiteralIndex;
      }
   }
}

