/*!            MindState - транзакция системы вывода (для отложенного выполнения)

(с) 2018-oct: steps: synset.com, qudata.com
****************************************************************************************/
#ifndef MindTransactionH
#define MindTransactionH

#include "MindVarAxiom.h"

SPTR_DEF(MindVarAxiom)

//=======================================================================================
//! Транзакция системы вывода (для отложенного выполнения)
//
class MindTransaction
{
public:
   MindTransaction(MindVarAxiomPtr axiom, UInt atomIndex, vector<UInt> &vals);
   MindVarAxiomPtr  _axiom;      //!< ссылка на аксиому, которая сработала
   UInt             _atomIndex;  //!< индекс атома для отложенного выполнения
   vector<UInt>     _vals;       //!< значения переменных
};

#endif