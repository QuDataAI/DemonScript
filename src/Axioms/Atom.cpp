#include "Atom.h"
#include "Expr.h"

void Atom::calc()
{
   _expr->root(false);
   _val = _expr->run().get_Logic();
   if (_not)                                      // атом с отрицанием
      _val._not(); //_val = !_val;
}

void Atom::set()
{
   _val = _not ? Logic::False : Logic::True;      // новое значениие

   if (_expr->kind() == Expr::_EDGE)
   {
      ExprEdgePtr exprEdge = SPTR_DCAST(ExprEdge,_expr);
      if (exprEdge->_val == Logic::False)
      {
         _val = !_val;
      }
      _expr->root(true);
   }
   _expr->set(_val);
}

string Atom::print()
{
   stringstream str;
   if (_not)
      str << "!";
   _expr->print(str);   
   return str.str();
}
