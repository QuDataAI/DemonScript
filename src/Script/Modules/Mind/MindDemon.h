/*!            MindDemon - демон, выступающий в качестве атома аксиомы

(с) 2018-oct: steps: synset.com, qudata.com
****************************************************************************************/
#ifndef MindDemonH
#define MindDemonH

#include "OTypes.h"
#include "Function.h"
#include <vector>

using namespace std;

SPTR_DEF(MindDemon)
SPTR_DEF(FunDemon)

//=======================================================================================
//! Демон, выступающий в качестве атома аксиомы
//
class MindDemon
{
public:
   FunDemonPtr   _fun;                           //!< вычисляемая функция
   bool          _set;                           //!< есть раздел set
   vector<int>   _vars;                          //!< переменные, если <=0 - то константы

   MindDemon() { _fun = 0;  _set = false; }
};

#endif