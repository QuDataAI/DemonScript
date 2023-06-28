#include "ValueEdge.h"
#include "GraphManager.h"

string ValueEdge::toString()
{
   string fullName = "";
   fullName = GraphManager::instance().currentGraph()->edge_name(_val);
   //fullName += " (" + to_string(_val) + ")";
   return fullName;
}
