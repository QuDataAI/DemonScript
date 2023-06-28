#include "ValueBase.h"

string ValueBase::type_str(Kind type)
{
   switch (type)
   {
      case _NONE: return "NONE";  break;
      case _FLOAT:return "FLOAT"; break;
      case _STR:  return "STR";   break;
      case _LOGIC:return "LOGIC"; break;
      case _NODE: return "NODE";  break;
      case _EDGE: return "EDGE";  break;
      case _ARRAY:return "ARRAY"; break;
      case _GRAPH:return "GRAPH"; break;
      case _FILE: return "FILE"; break;
      case _FUSNUMBER: return "FUSNUMBER"; break;
      case _MAP: return "MAP"; break;
      case _ANY:  return "ANY";   break;
      default: return "UNKNOWN";
   }
}

Logic ValueBase::eq(ValueBase * v)
{
   CHECK_TYPES(v, Logic());
   return Logic();
}

Logic ValueBase::neq(ValueBase * v)
{
   CHECK_TYPES(v, Logic());
   return Logic();
}

Logic ValueBase::lt(ValueBase * v)
{
   CHECK_TYPES(v, Logic());
   return Logic();
}

Logic ValueBase::lteq(ValueBase * v)
{
   CHECK_TYPES(v, Logic());
   return Logic();
}

Logic ValueBase::gt(ValueBase * v)
{
   CHECK_TYPES(v, Logic());
   return Logic();
}

Logic ValueBase::gteq(ValueBase * v)
{
   CHECK_TYPES(v, Logic());
   return Logic();
}

Logic ValueBase::or_(ValueBase * v)
{
   CHECK_TYPES(v, Logic());
   return Logic();
}

Logic ValueBase::and_(ValueBase * v)
{
   CHECK_TYPES(v, Logic());
   return Logic();
}

Logic ValueBase::impl(ValueBase * v)
{
   CHECK_TYPES(v, Logic());
   return Logic();
}
