/*!                        Инвариантные признаки графа

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef GraphInvariantsH
#define GraphInvariantsH
#include "OTypes.h"
#include <vector>

using namespace std;

//=======================================================================================
//! Инвариантные признаки графа
class GraphInvariants
{
public:
   bool           outdated       = true;  //!< параметры устарели
   int            vertexes       = 0;     //!< число вершин
   int            edges          = 0;     //!< число ребер
   vector<int>    vertexesDegree;         //!< упорядоченный массив степеней вершин графа
   vector<int>    kindTypes;              //!< упорядоченный массив типов ребер
   bool operator==(const GraphInvariants& other);
};
#endif
