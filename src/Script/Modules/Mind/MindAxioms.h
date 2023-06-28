/*!            MindAxioms - список аксиом текущего графа

(с) 2019-apr: steps: synset.com, qudata.com
****************************************************************************************/

#ifndef MindAxiomsH
#define MindAxiomsH

#include "MindAxiom.h"

class MindState;

SPTR_DEF(MindAxioms)

//=======================================================================================
//! Cписок аксиом текущего графа
//
class MindAxioms
{
public:
   MindAxioms();
   ~MindAxioms();
   vector<MindAtomPtr>  _atoms;        //!< список атомов аксиом
   vector<MindAxiomPtr> _axioms;       //!< список аксиом графа
   /*!
   Добавить новую аксиому из переменной с текущим состоянием переменных
   \param state текущее состояние Mind
   */
   void add(MindState &state);
   /*!
   Получить статический атом из переменного, если
   ранее добавлен в словарь атомов, берем оттуда, иначе создаем новый
   \param varAxiom переменная аксиома
   \param state текущее состояние Mind
   */
   MindAtomPtr getAtom(MindVarAtom & varAtom, MindState &state);
   /*!
   Получить статический атом ребра, если
   ранее добавлен в словарь атомов, берем оттуда, иначе создаем новый
   \param varAxiom переменная аксиома
   \param state текущее состояние Mind
   */
   MindAtomPtr getEdgeAtom(int edge, UInt n1, UInt n2, Value &vg2);
   /*!
   Проверяем какие аксиомы сработали
   */
   void check();
private:

};


#endif
