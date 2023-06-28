#include "Expr.h"
#include "Function.h"
#include "Script.h"
#include "ValueNode.h"
#include "GraphManager.h"

Value ExprValueField::run()
{
#ifdef _DS_CHECK_POINTER
   if (!_valExpr)
      return Logic();
#endif

   Value val       = _valExpr->run();
   if (val.type() == ValueBase::_GRAPH)
   {
      if (_fieldEvaluated && _field.type() == ValueBase::_NODE)
      {        
         Value fieldVal = val.field(_field);
         return fieldVal;
      }
      else
      {
         //выражение _fieldExpr должно выполнятся в контексте графа который стоит слева от =>
         //поэтому ниже импровизация стека, затем результат кешируется если справа от => была константа
         CurrentGraph currentGraph(val); 
         Value field = _fieldExpr->run();
         currentGraph.restore();
         Value fieldVal = val.field(field);
         if (_fieldExpr->kind() == _NODE_CONST)
         {
            //для констант идентификаторы узлов всегда кешируем, так как изменяться не могут
            _field = field;
            _fieldEvaluated = true;
         }
         return fieldVal;
      }
   }

   return Value();
}

Value ExprNodeConst::run()
{
   if (_evaluated)
      return _val;

   Graph*   graph   = GraphManager::instance().currentGraph();   
   UInt     classID = graph->classID();
   UInt     nodeID  = GraphManager::instance().node(classID, _name);

   if (nodeID == 0)
   {
      if (graph->_valDefSensesGraph)
      {
         //узел может бать в графе смыслов по умолчанию для текущего графа
         classID = graph->_valDefSensesGraph->get_GraphClassID();
         nodeID  = GraphManager::instance().node(classID, _name);
      }
      if (nodeID == 0)
      {
         TRACE_CRITICAL << "node with name " << _name << " is not found in current graph class";
         return Value();
      }
      _val = Value(ValueNode(graph->_valDefSensesGraph, nodeID));
   }
   else
   {
      _val = Value(ValueNode(classID, nodeID));
   }   
   _evaluated = true;

   return _val;
}
//=======================================================================================
//                                    ExprArray
//=======================================================================================
// Вернуть значения массива
//
Value ExprArray::run()
{
   vector<Value> arr;
   for (UInt i = 0; i < _exprs.size(); i++)
      if (_exprs[i])
         arr.push_back(_exprs[i]->run());
   return arr;
}
//=======================================================================================
// Вывести массив в поток, в виде текста программы
//
ostream& ExprArray::print(ostream& out)
{
   out << "[";
   for (UInt i = 0; i < _exprs.size(); i++) {
      if (_exprs[i])
         _exprs[i]->print(out);
      if (i + 1 < _exprs.size())
         out << ", ";
   }
   return out << "]";
}
//=======================================================================================
//                                    ExprFun
//=======================================================================================
// Получить функцию
//
FunctionPtr  ExprFun::getFunction()
{
   if (_fun)
      return _fun;

   //возможно внутри _obj лежит лямбда-функция
   Value objVal = _obj->run();

   if (objVal.type() != ValueBase::_LAMBDA)
      return nullptr;

   return objVal.get_Lambda();
}
//=======================================================================================
// Вернуть значение функции
//
Value ExprFun::run()
{
   FunctionPtr fun = getFunction();
   if (!fun)
      return Value();

   return fun->run(_args, _obj);                   // вычисляем значение функции
}

Value ExprFun::set(const Value & v)
{
   FunctionPtr fun = getFunction();
   if (!fun)
      return Value();

   return fun->set(_args, _obj, v);                 // устанавливаем значение в функцию
}
//=======================================================================================
// Вывести функцию в поток, в виде текста (программы)
//
ostream& ExprFun::print(ostream& out)
{
   if (_fun == 0)
      return out << "???(???)";

   if (_obj)
      _obj->print(out);
   out << _fun->_name << "(";                     // имя функции
   for (UInt i = 0; i < _args.size(); i++)           // аргументы (выражения)
      if (_args[i]) {
         _args[i]->print(out);
         out << ((i + 1 < _args.size()) ? ", " : "");
      }
   return out << ")";
}

Value ExprEquality::run()
{
   Value leftVal  = _lf->run();
   Value rightVal = _rt->run();

   Value res = ((!leftVal) | (rightVal)) & ((leftVal) | (!rightVal));

   return res;
}

Value ExprValElement::run()
{
#ifdef _DS_CHECK_POINTER
   if (!_expr)
      return Logic();
#endif
   if (!_indx)                                    // нет индекса - просто значение
      return _expr->run();

   Value exprVal = _expr->run();
   Value indxVal = _indx->run();

   if (exprVal.type() == ValueBase::_MAP)
   {
      return exprVal[indxVal];     
   }
   else if (indxVal.type() == ValueBase::_NODE)
   {
      //установка атрибута узла
      Graph* obGraph = exprVal.get_Graph();
      UInt   obNodeID = exprVal.get_UInt();
      Graph* attrGraph = indxVal.get_Graph();
      UInt   attrNodeID = indxVal.get_UInt();
      return obGraph->get_attr(obNodeID, attrNodeID, attrGraph, false);
   }
   else if (indxVal.type() == ValueBase::_FLOAT)
   {
      //индексный доступ к массиву, либо логическому значению
      if (
         exprVal.type() != ValueBase::_ARRAY &&
         exprVal.type() != ValueBase::_LOGIC &&
         exprVal.type() != ValueBase::_GRAPH &&
         exprVal.type() != ValueBase::_FUSNUMBER &&
         exprVal.type() != ValueBase::_STR
         )
      {
         TRACE_CRITICAL << "Incorrect type of value (ARRAY or LOGIC or GRAPH or FUSNUMBER or STRING is expected)" << endl;
         return Logic();
      }

      ValueIndexRef ref = exprVal[indxVal];

      return exprVal[indxVal];     // индексный доступ (массивы, логика, граф)
   }
   else
   {
      TRACE_CRITICAL << "Incorrect type of index" << endl;
      return Logic();
   }
}

Value ExprValElement::set(const Value & v)
{
#ifdef _DS_CHECK_POINTER
   if (!_expr)
      return Logic();
#endif
   if (!_indx)                                    // нет индекса - просто значение
      return _expr->run();

   Value exprVal = _expr->run();
   Value indxVal = _indx->run();

   if (exprVal.type() == ValueBase::_MAP)
   {
      return exprVal[indxVal] = v;
   }
   else if (indxVal.type() == ValueBase::_NODE)
   {
      //установка атрибута узла
      if (indxVal.type() != ValueBase::_NODE)
      {
         TRACE_CRITICAL << "Incorrect type of index (NODE expected)";
         return Logic();
      }
      Graph* obGraph    = exprVal.get_Graph();
      UInt   obNodeID   = exprVal.get_UInt();
      Graph* attrGraph  = indxVal.get_Graph();
      UInt   attrNodeID = indxVal.get_UInt();
      GraphManager::instance().setAttr(obGraph->classID(), obNodeID, obGraph, attrNodeID, attrGraph, v.ptr());
      return Logic::True;
   } 
   else if (indxVal.type() == ValueBase::_FLOAT)
   {
      //индексный доступ к массиву, либо логическому значению
      if (
         exprVal.type() != ValueBase::_ARRAY     &&
         exprVal.type() != ValueBase::_LOGIC     &&
         exprVal.type() != ValueBase::_FUSNUMBER &&
         exprVal.type() != ValueBase::_STR 
         )
      {
         TRACE_CRITICAL << "Incorrect type of value (ARRAY or LOGIC or FUSNUMBER or STRING expected)" << endl;
         return Logic();
      }
      return exprVal[indxVal] = v;
   }
   else
   {
      TRACE_CRITICAL << "Incorrect type of index";
      return Logic();
   }
}

Value ExprMultiary::run()
{
   Value cond = _condExpr->run();
   Logic res  = cond.get_Logic();
   if (_forthArity == 0)
   {
      //тернарный вид      E ? : A : B - если E истина, то A, иначе B
      if (res == Logic::True)
         return _secondArity->run();
      else
         return _thirdArity->run();     
   }
   else if (_fifthArity == 0)
   {
      //кватернарный вид   E ? : A : B : C - если E истина, то A, если E ложь, то B, иначе С
      if (res == Logic::True)
         return _secondArity->run();
      else if (res == Logic::False)
         return _thirdArity->run();
      else 
         return _forthArity->run();
   }
   else
   {
      //квантиринарный вид E ? : A : B : C : D - если E истина, то A, если E ложь, то B, если E не определено, то С, иначе D
      if (res == Logic::True)
         return _secondArity->run();
      else if (res == Logic::False)
         return _thirdArity->run();
      else if (res == Logic::Undef)
         return _forthArity->run();
      else
         return _fifthArity->run();
   }
   return Value();
}

ostream & ExprMultiary::print(ostream & out)
{
   out << "(";
   if (_condExpr) _condExpr->print(out); out << " ? "; 
   if (_secondArity) _secondArity->print(out);
   if (_thirdArity)
   {
      out << " : ";
      _thirdArity->print(out);
   }
   if (_forthArity)
   {
      out << " : ";
      _forthArity->print(out);
   }
   if (_fifthArity)
   {
      out << " : ";
      _fifthArity->print(out);
   }
   out << ")";
   return out;
}
//=======================================================================================
//                                    ExprMap
//=======================================================================================
// Вернуть значения мэпа
//
Value ExprMap::run()
{
   shared_ptr<ValueMap> map = make_shared<ValueMap>();
   for (UInt i = 0; i < _exprs.size(); i++)
   {
      Value key = _exprs[i]._keyExpr->run();
      if (map->hasKey(key.ptr()))
      {
         TRACE_CRITICAL << "key " << key << " already exists" << endl;
         continue;
      }
      map->setArrVal(key.ptr(), _exprs[i]._valExpr->run().ptr());
   }
   return Value(map);
}
//=======================================================================================
// Вывести функцию в поток, в виде текста программы
//
ostream & ExprMap::print(ostream & out)
{
   out << "{";
   for (UInt i = 0; i < _exprs.size(); i++) {
      _exprs[i]._keyExpr->print(out);
      out << ":";
      _exprs[i]._valExpr->print(out);
      if (i + 1 < _exprs.size())
         out << ", ";
   }
   return out << "}";
}

Value ExprEdge::run()
{
   if (_root)
      return set(_val);

   Value edge = _edgeExpr->run();
#ifdef _DS_CHECK_TYPE
   if (edge.type() != ValueBase::_EDGE
      ) {
      TRACE_CRITICAL << "! .set_edge: edge has incorrect type (EDGE is expected)\n";
      return Logic();
   }
#endif
   Int edgeID = edge.get_Int();
   Logic res = Logic::True;

   /*в узле могут быть:
   - константа a in b
   - отрицание a in !b или !с in b
   - массив c отрицаниями и неопределенностями a in [b,!c,?d] или [b,!c,?d] in a
   */
   if (_node1Expr->kind() == _NOT)
   {
      //- отрицание !a in b
      Value node1 = SPTR_DCAST(ExprNot, _node1Expr)->_expr->run();
      res = getExprEdge(edgeID, node1, _node2Expr);
      res = !res;
   }
   else if (_node1Expr->kind() == _UNDEF)
   {
      //- отрицание ?a in b
      Value node1 = SPTR_DCAST(ExprUndef,_node1Expr)->_expr->run();
      res = getExprEdge(edgeID, node1, _node2Expr);
      res = res == Logic::Undef;
   }
   else if (_node1Expr->kind() == _ARRAY)
   {
      //массив в котором могут быть отрицания типа: [b,!c,d,e] in a
      ExprArrayPtr node1ExprArray = SPTR_DCAST(ExprArray, _node1Expr);
      for (auto expr : node1ExprArray->_exprs)
      {
         if (expr->kind() == _NOT)
         {
            Value node1 = SPTR_DCAST(ExprNot,expr)->_expr->run();
            Logic itemRes = Logic::True;
            if (node1.type() == ValueBase::_NODE)
            {
               itemRes = getExprEdge(edgeID, node1, _node2Expr);
            }
            res = res && (!itemRes);
         }
         else if (expr->kind() == _UNDEF)
         {
            Value node1 = SPTR_DCAST(ExprUndef,expr)->_expr->run();
            Logic itemRes = Logic::True;
            if (node1.type() == ValueBase::_NODE)
            {
               itemRes = getExprEdge(edgeID, node1, _node2Expr);
            }
            res = res && (itemRes == Logic::Undef);
         }
         else
         {
            Value node1 = expr->run();
            Logic itemRes = getExprEdge(edgeID, node1, _node2Expr);
            res = res && itemRes;
         }
      }
   }
   else
   {
      Value node1 = _node1Expr->run();
      res = getExprEdge(edgeID, node1, _node2Expr);
   }

   if (_val == Logic::False)
   {
      res = !res;
   }
   else if (_val == Logic::Undef)
   {
      res = res == Logic::Undef;
   }

   return res;
}

Value ExprEdge::set(const Value & val)
{
   Value edge  = _edgeExpr->run();   
#ifdef _DS_CHECK_TYPE
   if (edge.type() != ValueBase::_EDGE
      ) {
      TRACE_CRITICAL << "! .set_edge: edge has incorrect type (EDGE is expected)\n";
      return Logic();
   }
   if (val.type() != ValueBase::_LOGIC
      ) {
      TRACE_CRITICAL << "! .set_edge: value has incorrect type (LOGIC is expected)\n";
      return Logic();
   }
#endif
   Int edgeID = edge.get_Int();
   /*в узле могут быть:
   - константа a in b
   - отрицание a in !b или !с in b
   - массив c отрицаниями a in [b,!c,d,e] или [b,!c] in a 
   */
   if (_node1Expr->kind() == _NOT)
   {
      //- отрицание !a in b
      Value node1 = SPTR_DCAST(ExprNot,_node1Expr)->_expr->run();
      Value setVal = !val;
      setExprEdge(edgeID, node1, _node2Expr, setVal);
      return setVal;
   }
   else if (_node1Expr->kind() == _UNDEF)
   {
      //- отрицание ?a in b
      Value node1 = SPTR_DCAST(ExprUndef,_node1Expr)->_expr->run();
      Value setVal = Logic::Undef;
      setExprEdge(edgeID, node1, _node2Expr, setVal);
      return setVal;
   }
   else if (_node1Expr->kind() == _ARRAY)
   {
      //массив в котором могут быть отрицания типа: [b,!c,d,e] in a
      ExprArrayPtr node1ExprArray = SPTR_DCAST(ExprArray,_node1Expr);
      for (auto expr : node1ExprArray->_exprs)
      {
         if (expr->kind() == _NOT)
         {
            Value node1 = SPTR_DCAST(ExprNot,expr)->_expr->run();
            Value setVal = !val;
            if (node1.type() == ValueBase::_NODE)
            {
               setExprEdge(edgeID, node1, _node2Expr, setVal);
            }
         }
         else
         {
            Value node1 = expr->run();
            setExprEdge(edgeID, node1, _node2Expr, val);
         }
      }
      return val;
   }

   Value node1 = _node1Expr->run();
   setExprEdge(edgeID, node1, _node2Expr, val);
   return val;
}

Value ExprEdge::setExprEdge(Int edge, Value & node1, ExprPtr node2Expr, const Value & val)
{
   if (_node2Expr->kind() == _NOT)
   {
      //- отрицание a in !b
      Value node2 = (SPTR_DCAST(ExprNot,_node2Expr))->_expr->run();
      Value setVal = !val;
      if (node2.type() == ValueBase::_NODE)
      {
         if (node1.type() == ValueBase::_NODE)
         {
            setEdge(edge, node1, node2, setVal);
         }
         else if (node1.type() == ValueBase::_ARRAY)
         {
            for (UInt i = 0; i < node1.size(); i++)
            {
               Value node1Item = node1[i];
               setEdge(edge, node1Item, node2, setVal);
            }
            return val;
         }
      }
      return setVal;
   }
   else if (_node2Expr->kind() == _UNDEF)
   {
      //- отрицание a in ?b
      Value node2 = SPTR_DCAST(ExprUndef,_node2Expr)->_expr->run();
      Value setVal = Logic::Undef;
      if (node2.type() == ValueBase::_NODE)
      {
         if (node1.type() == ValueBase::_NODE)
         {
            setEdge(edge, node1, node2, setVal);
         }
         else if (node1.type() == ValueBase::_ARRAY)
         {
            for (UInt i = 0; i < node1.size(); i++)
            {
               Value node1Item = node1[i];
               setEdge(edge, node1Item, node2, setVal);
            }
            return val;
         }
      }
      return setVal;
   }
   else if (_node2Expr->kind() == _ARRAY)
   {
      //массив в котором могут быть отрицания типа: a in [b,!c,d,e]
      ExprArrayPtr node2ExprArray = SPTR_DCAST(ExprArray,_node2Expr);
      for (auto expr : node2ExprArray->_exprs)
      {
         if (expr->kind() == _NOT)
         {
            Value node2 = SPTR_DCAST(ExprNot,expr)->_expr->run();
            Value setVal = !val;
            if (node2.type() == ValueBase::_NODE)
            {
               setEdge(edge, node1, node2, setVal);
            }
         }
         else if (expr->kind() == _UNDEF)
         {
            Value node2 = SPTR_DCAST(ExprUndef, expr)->_expr->run();
            Value setVal = Logic::Undef;
            if (node2.type() == ValueBase::_NODE)
            {
               setEdge(edge, node1, node2, setVal);
            }
         }
         else
         {
            Value node2 = expr->run();
            setEdge(edge, node1, node2, val);
         }
      }
      return val;
   }
   Value node2 = _node2Expr->run();
   setEdge(edge, node1, node2, val);

   return val;
}

Logic ExprEdge::getExprEdge(Int edge, Value & node1, ExprPtr node2Expr)
{
   Logic res = Logic::True;
   if (_node2Expr->kind() == _NOT)
   {
      //- отрицание a in !b
      Value node2 = SPTR_DCAST(ExprNot, _node2Expr)->_expr->run();
      if (node2.type() == ValueBase::_NODE)
      {
         if (node1.type() == ValueBase::_NODE)
         {
            res = res && getEdge(edge, node1, node2);
         }
         else if (node1.type() == ValueBase::_ARRAY)
         {
            for (UInt i = 0; i < node1.size(); i++)
            {
               Value node1Item = node1[i];
               res = res && getEdge(edge, node1Item, node2);
            }
         }
      }
      res = !res;
   }
   else if (_node2Expr->kind() == _UNDEF)
   {
      //- отрицание a in ?b
      Value node2 = SPTR_DCAST(ExprUndef, _node2Expr)->_expr->run();
      if (node2.type() == ValueBase::_NODE)
      {
         if (node1.type() == ValueBase::_NODE)
         {
            res = res && getEdge(edge, node1, node2);
         }
         else if (node1.type() == ValueBase::_ARRAY)
         {
            for (UInt i = 0; i < node1.size(); i++)
            {
               Value node1Item = node1[i];
               res = res && getEdge(edge, node1Item, node2);
            }
         }
      }
      res = res == Logic::Undef;
   }
   else if (_node2Expr->kind() == _ARRAY)
   {
      //массив в котором могут быть отрицания типа: a in [b,!c,d,e]
      ExprArrayPtr node2ExprArray = SPTR_DCAST(ExprArray,_node2Expr);
      for (auto expr : node2ExprArray->_exprs)
      {
         if (expr->kind() == _NOT)
         {
            Value node2 = SPTR_DCAST(ExprNot, expr)->_expr->run();
            Logic itemVal = Logic::Undef;
            if (node2.type() == ValueBase::_NODE)
            {
               itemVal = getEdge(edge, node1, node2);
            }
            res = res && (!itemVal);
         }
         else if (expr->kind() == _UNDEF)
         {
            Value node2 = SPTR_DCAST(ExprUndef,expr)->_expr->run();
            Logic itemVal = Logic::Undef;
            if (node2.type() == ValueBase::_NODE)
            {
               itemVal = getEdge(edge, node1, node2);
            }
            res = res && (itemVal == Logic::Undef);
         }
         else
         {
            Value node2 = expr->run();
            res = res && getEdge(edge, node1, node2);
         }
      }
   }
   else
   {
      Value node2 = _node2Expr->run();
      res = getEdge(edge, node1, node2);
   }
   return res;
}

Value ExprEdge::setEdge(Int edge, Value & node1, Value & node2, const Value & val)
{
   if (node1.type() != ValueBase::_NODE  &&
       node1.type() != ValueBase::_ARRAY) {
       TRACE_CRITICAL << "! .set_edge: node 1 has incorrect type (NODE or ARRAY is expected)\n";
       return Logic();
   }
   if (node2.type() != ValueBase::_NODE  &&
       node2.type() != ValueBase::_ARRAY) {
       TRACE_CRITICAL << "! .set_edge: node 2 has incorrect type (NODE or ARRAY is expected)\n";
       return Logic();
   }
   Logic valLogic = Value(val).get_Logic();  //душим сonst TODO get_Logic - const 

   if (node1.type() == ValueBase::_NODE)
   {
      if (node2.type() == ValueBase::_NODE)
      {
         node1.get_Graph()->add(edge, node1.get_UInt(), node2.get_UInt(), valLogic, node2.get_Graph());
         return val;
      }
      else if (node2.type() == ValueBase::_ARRAY)
      {
         for (UInt i = 0; i < node2.size(); i++)
         {
            Value node2Item = node2[i];
            node1.get_Graph()->add(edge, node1.get_UInt(), node2Item.get_UInt(), valLogic, node2Item.get_Graph());
         }
         return val;
      }
      return Logic();
   }
   else if (node1.type() == ValueBase::_ARRAY)
   {
      for (UInt i = 0; i < node1.size(); i++)
      {
         Value node1Item = node1[i];
         if (node2.type() == ValueBase::_NODE)
         {
            node1Item.get_Graph()->add(edge, node1Item.get_UInt(), node2.get_UInt(), valLogic, node2.get_Graph());
         }
         else if (node2.type() == ValueBase::_ARRAY)
         {
            for (UInt i = 0; i < node2.size(); i++)
            {
               Value node2Item = node2[i];
               node1Item.get_Graph()->add(edge, node1Item.get_UInt(), node2Item.get_UInt(), valLogic, node2Item.get_Graph());
            }
         }
      }
      return val;
   }

   return val;
}

Logic ExprEdge::getEdge(Int edge, Value & node1, Value & node2)
{
   if (node1.type() != ValueBase::_NODE  &&
      node1.type() != ValueBase::_ARRAY) {
      TRACE_CRITICAL << "! .getEdge: node 1 has incorrect type (NODE or ARRAY is expected)\n";
      return Logic();
   }
   if (node2.type() != ValueBase::_NODE  &&
      node2.type() != ValueBase::_ARRAY) {
      TRACE_CRITICAL << "! .getEdge: node 2 has incorrect type (NODE or ARRAY is expected)\n";
      return Logic();
   }   
   if (node1.type() == ValueBase::_NODE)
   {
      Graph * node1Graph = node1.get_Graph();

      if (!node1Graph)
         return Logic::Undef;

      if (node2.type() == ValueBase::_NODE)
      {
         Logic res = node1Graph->val(edge, node1.get_UInt(), node2.get_UInt(), node1Graph, node2.get_Graph());

         return res;
      }
      else if (node2.type() == ValueBase::_ARRAY)
      {
         Logic res = Logic::True;
         for (UInt i = 0; i < node2.size(); i++)
         {
            Value node2Item = node2[i];
            res = res && node1Graph->val(edge, node1.get_UInt(), node2Item.get_UInt(), node1Graph, node2Item.get_Graph());
         }
         return res;
      }      
   }
   else if (node1.type() == ValueBase::_ARRAY)
   {
      Logic res = Logic::True;
      for (UInt i = 0; i < node1.size(); i++)
      {
         Value node1Item = node1[i];

         Graph * node1Graph = node1Item.get_Graph();

         if (!node1Graph)
            return Logic::Undef;

         if (node2.type() == ValueBase::_NODE)
         {
            res = res && node1Graph->val(edge, node1Item.get_UInt(), node2.get_UInt(), node1Graph, node2.get_Graph());
         }
         else if (node2.type() == ValueBase::_ARRAY)
         {
            for (UInt i = 0; i < node2.size(); i++)
            {
               Value node2Item = node2[i];
               res = res && node1Graph->val(edge, node1Item.get_UInt(), node2Item.get_UInt(), node1Graph, node2Item.get_Graph());
            }
         }
      }
      return res;
   }

   return Logic();
}


Value ExprVarPlusAssign::run()
{
   Value arg = _expr->run();
   _var->val() = _var->val() + arg;
   return _var->val();
}

Value ExprVarMinusAssign::run()
{
   Value arg = _expr->run();
   _var->val() = _var->val() - arg;
   return _var->val();
}

Value ExprVarDotAssign::run()
{
   Value arg = _expr->run();
   _var->val() = _var->val() * arg;
   return _var->val();
}

Value ExprVarDivFloatAssign::run()
{
   Value arg = _expr->run();
   _var->val() = _var->val() / arg;
   return _var->val();
}

Value ExprVarDivIntAssign::run()
{
   Value arg = _expr->run();
   _var->val() = _var->val().divInt(arg);
   return _var->val();
}

Value ExprVarModAssign::run()
{
   Value arg = _expr->run();
   _var->val() = _var->val() % arg;
   return _var->val();
}

Value ExprImpl::run()
{
   if (_root)
      return set(Logic::True);

   return _lf->run().impl(_rt->run());
}

Value ExprImpl::set(const Value & v)
{
   GraphManager::instance().currentGraph()->addAxiom(SPTR_FROM_THIS);
   return Value();
}

Value ExprOr::run()
{
   if (_root)
      return set(Value());

   Value x = _lf->run(), y = _rt->run();
#ifdef _DS_CHECK_TYPE
   if (wrong_args(x, y) || (y.type() == ValueBase::_FLOAT && y.get_Float() == 0))
      return Float(0);
#endif 
   return x | y;
}

Value ExprOr::set(const Value & v)
{
   GraphManager::instance().currentGraph()->addAxiom(SPTR_FROM_THIS);
   return Value();
}
