/*!                           ���� �������

(�) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef AtomH
#define AtomH

#include "OTypes.h"
#include "Logic.h"
#include <memory>

SPTR_DEF(Expr)

class Atom {
public:
   bool       _not = false;            //!< ���� �� ����� ������ ���������
   Logic      _val;                    //!< �������� �����, ����� calc()
   ExprPtr    _expr;                   //!< ���������, ����������� ����
   /*!
   ��������� ����
   */
   void calc();
   /*!
   ���������� ����� �������� � ����
   */
   void set();
   /*!
   ����������� ����
   */
   string print();
};

#endif
