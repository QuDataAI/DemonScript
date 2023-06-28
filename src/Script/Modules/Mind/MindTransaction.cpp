#include "MindTransaction.h"

MindTransaction::MindTransaction(MindVarAxiomPtr axiom, UInt atomIndex, vector<UInt>& vals):
   _axiom(axiom),
   _atomIndex(atomIndex),
   _vals(vals)
{
}
