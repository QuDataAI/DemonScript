/*!            MindLiteral - ������� �������, ���������� ���� � ���������� ���� ���

(�) 2019-apr: steps: synset.com, qudata.com
****************************************************************************************/

#ifndef MindLiteralH
#define MindLiteralH

#include "MindAtom.h"

//=======================================================================================
//! ���� ������� � ������������� ���������� ����������
//
class MindLiteral
{
public:
   MindLiteral() {}
   MindLiteral(MindAtomWPtr atom, bool not_);
   //! ������� ������� � ������ 
   string to_str(Graph &g, bool not_ = false, bool ignoreVal = false);
   MindAtomWPtr   _atom;         //!< ���� �������
   bool           _not = false;  //!< ���� �� ��������� ����� ������
};


#endif
