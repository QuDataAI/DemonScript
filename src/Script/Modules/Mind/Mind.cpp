#include "Mind.h"
#include "MindGraphModel.h"
#include "MindMerge.h"
#include "MindAxioms.h"
#include "MindState.h"
#include "Modules/ModuleManager.h"
#include "Stack/StackFrame.h"

//регистрируем модуль
IMPLEMENTATION_MODULE(Mind, "Mind")

/****************************************************************************************
*                             Mind  - парсинг, печать                                   *
*****************************************************************************************/
Mind::Mind():
   _max_vars(0),
   _usualSorted(false),
   _mode(1),
   _verbose(0),
   _validator(0),
   _changes(0),
   _differnt_vals(0),
   _out_undef(0),
   _num_cycles(0),
   _maxAxiomIndex(0),
   _show_false(true),
   _was_conflict(false)
{
}

// Добавить аксиому, преобразовав её в КНФ
//
void Mind::add(ExprPtr ex)
{
   MindVarAxiomPtr ax = SPTR_MAKE(MindVarAxiom)();
   ax->_group = _group;                         
   _names._vars.clear();
   _changes++;
   _maxAxiomIndex++;
   if (ax->set(ex, _names)) {
      ax->_index = _maxAxiomIndex;
      ax->_num_vars = int(_names._vars.size());
      _axiomsAlways.push_back(ax);
      if (ax->_num_vars > _max_vars)
         _max_vars = ax->_num_vars;
   }
}

void Mind::add_usual(ExprPtr ex)
{
   MindVarAxiomPtr ax = SPTR_MAKE(MindVarAxiom)();
   ax->_group = _group;
   _names._vars.clear();
   _changes++;
   _maxAxiomIndex++;
   if (!ax->set(ex, _names))
      return;
   // так-как в аксиоме присутствуют атомы с вероятностями,  
   // разделяем их на определенные аксиомы
   // например аксиома: X #in Y -> X in Y == (0.2,0.8)
   // разделится на две:
   // 1: X #in Y -> X in Y  (c вероятностью 0.8)
   // 2: X #in Y -> X !in Y (c вероятностью 0.2)        
   ax->_num_vars = int(_names._vars.size());
   ax->_index  = _maxAxiomIndex;   
   _axiomsUsual.push_back(ax);
   if (ax->_num_vars > _max_vars)
   {
       _max_vars = ax->_num_vars;
   }
   ax->_parent  = ax;
   split_usual_axiom(ax);
   ax->_parent  = 0;
   _usualSorted = false;
}

//=======================================================================================
// Разделить аксиому c неопределенными атомами
//
void Mind::split_usual_axiom(MindVarAxiomPtr ax)
{
   if (ax->_notSplittedAtoms == 0)
   {
      //нет атомов для деления
      //добавляем аксиому в массив 
      _axiomsUsualSplitted.push_back(ax);
      return; 
   }     
   for (size_t i = 0; i < ax->_atoms.size(); i++) {
      if (!ax->_atoms[i]._needSplit)
         continue;
      MindVarAtom &atom = ax->_atoms[i];
      // делим атом и создаем соответственно две независимые аксиомы
      // с разными вероятностями
      atom._needSplit = false;
      ax->_notSplittedAtoms--;
      MindVarAxiomPtr posAxiom = SPTR_MAKE(MindVarAxiom)(*ax);
      MindVarAxiomPtr negAxiom = SPTR_MAKE(MindVarAxiom)(*ax);
      
      posAxiom->_p = ax->_p * atom._p.p1();  //меняем вероятность, так-как атом становится опреденным
      posAxiom->_atoms[i]._p = Logic::True;  

      negAxiom->_p = ax->_p * atom._p.p0();      
      negAxiom->_atoms[i]._p   = Logic::True;
      negAxiom->_atoms[i]._not = !negAxiom->_atoms[i]._not;  //инвертируем 'not' в соответствующем атоме, так-как берем отрицательную полярность

      split_usual_axiom(posAxiom);
      split_usual_axiom(negAxiom);
   }
}

void Mind::getAxioms(Value & graphValue, MindAxioms &axioms)
{
   int group = 0;
   // значения переменных 
   vector<UInt>  vals(_max_vars + 1);             

   MindState state(MindState::MODE_COMPILE, graphValue, vals, _names, -1, _verbose, false, _out_undef);

   state._axioms = &axioms;

   for (auto & ax : _axiomsAlways) {   // по всем аксиомам         
      if (group >= 0 && ax->_group != group)
         continue;

      ax->_all_vars_ok = true;
      state._axiom = ax;

      if (_differnt_vals)
         check_rule_dif_vals(state);
      else
         check_rule_all_vals(state);
   }
}

//=======================================================================================
// Вывести все аксиомы в поток out
//
ostream & Mind::print(ostream & out)
{
   const char * names[12] = { "", "X", "Y", "Z", "U", "V", "W", "P", "Q", "R", "S", "T" };

   if (_names._consts.size()) {
      out << "Consts: ";
      for (size_t i = 0; i < _names._consts.size(); i++)
         out << _names._consts[i] << ", ";
      out << endl;
   }
   out << "Max number of variables: " << _max_vars << endl;
   //out << "Mind.mode           = " << _mode << endl;
   out << "Mind.different_vals:     " << _differnt_vals << endl;
   out << "Cycles: " << _num_cycles << endl;
   out << "Always axioms: "<< endl;

   for (size_t i = 0; i < _axiomsAlways.size(); i++) {
      MindVarAxiom & ax = *_axiomsAlways[i];
      out << "[" << ax._group<< ":" << ax._index << "] vars: " << ax._num_vars 
          << " used: " << ax._num_used << "/" << ax._num_checked << " > ";
      for (size_t j = 0; j < ax._atoms.size(); j++) {
         MindVarAtom & a = ax._atoms[j];
         if (a._demon) {            
            if (a._not)
               out << '!';
            out << a._demon->_fun->_name << "(";
            for (size_t k = 0; k < a._demon->_vars.size(); k++) {
               int x = a._demon->_vars[k];
               out << (x > 0 ? (x<12 ? names[x] : "#" + to_string(x))
                  : _names._consts[-x].toString());
               if (k + 1 < a._demon->_vars.size())
                  out << ", ";
            }
            out << ")";
         }
         else {
            string v1 = a._x1 > 0 ? (a._x1 < 12 ? names[a._x1] : "#" + to_string(a._x1))
               : _names._consts[-a._x1].toString();
            string v2 = a._x2 > 0 ? (a._x2 < 12 ? names[a._x2] : "#" + to_string(a._x2))
               : _names._consts[-a._x2].toString();
            if (a._edge == 0)
               out << " " << v1 << (a._not ? "!=" : "==") << v2;
            else
               out << (a._not ? "!" : " ") << v1 << " " << GraphManager::instance().edgeName(a._edge) << " " << v2;
          }
         if (j + 1 < ax._atoms.size())
            out << " | ";
      }
      out << endl;
   }
   if (_axiomsUsual.size() == 0)
      return out;

   out << "Usual axioms: "  << endl;

   for (size_t i = 0; i < _axiomsUsual.size(); i++) {
      MindVarAxiom & ax = *_axiomsUsual[i];
      out << "[" << ax._group << ":" << ax._index << "] vars: " << ax._num_vars
         << " used: " << ax._num_used << "/" << ax._num_checked << " > ";
      for (size_t j = 0; j < ax._atoms.size(); j++) {
         MindVarAtom & a = ax._atoms[j];
         if (a._demon) {
            if (a._not)
               out << '!';
            out << a._demon->_fun->_name << "(";
            for (size_t k = 0; k < a._demon->_vars.size(); k++) {
               int x = a._demon->_vars[k];
               out << (x > 0 ? (x<12 ? names[x] : "#" + to_string(x))
                  : _names._consts[-x].toString());
               if (k + 1 < a._demon->_vars.size())
                  out << ", ";
            }
            out << ")";
         }
         else {
            string v1 = a._x1 > 0 ? (a._x1 < 12 ? names[a._x1] : "#" + to_string(a._x1))
               : _names._consts[-a._x1].toString();
            string v2 = a._x2 > 0 ? (a._x2 < 12 ? names[a._x2] : "#" + to_string(a._x2))
               : _names._consts[-a._x2].toString();
            if (a._edge == 0)
               out << " " << v1 << (a._not ? "!=" : "==") << v2;
            else
               out << (a._not ? "!" : " ") << v1 << " " << GraphManager::instance().edgeName(a._edge) << " " << v2;
         }
         if (j + 1 < ax._atoms.size())
            out << " | ";
      }
      out << endl;
   }

   return out;
}
//=======================================================================================
// Очистить аксиомы
//
void Mind::clear()
{
   _axiomsAlways.clear();
   _axiomsUsual.clear();
   _axiomsUsualSplitted.clear();
   _names.clear();
   _max_vars = 0;
   _maxAxiomIndex = 0;
   _usualSorted = false;
   _changes++;
}
/****************************************************************************************
*                             Mind  - логический вывод                                  *
*****************************************************************************************/
//=======================================================================================
// Получить все возможные модели models для графа graph, но не более count штук
//
size_t Mind::get_models(Value &graphValue, vector<Int> &edges, vector<shared_ptr<ValueBase> >* models, size_t count)
{
   CurrentGraph currentGraph;                            // текущий граф
   if (_validator) {
      _validator->_vals.resize(1);
   }

   size_t count_models = 0;
   vector<Value> open;
   if (count > 0)
      open.reserve(count);
   open.push_back(graphValue.copy());
   if (set_graph(open.back()) < 0) {                     // вывод для стартового графа
      if (_verbose >= 2)
         TRACE << "Was inconsistent model\n";
      return 0;
   }
   Logic vals[2] = { Logic::True, Logic::False };

   while (!open.empty()) {
      Value gVal = open.back();
      Graph *g = gVal.get_Graph();
      open.pop_back();

      if (_verbose >= 2) {
         TRACE_STREAM(tout);
         tout << "*** get graph from queue:\n";  g->print(tout, 0);
      }
      bool ok = false;
      for (size_t k = 0; k < edges.size(); k++) {  // по запрошенным рёбрам:
         for (size_t i = 1; i < g->_nodes.size(); i++) {
            vector<Edge> & e = g->_nodes[i].edgesOut(edges[k])._edges;
            if (e.size() == g->_nodes.size())     // все рёбра установлены
               continue;                          // проверять смысла нет

            for (size_t j = 1; j < g->_nodes.size(); j++) {
               // для неопределённого ребра:
               if (g->val(edges[k], i, j, NULL, NULL, false) == Logic::Undef) {
                  // устанавливаем ребро в True,False:
                  for (auto val : vals) {
                     open.push_back(gVal.copy());
                     Graph *g0 = open.back().get_Graph();
                     g0->add(edges[k], i, j, val);
                     if (_verbose >= 2) {
                        TRACE_STREAM(tout);
                        tout << "*** set edge: " << g0->node_name(i) << " " << g0->edge_name(edges[k]) << " " << g0->node_name(j) << " = " << val << "\n";
                     }
                     int res = set_graph(open.back());        // делаем логический вывод
                     if (res < 0) {
                        TRACE << "!!! Mind::get_models> wrong axioms\n";
                        continue;
                     }
                     if (_verbose >= 2) {
                        TRACE_STREAM(tout); tout << "--- number of new edges: " << res << "\n";
                        g0->print(tout, 0);
                     }
                  }//val
                  ok = true;
                  break;                                     // иначе могут быть дубли
               }//if
            }//j
            if (ok) break;
         } //i
         if (ok) break;
      }//k

      if (!ok) {                                   // это полностью определёная модель         
         if (_validator) {
            currentGraph.set(gVal);                // меняем текущий граф
            StackFrame frame(*_validator);
            _validator->_codeCommon->_local_variables[0]->_val = gVal;
            Logic val = _validator->run().get_Logic();
            if (val != Logic::True) {
               if (_verbose >= 3) {
                  TRACE_STREAM(tout);
                  tout << "*** final graph not valid:\n";  g->print(tout, 0);
               }
               continue;
            }
         }
         count_models++;                            // подсчитываем число моделей

         if (models) {
            gVal.get_Graph()->_graph_name = g->_graph_name + "_" + to_string(models->size() + 1);
            if (_verbose >= 2) {
               TRACE_STREAM(tout);
               tout << "*** final graph:\n";  g->print(tout, 0);
            }

            models->push_back(gVal.ptr());
            if (count > 0 && models->size() >= count) {
               return models->size();
            }
         }
      }
   }

   return count_models;
}

size_t Mind::get_usual_models(MindGraphModelPtr graphModel, vector<Int> &edges, vector<MindGraphModelPtr> &graphModels, size_t count/* = 0*/, Float minP/* = 0.0*/, Float maxP/* = 1.0*/)
{
   Value &graphValue = graphModel->_graphValue;
   CurrentGraph currentGraph;                            // текущий граф
   if (_validator) {
      _validator->_vals.resize(1);
   }

   size_t count_models = 0;

   //модели отсортированные по вероятности
   multimap<Float, MindGraphModelPtr> open;
   multimap<Float, MindGraphModelPtr> close;
   vector<MindGraphModelPtr>          tmpModels;

   open.insert(pair<Float, MindGraphModelPtr>(-graphModel->_p, graphModel));

   while (!open.empty()) {
      if (count > 0 && count_models >= count)
         break;
      auto openFirstIt = open.begin();
      MindGraphModelPtr openGraphModel = openFirstIt->second;
      Value             openGraphValue = openGraphModel->_graphValue;
      if (_verbose >= 2) {
         TRACE_STREAM(tout);
         tout << "*** get graph (p:" << openGraphModel->_p << ") from queue:\n";  openGraphValue.get_Graph()->print(tout, 0);
      }
      if (openGraphModel->_p < minP)
         break; //дальше идут модели с более низкими вероятностями, поэтому делить нет смысла, так как деление только снижает вероятность

      open.erase(openFirstIt);
      //применяем стандартные аксиомы
      int res = set_graph(openGraphValue);
      if (res < 0) {
         //модель не подходит
         continue;
      }
      tmpModels.clear();
      //сначала делим модель используя "usual" аксиомы
      split_graph_model_with_usual_axioms(openGraphModel, tmpModels);
      if (tmpModels.size() == 0)
      {
         //если "usual" аксиомы не сработали, делим модель используя не определенные ребра "Undef"
         split_graph_model(openGraphModel, edges, tmpModels);
      }

      if (tmpModels.size() == 0)
      {
         //модель полностью определена
         if (openGraphModel->_p < minP || openGraphModel->_p > maxP)
            continue; //не удовлетворяет установленному промежутку

         Graph* g = openGraphValue.get_Graph();
         if (_validator) {
            currentGraph.set(openGraphValue);                // меняем текущий граф
            StackFrame frame(*_validator);
            _validator->_codeCommon->_local_variables[0]->_val = openGraphValue;
            Logic val = _validator->run().get_Logic();
            if (val != Logic::True) {
               if (_verbose >= 3) {
                  TRACE_STREAM(tout);
                  tout << "*** final graph not valid:\n";  g->print(tout, 0);
               }
               continue;
            }
         }
         g->_graph_name = g->_graph_name + "_" + to_string(close.size() + 1);
         if (_verbose >= 2) {
            TRACE_STREAM(tout);
            tout << "*** final graph (p:"<< openGraphModel->_p << "):\n";  openGraphValue.get_Graph()->print(tout, 0);
         }
         close.insert(pair<Float, MindGraphModelPtr>(-openGraphModel->_p, openGraphModel));
         count_models++;
      }         
      else
      {
         for (auto tmpModel : tmpModels)
         {
            if (_verbose >= 2) {
               TRACE_STREAM(tout);
               tout << "*** add graph (p:" << tmpModel->_p  << ") in queue:\n";  tmpModel->_graphValue.get_Graph()->print(tout, 0);
            }
            open.insert(pair<Float, MindGraphModelPtr>(-tmpModel->_p, tmpModel));
         }            
      }
   }

   for (auto &pair : close) {
      graphModels.push_back(pair.second);
   }

   return count_models;
}
//=======================================================================================
// Подготовить перед вычислениями демонов
//
void Mind::prepare_demons()
{
   for (size_t i = 0; i < _axiomsAlways.size(); i++) {
      MindVarAxiom &ax = *_axiomsAlways[i];
      for (size_t k = 0; k < ax._atoms.size(); k++) {
         MindDemonPtr demon = ax._atoms[k]._demon;
         if (demon) {
            demon->_fun->_vals.resize(demon->_fun->_vars.size());
            for (size_t i = 0; i < demon->_fun->_vars.size(); i++) {
               int x = demon->_vars[i];
               if (x <= 0)
                 *demon->_fun->_codeCommon->_local_variables[i]  = _names._consts[-x];
               else
                  *demon->_fun->_codeCommon->_local_variables[i] = ValueNode(0);
            }
         }
      }
   }
}
/*
//=======================================================================================
// Установить все рёбра графа graph в соответствии с аксиомами
//
int Mind::set_graph(Value &graphValue, bool show_false, Int group)
{
   if (_verbose)
      TRACE << "Mind.verbose = " << _verbose << endl;

   if (!_sorted) {
      std::sort(_axioms.begin(), _axioms.end());  // упорядочиваем по числу переменных
      _sorted = true;
   }

   prepare_demons();                              // подготавляиваем демонов

   if (_max_vars < 1) {
      TRACE_CRITICAL << "! Mind::set_graph> No axioms with variables\n";
      return 0;
   }
   Graph *g = graphValue.get_Graph();
   _names._isa_id = ReservedEdgeIDs::EDGE_ISA;    // id ребра isa

   vector<UInt>  vals(_max_vars + 1);             // значения переменных
   int max_val = int(g->_nodes.size()) - 1;       // максимальный номер узла

   // Повторяем до тех пор пока граф не перестанет изменяться
   //
   g->_changes = 0;                                // сбрасываем измения
   UInt num_sets = 0;
   for (size_t cicle = 0; cicle < 1000000; cicle++) {// \todo
      UInt changes = g->_changes;
	  int num_vars = 1;                            // число переменных

      // Так как число переменных в аксиомах может быть произвольным,
      // органицания циклов по переменным делается при помощи перебора
      // всех чисел от [1, ..., 1] до [max_val, ..., max_val] в массиве vals

      for (Int i = 1; i < vals.size(); i++)       // начальные значения переменных
         vals[i] = 1;                             // все переменные на первый узел

      while (true) {                              // перебераем все переменные
         // применяем аксиомы:
         Int num = check_rules(graphValue, vals, num_vars, show_false, group);
         if (num < 0)                            
            return -1;                            // было противоречие
         num_sets += num;


         UInt k = 0, i = 1;
         for (; i < vals.size(); i++) {           // бежим по разрядам "числа" вправо
            if (int(vals[i]) < max_val) {         // пока меньше максимального значения
               vals[i]++;                         // увеличиваем разряд
               break;                             // и всё по новой
            }
            else                                  // достигли максимального значения
               vals[i] = 1;                       // "обнуляем"
         }
         if (i == vals.size())
            break;
         if (int(i) > num_vars)
            num_vars = (int)i;
      }

      if (g->_changes == changes)                 // не было изменений графа
         return num_sets;                         // общее число изменений
   }
   TRACE_CRITICAL << "! Mind::set_graph> Done max number of loops, but graph changes\n";
   return 0;
}
//=======================================================================================
// Проверить аксиомы с >= num_var  переменными c индексми ids
//
int Mind::check_rules(Value &graphValue, vector<UInt> &vals, Int num_vars, bool show_false, Int group)
{
   //TRACE<<num_vars<<": "; for(size_t i=0;i<vals.size();i++) TRACE<<vals[i];TRACE<<endl;

   UInt num_sets = 0;
   for (size_t i = 0; i < _axioms.size(); i++) {
      MindVarAxiom & ax = *_axioms[i];
      if (group >= 0 && ax._group != group)
         continue;
      if (ax._num_vars < num_vars)                
		   continue;
      if (_mode == 1) {
         int res = ax.check_rule1(graphValue, vals, _names, _verbose, show_false);
         if (res < 0)
            return -1;
         if (res > 0) {
            ax._num_used++;
            num_sets += res;
         }
      }
      else
         ax.check_rule2(graphValue, vals, _names, _verbose, show_false);
   }
   return num_sets;
}
*/
//=======================================================================================
// Установить все рёбра графа graph в соответствии с аксиомами
//
int Mind::set_graph(Value &graphValue, bool show_false, Int group)
{
   if (!_show_false)       // был вызван Mind.verbose(-1)
      show_false = false;

   _was_conflict = false;

   if (_verbose)
   {
      TRACE << "Mind::set_graph(" << graphValue.get_Graph()->name() <<  ", group = " << group << ")" << endl;
   }
   /*
   if (!_sorted) {
      std::sort(_axioms.begin(), _axioms.end());  // упорядочиваем по числу переменных
      _sorted = true;
   }
   */

   prepare_demons();                              // подготавляиваем демонов

   /*
   if (_max_vars < 1) {
      TRACE_CRITICAL << "! Mind::set_graph> No axioms with variables\n";
      return 0;
   }
   */

   Graph *g = graphValue.get_Graph();
   
   vector<UInt>  vals(_max_vars + 1);             // значения переменных 


   /*
   if (_changes == 0 && g->_changes == 0 && group == 0)
   {
      return 0;
   }
   */
   
   // сбрасываем изменения
   g->_changes = 0;                                
   _changes = 0;
   // Повторяем до тех пор пока граф не перестанет изменяться
   //
   for (auto ax : _axiomsAlways)
      ax->_all_vars_ok = false;

   MindState state(MindState::MODE_SET, graphValue, vals, _names, -1, _verbose, show_false, _out_undef);

   for (size_t cicle = 0; cicle < 1000000; cicle++) {// \todo
      _num_cycles++;
      UInt changes = g->_changes;

      g->checkAxioms(); // пробегаем по аксиомам графа
    
      for (auto & ax : _axiomsAlways) {   // по всем аксиомам         
         if (group >= 0 && ax->_group != group)
            continue;
         // переключение узлов!
         //if (ax._all_vars_ok)
         //   continue;                             // эта аксиома уже выполняется для всех

         ax->_all_vars_ok = true;
         state._axiom = ax;

         if(_differnt_vals)
            check_rule_dif_vals(state);
         else
            check_rule_all_vals(state);
      }

      if (g->_changes == changes)                  // не было изменений графа
      {
         _was_conflict = state._blocked;
         return state._num_sets;                   // общее число изменений
      }
         
   }

   TRACE_CRITICAL << "! Mind::set_graph> Done max number of loops, but graph changes\n";
   return 0;
}
//=======================================================================================
#define CHECK_RULE   int res = ax.check_rule1(state);                                          \
                     ax._num_checked++;                                                        \
                     if (res < 0 || state._blocked)                                            \
                        return;                                                                \
                     if (res > 0)  ax._num_used++;                                             

/*!
Разделить граф на два новых в которых неопределенное ребро исходного графа будет установлено
в "True" для одного результирующего графа и в "False" для другого
\param graphValue исходных граф
\param edges список ребер по которым делаем разделение
\param models массив сгенерированных моделей (если равен 0-лю, то дальнейшее разделение невозможно)
*/
void Mind::split_graph_model(MindGraphModelPtr graphModel, vector<Int>& edges, vector<MindGraphModelPtr>& graphModels, bool saveLog/* = false*/)
{
   Value &graphValue = graphModel->_graphValue;
   Graph *g = graphValue.get_Graph();
   Logic vals[2] = { Logic::True, Logic::False };
   bool ok = false;

   for (size_t k = 0; k < edges.size(); k++) {  // по запрошенным рёбрам:
      for (size_t i = 1; i < g->_nodes.size(); i++) {
         vector<Edge> & e = g->_nodes[i].edgesOut(edges[k])._edges;
         if (e.size() == g->_nodes.size())     // все рёбра установлены
            continue;                          // проверять смысла нет

         for (size_t j = 1; j < g->_nodes.size(); j++) {
            // для неопределённого ребра:
            if (g->val(edges[k], i, j, NULL, NULL, false) == Logic::Undef) {
               // устанавливаем ребро в True,False:
               for (auto val : vals) {
                  Value newGraphValue = graphValue.copy();
                  MindGraphModelPtr newModel = SPTR_MAKE(MindGraphModel)(newGraphValue, graphModel->_p);
                  Graph *g0 = newGraphValue.get_Graph();
                  g0->add(edges[k], i, j, val);
                  if (saveLog)
                  {
                     std::stringstream str;
                     str << g0->node_name(i) << " " << g0->edge_name(edges[k]) << " " << g0->node_name(j) << " = " << val << " (split always)";
                     newModel->pushLog(str.str());
                  }
                  if (_verbose >= 2) {
                     TRACE_STREAM(tout);
                     tout << "Mind::split_graph_model set edge: " << g0->node_name(i) << " " << g0->edge_name(edges[k]) << " " << g0->node_name(j) << " = " << val << "\n";
                  }
                  //int res = set_graph(newGraphValue);        // делаем логический вывод
                  //if (res < 0) {
                  //   TRACE << "!!! Mind::split_graph> wrong axioms\n";
                  //   continue;
                  //}
                  //if (_verbose >= 2) {
                  //   TRACE_STREAM(tout); tout << "--- number of new edges: " << res << "\n";
                  //   g0->print(tout, 0);
                  //}
                  graphModels.push_back(newModel);
               }//val

               ok = true;
               break;                                     // иначе могут быть дубли
            }//if
         }//j
         if (ok) break;
      } //i
      if (ok) break;
   }//k
}

/*!
Разделить граф используя аксиомы с вероятностями
\param graphValue исходных граф
\param models массив сгенерированных моделей (если равен 0-лю, то дальнейшее разделение невозможно)
*/
bool Mind::split_graph_model_with_usual_axioms(MindGraphModelPtr graphModel, vector<MindGraphModelPtr>& graphModels, bool saveLog/* = false*/)
{  
   Value &graphValue = graphModel->_graphValue;

   if (!_usualSorted)
   {
      std::sort(_axiomsUsualSplitted.begin(), _axiomsUsualSplitted.end(),
         [](const MindVarAxiomPtr & a, const MindVarAxiomPtr & b) -> bool
      {
         return a->_p > b->_p;
      });
      _usualSorted = true;
   }

   Graph *g = graphValue.get_Graph();

   if (_verbose)
   {
      TRACE << "split_graph_with_usual_axioms(" << graphValue.get_Graph()->name() << ")" << endl;
   }
   if (_verbose >= 2) {
      TRACE_STREAM(tout);
      tout << "input graph:\n";  g->print(tout, 0);
   }

   prepare_demons();                              // подготавляиваем демонов

   vector<UInt>  vals(_max_vars + 1);             // значения переменных 

   // сбрасываем изменения
   g->_changes = 0;
   _changes = 0;

   for (auto ax : _axiomsUsualSplitted)
      ax->_all_vars_ok = false;

   //ищем первую попытку установки ребра графа и тут-же останавливаемся для дальнейшего разделения
   MindState state(MindState::MODE_CHECK, graphValue, vals, _names, 1, _verbose, false, _out_undef);

   UInt changes = g->_changes;

   g->checkAxioms(); // пробегаем по аксиомам графа

   for (auto & ax : _axiomsUsualSplitted) {    // по всем аксиомам         
      ax->_all_vars_ok = true;
      state._axiom = ax;

      if (_differnt_vals)
         check_rule_dif_vals(state);
      else
         check_rule_all_vals(state);
      
      if (state._blocked)
         break;
   }

   if (state._pendingTransactions.size() == 0)
      return false;

   //есть отложенная транзакция, разделяем граф на две части и применяем
   //транзакцию с противоположными полярностями к обоим графам

   MindTransaction &transaction = state._pendingTransactions[0];

   Value graphLeft  = graphValue.copy();
   Value graphRight = graphValue.copy();

   //создаем модели и устанавливаем им вероятности
   Float graphLeftModelP  = transaction._axiom->_p;
   Float graphRightModelP = 1.0 - transaction._axiom->_p;

   Float graphModelPowP = pow(graphModel->_p, graphModel->_n);

   //вычисляем среднее геометрическое новой вероятности и сохраняем его
   graphLeftModelP  = pow(graphModelPowP * graphLeftModelP,  1.0 / (graphModel->_n + 1));
   graphRightModelP = pow(graphModelPowP * graphRightModelP, 1.0 / (graphModel->_n + 1));

   MindGraphModelPtr graphLeftModel  = SPTR_MAKE(MindGraphModel)(graphLeft,  graphLeftModelP,  graphModel->_n + 1);
   MindGraphModelPtr graphRightModel = SPTR_MAKE(MindGraphModel)(graphRight, graphRightModelP, graphModel->_n + 1);

   state._graphValue = graphLeft;
   state.setAtom(transaction._axiom, (int)transaction._atomIndex, transaction._vals, true);
   state._graphValue = graphRight;
   state.setAtom(transaction._axiom, (int)transaction._atomIndex, transaction._vals, false);

   if (saveLog)
   {
      graphLeftModel->pushLog(state.atomToStr(transaction._axiom, (int)transaction._atomIndex, transaction._vals, true) + " (split usual)");
      graphRightModel->pushLog(state.atomToStr(transaction._axiom, (int)transaction._atomIndex, transaction._vals, false) + " (split usual)");
   }

   graphModels.push_back(graphLeftModelP > graphRightModelP ? graphLeftModel  : graphRightModel);
   graphModels.push_back(graphLeftModelP > graphRightModelP ? graphRightModel : graphLeftModel);

   return true;
}

void Mind::merge(Value & graphValue1, Value & graphValue2, Value &excludeEdges)
{ 
   //сначала выполняем логический вывод над графами
   //set_graph(graphValue1);
   //set_graph(graphValue2);
   MindAxioms graph1Axioms;
   MindAxioms graph2Axioms;
   //получаем аксиомы обоих графов
   getAxioms(graphValue1, graph1Axioms);
   getAxioms(graphValue2, graph2Axioms);
   if (_verbose > 0)
   {
      TRACE << "Mind.merge(" << graphValue1.get_Graph()->name() << ", " << graphValue2.get_Graph()->name() << ")" << endl;
   }
   //создаем объект выполняющий объединение
   MindMerge merge(graphValue1, graph1Axioms, graphValue2, graph2Axioms, excludeEdges, _verbose);
   merge.init();
   if (!merge.need())
      return; //!< нет атомов для объединения

   //запускаем объединение
   merge.run();
}

//=======================================================================================
// Проверить аксиому по всем переменным
//
void Mind::check_rule_all_vals(MindState &state)
{
   Graph *g      = state._graphValue.get_Graph();
   MindVarAxiom    &ax   = *state._axiom;
   vector<UInt> &vals = state._vals;
   int max_val = int(g->_nodes.size()) - 1;       // максимальный номер узла
   if (ax._num_vars == 1) {                       // одна переменная:
      for (int i = 1; i <= max_val; i++) {
         vals[1] = i;
         CHECK_RULE                               // применяем аксиому
      }
   }
   else if (ax._num_vars == 2) {                  // две пременные:
      for (int i = 1; i <= max_val; i++) {
         vals[1] = i;
         for (int j = 1; j <= max_val; j++) {
            vals[2] = j;
            CHECK_RULE                            // применяем аксиому
         }
      }
   }
   else {                                         // произвольное число переменных
      for (size_t i = 1; i <= ax._num_vars; i++)
         vals[i] = 1;                             // начальные значения на первый узел

      while (true) {                              // перебераем все переменные
         CHECK_RULE                               

         UInt k = 0, i = 1;
         for (; i <= ax._num_vars; i++) {         // бежим по разрядам "числа" вправо
            if (int(vals[i]) < max_val) {         // пока меньше максимального значения
               vals[i]++;                         // увеличиваем разряд
               break;                             // и всё по новой
            }
            else                                  // достигли максимального значения
               vals[i] = 1;                       // "обнуляем"
         }
         if (i > ax._num_vars)
            break;
      }
   }  
}
//=======================================================================================
// Проверить аксиому по всем не совпадающим переменным
//
void Mind::check_rule_dif_vals(MindState &state)
{
   Graph *g = state._graphValue.get_Graph();
   MindVarAxiom    &ax   = *state._axiom;
   vector<UInt> &vals = state._vals;
   int max_val = int(g->_nodes.size()) - 1;       // максимальный номер узла
   if (ax._num_vars == 1) {                       // одна переменная:
      for (int i = 1; i <= max_val; i++) {
         vals[1] = i;
         CHECK_RULE                               
      }
   }
   else if (ax._num_vars == 2) {                  // две переменных:
      for (int i = 1; i <= max_val; i++) {
         vals[1] = i;
         for (int j = 1; j <= max_val; j++) if ( i!=j ){
            vals[2] = j;
            CHECK_RULE                            
         }
      }
   }
   else {                                         // произвольное число переменных:
      for (size_t i=1; i <= ax._num_vars; i++)  
         vals[i] = 1;                             // начальные значения на первый узел
      vector<bool> was;  was.resize(max_val + 1);
      while (true) {                              // перебераем все не равные переменные
         for (size_t i = 1; i < was.size(); i++) was[i] = false;
         bool ok = true;
         for (size_t i = 1; i <= ax._num_vars; i++) {
            if (was[vals[i]]) {
               ok = false;
               break;
            }
            was[vals[i]] = true;
         }
         if (ok) {
            CHECK_RULE                               // применяем аксиому
         }

         UInt k = 0, i = 1;
         for (; i <= ax._num_vars; i++) {         // бежим по разрядам "числа" вправо
            if (int(vals[i]) < max_val) {         // пока меньше максимального значения
               vals[i]++;                         // увеличиваем разряд
               break;                             // и всё по новой
            }
            else                                  // достигли максимального значения
               vals[i] = 1;                       // "обнуляем"
         }
         if (i > ax._num_vars)
            break;
      }
   }
}

