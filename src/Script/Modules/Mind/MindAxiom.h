/*!            MindAxiom - �������, ���������� ������������ �����, ������� � ���� ������� ��������� ���������� ���

(�) 2019-apr: steps: synset.com, qudata.com
****************************************************************************************/

#ifndef MindAxiomH
#define MindAxiomH

#include "MindVarAxiom.h"
#include "MindLiteral.h"

SPTR_DEF(MindAxiom)

//=======================================================================================
//! ���� ������� ��� ����������� ������
//
class MindAxiom
{
public:
   MindAxiom();
   ~MindAxiom();
   //! ������� ������� � ������ 
   string to_str(Graph &g);
   vector<MindLiteral>  _literals;           //!< �������� �������
   Int                  _trueLiteralIndex;   //!< ������ �����, ������� ������� �������� ���� ��� ���������, ����� -1
};


#endif
