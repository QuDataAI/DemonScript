/*!            MindState - ���������� ������� ������ (��� ����������� ����������)

(�) 2018-oct: steps: synset.com, qudata.com
****************************************************************************************/
#ifndef MindTransactionH
#define MindTransactionH

#include "MindVarAxiom.h"

SPTR_DEF(MindVarAxiom)

//=======================================================================================
//! ���������� ������� ������ (��� ����������� ����������)
//
class MindTransaction
{
public:
   MindTransaction(MindVarAxiomPtr axiom, UInt atomIndex, vector<UInt> &vals);
   MindVarAxiomPtr  _axiom;      //!< ������ �� �������, ������� ���������
   UInt             _atomIndex;  //!< ������ ����� ��� ����������� ����������
   vector<UInt>     _vals;       //!< �������� ����������
};

#endif