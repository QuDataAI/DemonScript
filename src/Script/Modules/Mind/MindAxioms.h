/*!            MindAxioms - ������ ������ �������� �����

(�) 2019-apr: steps: synset.com, qudata.com
****************************************************************************************/

#ifndef MindAxiomsH
#define MindAxiomsH

#include "MindAxiom.h"

class MindState;

SPTR_DEF(MindAxioms)

//=======================================================================================
//! C����� ������ �������� �����
//
class MindAxioms
{
public:
   MindAxioms();
   ~MindAxioms();
   vector<MindAtomPtr>  _atoms;        //!< ������ ������ ������
   vector<MindAxiomPtr> _axioms;       //!< ������ ������ �����
   /*!
   �������� ����� ������� �� ���������� � ������� ���������� ����������
   \param state ������� ��������� Mind
   */
   void add(MindState &state);
   /*!
   �������� ����������� ���� �� �����������, ����
   ����� �������� � ������� ������, ����� ������, ����� ������� �����
   \param varAxiom ���������� �������
   \param state ������� ��������� Mind
   */
   MindAtomPtr getAtom(MindVarAtom & varAtom, MindState &state);
   /*!
   �������� ����������� ���� �����, ����
   ����� �������� � ������� ������, ����� ������, ����� ������� �����
   \param varAxiom ���������� �������
   \param state ������� ��������� Mind
   */
   MindAtomPtr getEdgeAtom(int edge, UInt n1, UInt n2, Value &vg2);
   /*!
   ��������� ����� ������� ���������
   */
   void check();
private:

};


#endif
