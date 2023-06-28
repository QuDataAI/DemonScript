#include "MindState.h"

MindState::MindState(MindStateMode mode, 
   Value & graphValue,
   vector<UInt>& vals, 
   MindNames & names, 
   int max_num_set/* = -1*/,
   int verbose/* = 0*/,
   bool show_false /*= false*/,
   int out_undef/* = 0*/):
   _mode(mode),
   _graphValue(graphValue),
   _vals(vals),
   _names(names),
   _max_num_sets(max_num_set),
   _verbose(verbose),
   _show_false(show_false),
   _out_undef(out_undef),
   _blocked(false),
   _num_sets(0)
{

}

void MindState::addTransaction(MindVarAxiomPtr axiom, int atomIndex)
{
   if (_blocked)
      return;

   _num_sets++;

   if (_mode == MODE_SET)
   {
      //выполняем сразу
      setAtom(axiom, atomIndex, _vals);
   }
   else
   {
      //выполним позже
      _pendingTransactions.push_back(MindTransaction(axiom, atomIndex, _vals));
   }

   if (_max_num_sets > -1 && _num_sets >= _max_num_sets)
      _blocked = true;
}

void MindState::setAtom(MindVarAxiomPtr axiom, int atomIndex, vector<UInt>& vals, bool val/* = true*/)
{
   MindVarAtom & atom = axiom->_atoms[atomIndex];

   atom._val = atom._not ? Logic::False : Logic::True;   // новое значениие
   atom._val = val ? atom._val : !atom._val;             // инвертируем для аксиом usual

   if (_verbose)
   {
      printSetAtom(axiom, atomIndex, vals);
   }

   if (atom._demon) {                                    // это демон с set:
      if (!atom._demon->_set) {
         TRACE_CRITICAL << "! Mind::setAtom> Demon without set!\n";
         setBlock();
         return;
      }
      atom.calc_demon(_graphValue, vals, _names, &atom._val);
   }
   else {                                         // это обычное ребро:
      Int edge = atom._edge;
      if (edge < 0)
         edge = ReservedEdgeIDs::EDGE_ISA;
      Graph *g = _graphValue.get_Graph();
      UInt   gClassID = g->classID();
      UInt   n1 = atom._x1 > 0 ? vals[atom._x1] : _names._consts[-atom._x1].get_UInt();
      UInt   n2 = atom._x2 > 0 ? vals[atom._x2] : _names._consts[-atom._x2].get_UInt();
      Graph *g2 = atom._x2 > 0 ? 0 : (gClassID == _names._consts[-atom._x2].get_GraphClassID() ? 0 : _names._consts[-atom._x2].get_Graph());
      g->add(edge, n1, n2, atom._val, g2);
   }
}

string MindState::atomToStr(MindVarAxiomPtr axiom, int atomIndex, vector<UInt> &vals, bool val)
{
   MindVarAtom & a = axiom->_atoms[atomIndex];

   Graph *g = _graphValue.get_Graph();

   string out;
   
   out = axiom->to_str(*g, vals, _names, atomIndex, val);
   return out;
}

void MindState::printSetAtom(MindVarAxiomPtr axiom, int atomIndex, vector<UInt> &vals)
{
   MindVarAtom & a = axiom->_atoms[atomIndex];

   Graph *g = _graphValue.get_Graph();

   TRACE << "Mind::set: " << axiom->to_str(*g, vals, _names, atomIndex, a._val) << " (graph: \"" << g->name()  << "\")" << endl;
}

void MindState::setBlock()
{
   _blocked = true;
}
