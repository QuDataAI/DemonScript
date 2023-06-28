/*!            MindMergeItem - ������ ����������� ������
�������� ������ ������-���������� �� ������� � ������ ������, ������� ������� ��������

(�) 2019-apr: steps: synset.com, qudata.com
****************************************************************************************/

#ifndef MindMergeItemH
#define MindMergeItemH

#include "MindAxioms.h"

//=======================================================================================
//!  ������ ����������� ������
//
class MindMergeItem
{
public:
   MindMergeItem(Value &graphVal, MindAxioms &graphAxioms, Value &excludeEdges);
   Value &              _graphVal;     //!< �������� �����
   MindAxioms &         _graphAxioms;  //!< ������� �����
   Value &              _excludeEdges; //!< ����� ������� ����� ��������� �� �������
   vector<MindAtomPtr>  _waitForCopy;  //!< �����-��������� �� ������� � ������ ����
   vector<MindLiteral>  _waitForCancel;//!< ��������, ������� ����� ��������
};


#endif
