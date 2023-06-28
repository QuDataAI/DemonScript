#include "MindNames.h"
#include "Expr.h"

//------------------------------------------------------------------------------------
//! ѕолучить идентификатор переменной или константы. Ќомера переменных идут с 1
//
int MindNames::get(ExprPtr ex)
{
   if (ex->kind() == Expr::_NODE_CONST || ex->kind() == Expr::_FIELD || ex->kind() == Expr::_CONST)
      return -get(ex->run());                 // минус - это константа: 0,-1,-2,...

   if (ex->kind() == Expr::_VAR)
      return  get(SPTR_DCAST(ExprVar, ex)->_var) + 1; //  переменные с единицы: 1,2,3

   TRACE_CRITICAL << "! MindNames::get> wrong kind expression : " << ex->kind() << endl;
   return 0;
}