/*!            MindDemon - �����, ����������� � �������� ����� �������

(�) 2018-oct: steps: synset.com, qudata.com
****************************************************************************************/
#ifndef MindDemonH
#define MindDemonH

#include "OTypes.h"
#include "Function.h"
#include <vector>

using namespace std;

SPTR_DEF(MindDemon)
SPTR_DEF(FunDemon)

//=======================================================================================
//! �����, ����������� � �������� ����� �������
//
class MindDemon
{
public:
   FunDemonPtr   _fun;                           //!< ����������� �������
   bool          _set;                           //!< ���� ������ set
   vector<int>   _vars;                          //!< ����������, ���� <=0 - �� ���������

   MindDemon() { _fun = 0;  _set = false; }
};

#endif