#include "GraphInvariants.h"

bool GraphInvariants::operator==(const GraphInvariants& other)
{
   if (vertexes != other.vertexes)
      return false;

   if (edges != other.edges)
      return false;

   if (vertexesDegree.size() != other.vertexesDegree.size())
      return false;

   if (kindTypes.size() != other.kindTypes.size())
      return false;

   for (int i = 0; i < vertexesDegree.size(); i++)
   {
      if (vertexesDegree[i] != other.vertexesDegree[i])
         return false;
   }

   for (int i = 0; i < kindTypes.size(); i++)
   {
      if (kindTypes[i] != other.kindTypes[i])
         return false;
   }

   return true;
}
