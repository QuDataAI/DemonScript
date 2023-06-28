#include "Edge.h"

Edge::Edge(Int knd, UInt n1, UInt n2, Logic & val, shared_ptr<ValueGraph> valGraph1, shared_ptr<ValueGraph> valGraph2) :
   _knd(knd), _n1(n1), _n2(n2), _val(val), _valGraph1(valGraph1), _valGraph2(valGraph2)
{
}

bool operator==(const Edge & x, const Edge & y)
{
   return (x._n1        == y._n1          && 
           x._n2        == y._n2          && 
           x._valGraph2 == y._valGraph2   &&
           x._knd       == y._knd         &&
           x._val       == y._val
          );
}

bool operator!=(const Edge & x, const Edge & y)
{
   return !(x==y);
}
