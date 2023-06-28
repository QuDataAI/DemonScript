/*!                           јтом аксиомы

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef AtomH
#define AtomH

#include "OTypes.h"
#include "Logic.h"
#include <memory>

SPTR_DEF(Expr)

class Atom {
public:
   bool       _not = false;            //!< есть ли перед атомом отрицание
   Logic      _val;                    //!< значение атома, после calc()
   ExprPtr    _expr;                   //!< выражение, вычисл€ющее атом
   /*!
   ¬ычислить атом
   */
   void calc();
   /*!
   ”становить новое значение в атом
   */
   void set();
   /*!
   –аспечатать атом
   */
   string print();
};

#endif
