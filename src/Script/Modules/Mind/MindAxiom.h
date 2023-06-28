/*!            MindAxiom - аксиома, содержащая определенные атомы, которые в свою очередь соеденены логическим ИЛИ

(с) 2019-apr: steps: synset.com, qudata.com
****************************************************************************************/

#ifndef MindAxiomH
#define MindAxiomH

#include "MindVarAxiom.h"
#include "MindLiteral.h"

SPTR_DEF(MindAxiom)

//=======================================================================================
//! Атом аксиомы при объединении графов
//
class MindAxiom
{
public:
   MindAxiom();
   ~MindAxiom();
   //! Вывести аксиому в строку 
   string to_str(Graph &g);
   vector<MindLiteral>  _literals;           //!< литералы аксиомы
   Int                  _trueLiteralIndex;   //!< индекс атома, который выведен аксиомой если она сработала, иначе -1
};


#endif
