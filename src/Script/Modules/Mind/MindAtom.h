/*!            MindAtom - атом аксиомы с определенными значениями переменных

(с) 2019-apr: steps: synset.com, qudata.com
****************************************************************************************/

#ifndef MindAtomH
#define MindAtomH

#include "ValueGraph.h"
#include "MindVarAtom.h"

SPTR_DEF(MindAtom)
SPTR_WDEF(MindAtom)
SPTR_DEF(MindAxiom)
SPTR_WDEF(MindAxiom)

//=======================================================================================
//! Атом аксиомы с определенными значениями переменных
//
class MindAtom
{
public:
   MindAtom(Int edge, MindDemonPtr demon, vector<UInt> &atomVals, Value &vg1, Value &vg2);
   ~MindAtom();
   //! Вычислить атом
   void calc(Value &graphValue);
   //! Вычислить атом-demon 
   Logic calc_demon(Value &graphValue);
   //! Установить новое значение атома
   void set(Value &graphValue, Logic val, int verbose = 0, const char* verbosePrf = "");
   //! Установить новое значение демона
   void set_demon(Value &graphValue, Logic val);
   //! Вывести атом в строку 
   string to_str(Graph &g, bool not_ = false, bool ignoreVal = false);
   int                     _edge;       //!< отношение, если == 0, то это X==Y
   vector<UInt>            _vals;       //!< значения переменных, либо констант
   Value                   _vg1, _vg2;  //!< значения графов
   MindDemonPtr            _demon;      //!< произвольный демон
   vector<MindAxiomWPtr>   _axioms;     //!< список аксиом, содержащих атом c положительным литералом
   vector<MindAxiomWPtr>   _axioms_not; //!< список аксиом, содержащих атом c отрицательным литералом
   Logic                   _val;        //!< логическое значение атома
};


#endif
