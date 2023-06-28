#include "MindAxiom.h"
#include "MindAtom.h"
#include "MindVarAxiom.h"
#include "MindState.h"

MindAxiom::MindAxiom():
   _trueLiteralIndex(-1)
{
}

MindAxiom::~MindAxiom()
{

}

string MindAxiom::to_str(Graph &g)
{
   string res = "";
   bool wasOut = false;
   for (int i = 0; i < _literals.size(); i++)
   {
      if (i != _trueLiteralIndex)
      {
         if (wasOut)
         {
            res += " & ";
         }
         res += _literals[i].to_str(g, true, true);
         wasOut = true;
      }
   }
   if (wasOut)
   {
      res += " -> ";
   }
   res += _literals[_trueLiteralIndex].to_str(g, false, true);
   return res;
}
