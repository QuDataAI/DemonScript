/*!            MindAtom - ���� ������� � ������������� ���������� ����������

(�) 2019-apr: steps: synset.com, qudata.com
****************************************************************************************/

#ifndef MindAtomH
#define MindAtomH

#include "ValueGraph.h"
#include "MindVarAtom.h"

SPTR_DEF(MindAtom)
SPTR_WDEF(MindAtom)
SPTR_DEF(MindAxiom)
SPTR_WDEF(MindAxiom)

//=======================================================================================
//! ���� ������� � ������������� ���������� ����������
//
class MindAtom
{
public:
   MindAtom(Int edge, MindDemonPtr demon, vector<UInt> &atomVals, Value &vg1, Value &vg2);
   ~MindAtom();
   //! ��������� ����
   void calc(Value &graphValue);
   //! ��������� ����-demon 
   Logic calc_demon(Value &graphValue);
   //! ���������� ����� �������� �����
   void set(Value &graphValue, Logic val, int verbose = 0, const char* verbosePrf = "");
   //! ���������� ����� �������� ������
   void set_demon(Value &graphValue, Logic val);
   //! ������� ���� � ������ 
   string to_str(Graph &g, bool not_ = false, bool ignoreVal = false);
   int                     _edge;       //!< ���������, ���� == 0, �� ��� X==Y
   vector<UInt>            _vals;       //!< �������� ����������, ���� ��������
   Value                   _vg1, _vg2;  //!< �������� ������
   MindDemonPtr            _demon;      //!< ������������ �����
   vector<MindAxiomWPtr>   _axioms;     //!< ������ ������, ���������� ���� c ������������� ���������
   vector<MindAxiomWPtr>   _axioms_not; //!< ������ ������, ���������� ���� c ������������� ���������
   Logic                   _val;        //!< ���������� �������� �����
};


#endif
