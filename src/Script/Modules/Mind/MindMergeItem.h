/*!            MindMergeItem - объект объединения графов
Содержит список атомов-кандидатов на перенос и список атомов, которые сменили значение

(с) 2019-apr: steps: synset.com, qudata.com
****************************************************************************************/

#ifndef MindMergeItemH
#define MindMergeItemH

#include "MindAxioms.h"

//=======================================================================================
//!  Объект объединения графов
//
class MindMergeItem
{
public:
   MindMergeItem(Value &graphVal, MindAxioms &graphAxioms, Value &excludeEdges);
   Value &              _graphVal;     //!< значение графа
   MindAxioms &         _graphAxioms;  //!< аксиомы графа
   Value &              _excludeEdges; //!< ребра которые нужно исключить из слияния
   vector<MindAtomPtr>  _waitForCopy;  //!< атомы-кандидаты на перенос в другой граф
   vector<MindLiteral>  _waitForCancel;//!< литералы, которые нужно отменить
};


#endif
