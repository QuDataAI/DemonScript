/*!            MindMerge - ������ ����������� ����������� ���� ������ ��������� �������:
�� ��������� ������, ��������� �������� ������:
(F F F F T)
(F F F T)
...
����� ������� �����, ������� �������� �������� � ����� ������
� �� �������� ����������� � Undef
����� ���������� ���������� ������ ��� ������, � ������� ����� ������� �������� � False �� Undef

����� ������ �������� �������� ���������� ������������ ����� � ��������������� ����, 
���� ��� � ��� ������������

(�) 2019-apr: steps: synset.com, qudata.com
****************************************************************************************/

#ifndef MindMergeH
#define MindMergeH

#include "MindMergeItem.h"

//=======================================================================================
//!  ������, ����������� ����������� ���� ������
//
class MindMerge 
{
public:
   MindMerge(Value &graph1Val, MindAxioms &graph1Axioms, Value &graph2Val, MindAxioms &graph2Axioms, Value &excludeEdges, int verbose);
   /*!
   ���������� � �����������:
   - �������� ������ ������-���������� �� �����������
   - �������� ������ ������ ������� �������� ��������
   */
   void init();
   /*!
   �������� ������������� ��������� �����������:
   */
   bool need();
   /*!
   ��������� �����������
   */
   void run();
private:
   MindMergeItem _mergeItem1;  //!< ������ �����������
   MindMergeItem _mergeItem2;  //!< ������ �����������
   /*!
   ������������  ����� ����� ���� �� ���� �������� ���������� ���������, ������� 
   ��������� � ��
   */
   void addSingleAtoms();
   /*!
   �������� ����� �� ������ ����������
   */
   void addExcludeAtoms();
   /*!
   ���������� �������� ��������� ������, �� ������ "WAIT_FOR_CANCEL";
   */
   bool processWaitForCancel(MindMergeItem &mergeItem);
   /*!
   ��������� �� ���������� ����� � ��������������� ����
   */
   void transferWaitForCopy(MindMergeItem &mergeItem1, MindMergeItem &mergeItem);

   int   _verbose;   //!< ������� �������
};


#endif
