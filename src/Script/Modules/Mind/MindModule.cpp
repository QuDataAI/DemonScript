#include "MindModule.h"
#include "MindGraphModel.h"
#include "Modules/ModuleManager.h"
/****************************************************************************************
Функции Mind в DemonScript
*****************************************************************************************/

//регистрируем функции модуля
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncAdd,              "add")
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncAddUsual,         "add_usual")
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncOut,              "out")
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncSetGraph,         "set_graph")
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncSplitGraph,       "split_graph")
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncSplitGraphUsual,  "split_graph_usual")
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncSetMode,          "mode")
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncClear,            "clear")
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncVerbose,          "verbose")
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncValidator,        "validator")
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncGetModels,        "get_models")
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncGetUsualModels,   "get_usual_models")
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncMerge,            "merge")
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncCountModels,      "count_models")
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncValModels,        "value")
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncClearUsed,        "clear_used")
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncGroup,            "group")
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncDifferntVals,     "differnt_vals")
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncOutUndef,         "out_undef")
IMPLEMENTATION_MODULE_FUNCTION(Mind, MindFuncWasConflict,      "was_conflict")



//=======================================================================================
//
Value MindFuncAdd::run()
{
   for (size_t i = 0; i < _args.size(); i++)
      ((Mind*)_module)->add(_args[i]);

   return Value();
}

//=======================================================================================
//
Value MindFuncAddUsual::run()
{
   for (size_t i = 0; i < _args.size(); i++)
      ((Mind*)_module)->add_usual(_args[i]);

   return Value();
}

//=======================================================================================
//
Value MindFuncOut::run()
{
   TRACE_STREAM(tout);
   ((Mind*)_module)->print(tout);
   return Value();
}
//=======================================================================================
//
Value MindFuncSetGraph::run()
{
   if (_args.size() < 1 ) {
      TRACE_CRITICAL << "! MindFuncSetGraph::run> Mind.set_graph has wrong count of arguments (must be 1 or more)\n";
      return Logic::False;
   }
   Value g = _args[0]->run();
   if (g.type() != ValueBase::_GRAPH) {
      TRACE_CRITICAL << "! MindFuncSetGraph::run> Argument of Mind.set_graph is not is a graph\n";
      return Logic::False;
   }
   if (_args.size() == 1) 
      return Float(((Mind*)_module)->set_graph(g, true, -1));   
   if (_args.size() == 2) {
      int group = int(_args[1]->run().get_Float());
      return Float(((Mind*)_module)->set_graph(g, true, group));      
   }   

   vector<int> groups;                            // номера групп
   for (size_t i = 1; i < _args.size(); i++)
      groups.push_back(int(_args[i]->run().get_Float()));   
   Float tot = 0;
   for (size_t cicle = 0; cicle < 1000000; cicle++) {// \todo
      bool ok = true;
      for (auto group : groups) {
         int res = ((Mind*)_module)->set_graph(g, true, group);         
         if (res < 0) {
            TRACE << "! MindFuncSetGraph::run> In group " << group << " was false axiom\n";
            return tot;
         }
         tot += res;
         ok = ok && res == 0;
         if (res != 0)
            break;
      }
      if (ok)                                    // все группы не дали новых рёбер
         return tot;                           
   }
   TRACE << "! MindFuncSetGraph::run> Was max cicles in group set_graph\n";
   return tot;
}

//=======================================================================================
//
MindFuncSplitGraph::MindFuncSplitGraph()
{
   _minArgs = 2;
   _maxArgs = 4;
   _argTypes.resize(4);
   _argTypes[0].push_back(ValueBase::_GRAPH);
   _argTypes[1].push_back(ValueBase::_EDGE);
   _argTypes[1].push_back(ValueBase::_ARRAY);
   _argTypes[2].push_back(ValueBase::_FLOAT);
   _argTypes[3].push_back(ValueBase::_FLOAT);
}

Value MindFuncSplitGraph::run()
{
   if (!initRun())
      return Value();

   vector<Int> edges;
   if (_vals[1].type() == ValueBase::_EDGE)
      edges.push_back(_vals[1].get_Int());
   else {
      for (size_t i = 0; i < _vals[1].size(); i++)
         edges.push_back(_vals[1][i]->get_Int());
   }

   Float modelP = 1.0;
   if (_args.size() > 2)
      modelP = _vals[2].get_Float();

   bool saveLog = false;
   if (_args.size() > 3)
      saveLog = _vals[3].get_Float() > 0;

   MindGraphModelPtr model = SPTR_MAKE(MindGraphModel)(_vals[0], modelP);
   vector< MindGraphModelPtr > models;
   vector< shared_ptr<ValueBase> > modelVals;   

   ((Mind*)_module)->split_graph_model(model, edges, models, saveLog);

   for (auto &model : models)
      modelVals.push_back(model->get_Value().ptr());

   return Value(ValueArr(modelVals));
}

//=======================================================================================
//
MindFuncSplitGraphUsual::MindFuncSplitGraphUsual()
{
   _minArgs = 1;
   _maxArgs = 4;
   _argTypes.resize(4);
   _argTypes[0].push_back(ValueBase::_GRAPH);
   _argTypes[1].push_back(ValueBase::_FLOAT);
   _argTypes[2].push_back(ValueBase::_FLOAT);
   _argTypes[3].push_back(ValueBase::_FLOAT);
}

Value MindFuncSplitGraphUsual::run()
{
   if (!initRun())
      return Value();

   Float modelP = 1.0;
   if (_args.size() > 1)
      modelP = _vals[1].get_Float();

   UInt modelN = 1;
   if (_args.size() > 2)
      modelN = (UInt)_vals[2].get_Float();

   bool saveLog = false;
   if (_args.size() > 3)
      saveLog = _vals[3].get_Float() > 0;

   MindGraphModelPtr model = SPTR_MAKE(MindGraphModel)(_vals[0], modelP, modelN);
   vector< MindGraphModelPtr > models;
   vector< shared_ptr<ValueBase> > modelVals;

   ((Mind*)_module)->split_graph_model_with_usual_axioms(model, models, saveLog);

   for (auto &model : models)
      modelVals.push_back(model->get_Value().ptr());

   return Value(ValueArr(modelVals));
}

//=======================================================================================
//
Value MindFuncSetMode::run()
{
   if (_args.size() != 1) {
      TRACE_CRITICAL << "! MindFuncSetMode::run> Mind.set_mode has wrong count of arguments (must be 1)\n";
      return Logic::False;
   }
   Float v = _args[0]->run().get_Float();
   ((Mind*)_module)->_mode = int(v);
   return Logic::True;
}
//=======================================================================================
//
Value MindFuncVerbose::run()
{
   if (_args.size() != 1) {
      TRACE_CRITICAL << "! MindFuncVerbose::run> Mind.verbose has wrong count of arguments (must be 1)\n";
      return Logic::False;
   }
   Float v = _args[0]->run().get_Float();
   if (v < 0)
   {
      ((Mind*)_module)->_show_false = false;
   }
   else
   {
      ((Mind*)_module)->_verbose = int(v);
   }
   
   return Logic::True;
}
//=======================================================================================
//
Value MindFuncValidator::run()
{
   DCAST(Mind*,_module)->_validator = 0;
   if (_args.size() != 1) {
      TRACE_CRITICAL << "! MindFuncValidator::run> Mind.validator has wrong count of arguments (must be 1)\n";
      return Logic::False;
   }   
   ExprPtr ex = _args[0];
   if (ex->kind() != Expr::_FUN) {
      TRACE_CRITICAL << "! MindFuncValidator::run> Mind.validator arg must be demon\n";
      return Logic::False;
   }
   FunDemonPtr f = SPTR_DCAST(FunDemon,SPTR_DCAST(ExprFun,ex)->_fun);
   if (f->_vars.size() != 1 ) {
      TRACE_CRITICAL << "! MindFuncValidator::run> Mind.validator arg[0] must be demon with one arg of graph\n";
      return Logic::False;
   }
   DCAST(Mind*,_module)->_validator = f;
   return Logic::True;
}
//=======================================================================================
//
Value MindFuncClear::run()
{
   if (_args.size() == 0) {
      ((Mind*)_module)->clear();
   }
   return Logic::True;
}
//=======================================================================================
//
Value MindFuncClearUsed::run()
{
   Mind *m = (Mind*)_module;
   for (size_t i = 0; i < m->_axiomsAlways.size(); i++)
      m->_axiomsAlways[i]->_num_used = m->_axiomsAlways[i]->_num_checked = 0;            // сбрасываем число использований
   for (size_t i = 0; i < m->_axiomsUsual.size(); i++)
      m->_axiomsUsual[i]->_num_used = m->_axiomsUsual[i]->_num_checked = 0;            // сбрасываем число использований
   return Logic::True;
}
//=======================================================================================
//
Value MindFuncGetModels::run()
{
   if (_args.size() < 2) {
      TRACE_CRITICAL << "! MindFuncGetModels::run> Mind.get_models has wrong count of arguments\n";
      return Logic::False;
   }
   size_t count = 10;
   if (_args.size() >= 3)
      count = size_t(_args[2]->run().get_Float());
   Value v_edges = _args[1]->run();
   if (!(v_edges.type() == ValueBase::_EDGE || v_edges.type() == ValueBase::_ARRAY)) {
      TRACE_CRITICAL << "! MindFuncGetModels::run> Mind.get_models second arg must be edge or array of edges\n";
      return Logic::False;
   }
   vector<Int> edges;
   if (v_edges.type() == ValueBase::_EDGE)
      edges.push_back(v_edges.get_Int());
   else {
      for (size_t i = 0; i < v_edges.size(); i++)
         edges.push_back(v_edges[i]->get_Int());
   }
   vector< shared_ptr<ValueBase> > models;
   Value graphValue = _args[0]->run();
   ((Mind*)_module)->get_models(graphValue, edges, &models, count);
   return Value(ValueArr(models));
}

MindFuncGetUsualModels::MindFuncGetUsualModels()
{
   _minArgs = 2;
   _maxArgs = 5;
   _argTypes.resize(5);
   _argTypes[0].push_back(ValueBase::_GRAPH);
   _argTypes[1].push_back(ValueBase::_EDGE);
   _argTypes[1].push_back(ValueBase::_ARRAY);
   _argTypes[2].push_back(ValueBase::_FLOAT);
   _argTypes[3].push_back(ValueBase::_FLOAT);
   _argTypes[4].push_back(ValueBase::_FLOAT);
}

//=======================================================================================
//
Value MindFuncGetUsualModels::run()
{
   if (!initRun())
      return Value();

   size_t count = 10;
   if (_args.size() >= 3)
      count = size_t(_vals[2].get_Float());

   Value v_edges = _vals[1];
   if (!(v_edges.type() == ValueBase::_EDGE || v_edges.type() == ValueBase::_ARRAY)) {
      TRACE_CRITICAL << "! MindFuncGetUsualModels::run> Mind.get_usual_models second arg must be edge or array of edges\n";
      return Logic::False;
   }
   vector<Int> edges;
   if (v_edges.type() == ValueBase::_EDGE)
      edges.push_back(v_edges.get_Int());
   else {
      for (size_t i = 0; i < v_edges.size(); i++)
         edges.push_back(v_edges[i]->get_Int());
   }

   Value graphValue = _vals[0];
   Float minP = 0.0;
   Float maxP = 1.0;

   if (_vals.size() > 3)
      minP = _vals[3].get_Float();
   if (_vals.size() > 4)
      maxP = _vals[4].get_Float();

   MindGraphModelPtr model = SPTR_MAKE(MindGraphModel)(graphValue, 1.0);
   vector< MindGraphModelPtr > models;
   vector< shared_ptr<ValueBase> > modelVals;

   ((Mind*)_module)->get_usual_models(model, edges, models, count, minP, maxP);

   for (auto &model : models)
      modelVals.push_back(model->get_Value().ptr());

   return Value(ValueArr(modelVals));
}

MindFuncMerge::MindFuncMerge()
{
   _minArgs = 2;
   _maxArgs = 3;
   _argTypes.resize(3);
   _argTypes[0].push_back(ValueBase::_GRAPH);
   _argTypes[1].push_back(ValueBase::_GRAPH);
   _argTypes[2].push_back(ValueBase::_ARRAY);
}

Value MindFuncMerge::run()
{
   if (!initRun())
      return Value();

   Value excludeEdges;
   if (_vals.size() > 2)
   {
      excludeEdges = _vals[2];
   }

   ((Mind*)_module)->merge(_vals[0], _vals[1], excludeEdges);

   return Value();
}

//=======================================================================================
//
Value MindFuncCountModels::run()
{
   if (_args.size() < 2) {
      TRACE_CRITICAL << "! MindFuncGetModels::run> Mind.count_models has wrong count of arguments\n";
      return Value(Float(0));
   }
   Value v_edges = _args[1]->run();
   if (!(v_edges.type() == ValueBase::_EDGE || v_edges.type() == ValueBase::_ARRAY)) {
      TRACE_CRITICAL << "! MindFuncGetModels::run> Mind.count_models second arg must be edge or array of edges\n";
      return Value(Float(0));
   }
   vector<Int> edges;
   if (v_edges.type() == ValueBase::_EDGE)
      edges.push_back(v_edges.get_Int());
   else {
      for (size_t i = 0; i < v_edges.size(); i++)
         edges.push_back(v_edges[i]->get_Int());
   }
   Value graphValue = _args[0]->run();
   Float cnt = (Float)((Mind*)_module)->get_models(graphValue, edges, 0, 0);
   return Value(cnt);
}
//=======================================================================================
// Вычисляем значение выржения по всем моделям. Если оно одно и тоже, то возвращаем его
// Иначе возвращем Undef. Учитывает возможность многозначности логических значений.
//
Value MindFuncValModels::run()
{
   if (_args.size() < 2) {
      TRACE_CRITICAL << "! MindFuncValModels::run> Mind.val_models has wrong count of arguments (must be 2)\n";
      return Logic::Undef;
   }
   Value models = _args[0]->run();
   if ( models.type() != ValueBase::_ARRAY ) {
      TRACE_CRITICAL << "! MindFuncValModels::run> Mind.val_models first arg must be array of graphs\n";
      return Logic::Undef;
   } 
   if (models.size()==0)
      return Logic::Undef;

   ExprPtr expr = _args[1];   
   CurrentGraph currentGraph; // текущий граф

   bool  n_true = false, n_false = false,  n_other = false;
   Logic other = Logic::Undef;
   for (size_t i = 0; i < models.size(); i++) {
      Value g = models[i];
      if (g.type() != ValueBase::_GRAPH) {
         TRACE_CRITICAL << "! MindFuncValModels::run> Mind.val_models all elements of first arg must be graph\n";
         return Logic::Undef;
      }      
      currentGraph.set(g.ptr());
      Value val = expr->run();                                            // вычисляем выражение                           
      if (val.type() != ValueBase::_LOGIC) {   
         TRACE_CRITICAL << "! MindFuncValModels::run> Mind.val_models second arg must return logic value\n";
         return Logic::Undef;
      }
                  
      Logic v = val.get_Logic();
      if (v == Logic::Undef) {
         return Logic::Undef;
      }
      else if (v == Logic::True) {
         n_true = true;
         if (n_false || n_other) {
            return Logic::Undef;
         }
      }
      else if (v == Logic::False) {
         n_false = true;
         if (n_true || n_other) {
            return Logic::Undef;
         }
      }
      else {
         n_other = true;
         if (n_true || n_false || other != v) {
            return Logic::Undef;
         }
         other = v;
      }
   }

   if(n_true)
      return Logic::True;
   if (n_false)
      return Logic::False;
   return other;
}
//=======================================================================================
//
Value MindFuncGroup::run()
{
   if (_args.size() != 1) {
      TRACE_CRITICAL << "! MindFuncGroup::run> Mind.group has wrong count of arguments (must be 1)\n";
      return Logic::False;
   }
   Float v = _args[0]->run().get_Float();
   ((Mind*)_module)->_group = int(v);
   return Logic::True;
}
//=======================================================================================
//
Value MindFuncDifferntVals::run()
{
   if (_args.size() > 0) {
      Float v = _args[0]->run().get_Float();
      ((Mind*)_module)->_differnt_vals = int(v);
   }
   return Float(((Mind*)_module)->_differnt_vals);
}
//=======================================================================================
//
Value MindFuncOutUndef::run()
{
   if (_args.size() > 0) {
      Float v = _args[0]->run().get_Float();
      ((Mind*)_module)->_out_undef = int(v);
   }
   return Float(((Mind*)_module)->_out_undef);
}

//=======================================================================================
//
Value MindFuncWasConflict::run()
{
   return Logic(((Mind*)_module)->_was_conflict);
}

