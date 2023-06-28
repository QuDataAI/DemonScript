#include "ValueLambda.h"
#include "Script/Expr.h"

ValueLambda::ValueLambda(vector<string>& vars, ScriptPtr script)
{
   _val = SPTR_MAKE(FunLambda)(vars, script);
}

ostream & ValueLambda::print(ostream & out)
{
   return _val->print(out);
}

string ValueLambda::toString()
{
   stringstream str;
   _val->print(str);
   return str.str();
}
