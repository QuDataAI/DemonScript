/*!                        ������������ �������� �����

(�) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef GraphInvariantsH
#define GraphInvariantsH
#include "OTypes.h"
#include <vector>

using namespace std;

//=======================================================================================
//! ������������ �������� �����
class GraphInvariants
{
public:
   bool           outdated       = true;  //!< ��������� ��������
   int            vertexes       = 0;     //!< ����� ������
   int            edges          = 0;     //!< ����� �����
   vector<int>    vertexesDegree;         //!< ������������� ������ �������� ������ �����
   vector<int>    kindTypes;              //!< ������������� ������ ����� �����
   bool operator==(const GraphInvariants& other);
};
#endif
