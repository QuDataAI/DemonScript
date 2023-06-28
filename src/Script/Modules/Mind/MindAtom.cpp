#include "MindAtom.h"
#include "Stack/StackFrame.h"


MindAtom::MindAtom(Int edge, MindDemonPtr demon, vector<UInt>& atomVals, Value &vg1, Value &vg2):
   _edge(edge),
   _demon(demon),
   _vals(atomVals),
   _vg1(vg1),
   _vg2(vg2),
   _val(Logic::Undef)
{
}

MindAtom::~MindAtom()
{
   int a = 0;
}

void MindAtom::calc(Value &graphValue)
{
   if (_demon) {                                  // это демон
      _val = calc_demon(graphValue);
      return;
   }

   Graph *g = graphValue.get_Graph();
   Graph * g1 = _vg1.get_Graph();
   Graph * g2 = _vg2.get_Graph();

   UInt v1 = _vals[0];
   UInt v2 = _vals[1];

   if (_edge == 0) {                              // это равенство
      _val = ((v1 == v2) && (g1 == g2)) ? Logic::True : Logic::False;
   }
   else {
      if (_edge > 0)
         _val = g->val(_edge, v1, v2, g1, g2, false); // это произвольное ребро
      else {                                          // это функция isa
         _val = g->path(ReservedEdgeIDs::EDGE_ISA, v1, v2, g1, g2);
         if (_val == Logic::False)                // ??? \todo
            _val = Logic::Undef;
      }
   }
}

Logic MindAtom::calc_demon(Value & graphValue)
{
   CurrentGraph currentGraph(graphValue);                            // текущий граф
   for (size_t i = 0; i < _demon->_fun->_vars.size(); i++) {
      int x = _demon->_vars[i];
      if (x > 0)
      {
         Value &varVal = _demon->_fun->_codeCommon->_local_variables[i]->_val;
         if (varVal.type() == ValueBase::_NODE)
         {
            varVal.set_Node(ValueNode(graphValue.ptr(), _vals[i]));
         }
         else
         {
            varVal = Value(ValueNode(graphValue.ptr(), _vals[i]));
         }
      }
   }
   {
      StackFrame frame(*_demon->_fun);
      return _demon->_fun->run().get_Logic();
   }
}

void MindAtom::set(Value & graphValue, Logic val, int verbose/* = 0*/, const char* verbosePrf/* = ""*/)
{
   if (_demon) {                                    // это демон с set:
      if (!_demon->_set) {
         TRACE_CRITICAL << "! Mind::set> Demon without set!\n";        
         return;
      }
      set_demon(graphValue, _val);
   }
   else {                                         // это обычное ребро:
      Int edge = _edge;
      if (edge < 0)
         edge = ReservedEdgeIDs::EDGE_ISA;
      Graph *g = graphValue.get_Graph();
      UInt   gClassID = g->classID();
      UInt   n1 = _vals[0];
      UInt   n2 = _vals[1];
      Graph *g2 = _vg2.get_Graph();
      if (verbose > 0)
      {
         TRACE_STREAM(tout);
         tout << verbosePrf << "MindAtom::set: " << to_str(*g) << "\n";
      }
      g->add(edge, n1, n2, val, g2);
   }
}

void MindAtom::set_demon(Value & graphValue, Logic val)
{
   StackFrame frame(*_demon->_fun);
   *(_demon->_fun->_valueVar) = val;
   _demon->_fun->set().get_Logic();
}

string MindAtom::to_str(Graph &g, bool not_, bool ignoreVal)
{
   Int edge = _edge;
   if (edge < 0)
      edge = ReservedEdgeIDs::EDGE_ISA;

   Graph * g1 = _vg1.get_Graph();
   Graph * g2 = _vg2.get_Graph();

   UInt v1 = _vals[0];
   UInt v2 = _vals[1];

   Logic val = not_ ? !_val : _val;
   if (ignoreVal)
   {
      val = not_ ? Logic::False : Logic::True;
   }
   string res = "";
   if (_demon) {                                  // это демон
      res += not_ ? "!" : "";
      res += _demon->_fun->_name + "(";
      for (size_t k = 0; k < _demon->_vars.size(); k++) {
         res += g.node_name(_vals[k]);
         if (k + 1 < _demon->_vars.size())
            res += ", ";
      }
      res += ")";
      return res;
   }
   res += g.node_name(v1) ;
   res += " ";
   if (edge > 0)
   {
      res += val == Logic::False ? "!" : "";
      res += g.edge_name(edge);
   }
   else
   {
      res += val == Logic::False ? "!=" : "==";
   }
   res += " ";
   if (g2)
   {
      res += g2->name() + "." + g2->node_name(v2);
   }
   else
   {
      res += g.node_name(v2);
   }

   return res;
}
