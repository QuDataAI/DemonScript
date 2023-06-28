/*!            MindLiteral - единица аксиомы, содержащая атом с отрицанием либо без

(с) 2019-apr: steps: synset.com, qudata.com
****************************************************************************************/

#ifndef MindLiteralH
#define MindLiteralH

#include "MindAtom.h"

//=======================================================================================
//! Атом аксиомы с определенными значениями переменных
//
class MindLiteral
{
public:
   MindLiteral() {}
   MindLiteral(MindAtomWPtr atom, bool not_);
   //! Вывести литерал в строку 
   string to_str(Graph &g, bool not_ = false, bool ignoreVal = false);
   MindAtomWPtr   _atom;         //!< атом аксиомы
   bool           _not = false;  //!< есть ли отрицание перед атомом
};


#endif
