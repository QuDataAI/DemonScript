/*!            MindMerge - модуль выполняющий объединение двух графов следующим образом:
На основании аксиом, вычисляем значения атомов:
(F F F F T)
(F F F T)
...
Затем берутся атомы, которые изменили значение в обоих графах
и их значения переводятся в Undef
Далее рекурсивно отменяются выводы тех аксиом, в которых атомы сменили значение с False на Undef

После отмены атомомов копируем оставшиеся определенные атомы в противоположный граф, 
если они в нем неопределены

(с) 2019-apr: steps: synset.com, qudata.com
****************************************************************************************/

#ifndef MindMergeH
#define MindMergeH

#include "MindMergeItem.h"

//=======================================================================================
//!  Модуль, выполняющий объединение двух графов
//
class MindMerge 
{
public:
   MindMerge(Value &graph1Val, MindAxioms &graph1Axioms, Value &graph2Val, MindAxioms &graph2Axioms, Value &excludeEdges, int verbose);
   /*!
   Подготовка к объединению:
   - получаем список атомов-кандидатов на копирование
   - получаем список атомов которые изменили значение
   */
   void init();
   /*!
   Проверка необходимости выполнять объединение:
   */
   bool need();
   /*!
   Выполняем объединение
   */
   void run();
private:
   MindMergeItem _mergeItem1;  //!< объект объединения
   MindMergeItem _mergeItem2;  //!< объект объединения
   /*!
   Определенные  ребра графа тоже по сути являются единичными аксиомами, поэтому 
   добавляем и их
   */
   void addSingleAtoms();
   /*!
   Добавить атомы из списка исключений
   */
   void addExcludeAtoms();
   /*!
   Рекурсивно отменяем следствия атомов, из списка "WAIT_FOR_CANCEL";
   */
   bool processWaitForCancel(MindMergeItem &mergeItem);
   /*!
   Переносим не отмененные атомы в противоположный граф
   */
   void transferWaitForCopy(MindMergeItem &mergeItem1, MindMergeItem &mergeItem);

   int   _verbose;   //!< уровень отладки
};


#endif
