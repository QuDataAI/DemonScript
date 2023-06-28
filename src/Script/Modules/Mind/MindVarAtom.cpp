#include "MindVarAtom.h"
#include "Stack/StackFrame.h"

MindVarAtom::MindVarAtom():
   _not(false),
   _x1(0),
   _x2(0),
   _demon(0),
   _needSplit(false),
   _p(Logic::True)
{ 
}

//=======================================================================================
// ¬ывести атом дл€ графа g со значени€ми переменных vals и констнат names, 
// поставив перед ним !, если not_==true
//
string MindVarAtom::to_str(Graph &g, vector<UInt> &vals, MindNames & names, bool not_)
{
   Int edge = _edge;
   if (edge < 0)
      edge = ReservedEdgeIDs::EDGE_ISA;

   Logic v = (not_ ? !_val : _val);
   string res = "";
   if (_demon) {                                  // это демон
      res += not_ ? "!" : "";
      res +=_demon->_fun->_name + "(";
      for (size_t k = 0; k < _demon->_vars.size(); k++) {
         int x = _demon->_vars[k];
         res += (x > 0 ? g.node_name(vals[x]) : names._consts[-x].toString());
         if (k + 1 < _demon->_vars.size())
            res += ", ";
      }
      res += ")";
      return res;
   }
   res += (_x1 > 0 ? g.node_name(vals[_x1]) : names._consts[-_x1].toString());
   res += " ";
   if (edge > 0)
   {
      res += not_ ? "!" : "";
      res += g.edge_name(edge);
   }
   else
   {
      res += not_ ? "!=" : "==";
   }
   res += " ";
   res += (_x2 > 0 ? g.node_name(vals[_x2]) : names._consts[-_x2].toString());

   return res;
}
//=======================================================================================
// ¬ычислить атом-demon дл€ данных значений переменных vals
//
Logic MindVarAtom::calc_demon(Value &graphValue, vector<UInt> &vals, MindNames & names, Logic *value)
{
   CurrentGraph currentGraph(graphValue);                            // текущий граф
   for (size_t i = 0; i < _demon->_fun->_vars.size(); i++) {
      int x = _demon->_vars[i];
      if (x > 0)
      {
         Value &varVal = _demon->_fun->_codeCommon->_local_variables[i]->_val;
         if (varVal.type() == ValueBase::_NODE)
         {
            varVal.set_Node(ValueNode(graphValue.ptr(), vals[x]));
         }
         else
         {
            varVal = Value(ValueNode(graphValue.ptr(), vals[x]));
         }
      }
         
   }
   if (value) {
      StackFrame frame(*_demon->_fun);
      *(_demon->_fun->_valueVar) = *value;
      return _demon->_fun->set().get_Logic();
   }
   else
   {
      StackFrame frame(*_demon->_fun);
      return _demon->_fun->run().get_Logic();
   }

}
//=======================================================================================
// ¬ычислить атом дл€ данных значений переменных vals
//
void MindVarAtom::calc(Value &graphValue, vector<UInt> &vals, MindNames & names)
{
   if (_demon) {                                  // это демон
      _val = calc_demon(graphValue, vals, names);
      if (_not)                                   // атом с отрицанием
         _val = !_val;
      return;
   }

   Graph *g = graphValue.get_Graph();
   UInt graphClassID = graphValue.get_GraphClassID();
   UInt v1, v2; Graph *g1, *g2;                   // вычисл€ем значени€ аргументов
   if (_x1 > 0) {
      v1 = vals[_x1]; g1 = 0;
   }
   else {
      Value & c = names._consts[-_x1];
      v1 = c.get_UInt();
      g1 = (graphClassID == c.get_GraphClassID()) ? g : c.get_Graph();
   }
   if (_x2 > 0) {
      v2 = vals[_x2]; g2 = 0;
   }
   else {
      Value & c = names._consts[-_x2];
      v2 = c.get_UInt();
      g2 = (graphClassID == c.get_GraphClassID()) ? 0 : c.get_Graph();
   }

   if (_edge == 0) {                              // это равенство
      _val = ((v1 == v2) && (g1 == g2)) ? Logic::True : Logic::False;
   }
   else {
      if (_edge > 0)
         _val = g->val(_edge, v1, v2, g1, g2, false); // это произвольное ребро
      else {                                          // это функци€ isa
         _val = g->path(ReservedEdgeIDs::EDGE_ISA, v1, v2, g1, g2);
         if (_val == Logic::False)                // ??? \todo
            _val = Logic::Undef;
      }
   }
   if (_not)                                      // атом с отрицанием
      _val._not(); //_val = !_val;
}
