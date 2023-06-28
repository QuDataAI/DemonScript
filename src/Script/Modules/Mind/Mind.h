/*!            Mind - модуль логических выводов из множества аксиом

Доступ к функциям модуля осуществляется через вызов Mind.<имя функции>(<аргументы>)

                                             (с) 2018-oct: steps: synset.com, qudata.com
****************************************************************************************/
#ifndef MindH
#define MindH

#include "Modules/Module.h"
#include "MindNames.h"
#include "MindVarAxiom.h"
#include "MindGraphModel.h"
#include "MindState.h"
#include "Expr.h"
#include "Function.h"
#include "Graph.h"

#include <vector>
#include <map>
#include <algorithm>    // std::sort


//=======================================================================================
//! Модуль логических функций
//
class Mind : public Module
{
   INTERFACE_MODULE(Mind)
public:
   MindNames            _names;                 //!< список всех констанат и переменных
   vector<MindVarAxiomPtr> _axiomsAlways;          //!< список аксиом, которые выполняются всегда
   vector<MindVarAxiomPtr> _axiomsUsual;           //!< список аксиом, которые выполняются обычно 
   vector<MindVarAxiomPtr> _axiomsUsualSplitted;   //!< список аксиом, которые получены после разделения _axiomsUsual 
   UInt                 _max_vars;              //!< максимальное число переменных
   Bool                 _usualSorted;           //!< были ли отсортированы _axiomsUsual
   int                  _mode;                  //!< 1 - 3-значная логики, 2 - нечёткая
   int                  _verbose;               //!< детали порядке вычислений
   int                  _differnt_vals;         //!< использовать только различные значения переменных
   int                  _out_undef;             //!< выводить неопределённые выражения
   int                  _group;                 //!< текущая группа аксиом
   int                  _maxAxiomIndex;         //!< максимальный индекс аксиомы
   FunDemonPtr          _validator;             //!< демон корректности для get_models и count_models
   UInt                 _num_cycles;            //!< число циклов
   UInt                 _changes;               //!< число сделанных изменений графа
   bool                 _show_false;            //!< выводить конфликты аксиом
   bool                 _was_conflict;          //!< был конфликт аксиом после последнего set_graph

   Mind();

   //! Добавить аксиому, которая выполняется всегда
   void add(ExprPtr ex);

   //! Добавить аксиому, которая выполняется обычно
   void add_usual(ExprPtr ex);

   //! Установить все рёбра графа graph в соответствии с аксиомами
   //! Если show_false, то выводится сообщение о ложности аксиомы 
   int set_graph(Value &graphValue, bool show_false=true, Int group = -1);

   /*!
   Разделить модель графа на два новых в которых неопределенное ребро исходного графа будет установлено
   в "True" для одного результирующего графа и в "False" для другого
   \param graphModel исходная модель графа
   \param edges список ребер по которым делаем разделение
   \param models массив сгенерированных моделей (если равен 0-лю, то дальнейшее разделение невозможно)
   */
   void split_graph_model(MindGraphModelPtr graphModel, vector<Int>& edges, vector<MindGraphModelPtr>& graphModels, bool saveLog = false);
   
   /*!
   Разделить граф используя аксиомы с вероятностями
   \param model исходная модель графа
   \param models массив сгенерированных моделей (если равен 0-лю, то дальнейшее разделение невозможно)
   */
   bool split_graph_model_with_usual_axioms(MindGraphModelPtr graphModel, vector<MindGraphModelPtr> &graphModels, bool saveLog = false);

   /*!
   Объединить два графа. Копируем ребра, которые определены в одном графе и не определены в другом
   за исключением ребер которые стали следсвием ребер меняющих свое значение в обоих графах
   \param graphValue1 граф для объединения
   \param graphValue2 граф для объединения
   \param excludeEdges список ребер, которые не нужно сливать
   */
   void merge(Value &graphValue1, Value &graphValue2, Value &excludeEdges);

   //! Проверить аксиомы с >= num_var  переменными vals
   //int check_rules(Value &graphValue, vector<UInt> &vals, Int num_vars, bool show_false, Int group);   
   void check_rule_all_vals(MindState &state);
   void check_rule_dif_vals(MindState &state);

   //! Вывести все аксиомы  в поток out
   ostream & print(ostream & out);
   //! Очистить аксиомы
   void clear();

   //! Подготовить перед вычислениями демонов
   void prepare_demons();

   //! Получить все возможные модели models для графа graph, но не более count штук
   size_t get_models(Value &graphValue, vector<Int> &edges, vector<shared_ptr<ValueBase> > * models, size_t count);

   //! Получить все возможные модели models для графа graph, но не более count штук (дополнительно используются usual аксиомы)
   size_t get_usual_models(MindGraphModelPtr graphModel, vector<Int> &edges, vector<MindGraphModelPtr> &graphModels, size_t count = 0, Float minP = 0.0, Float maxP = 1.0);

private:
   //! Разделить аксиому c неопределенными атомами
   void split_usual_axiom(MindVarAxiomPtr ax);
   //! Получить аксиомы графа
   void getAxioms(Value &graphValue, MindAxioms &axioms);
   
};


#endif
