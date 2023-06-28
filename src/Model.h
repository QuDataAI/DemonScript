/*!      


                             (с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef ModelH
#define ModelH
#include "OTypes.h"
#include "Logic.h"
#include "Graph.h"
//=======================================================================================
//! 
//
class Model : public Graph
{
public:
   //! Отношение "x внутри y" (x внутри контейнера y с учётом транзитивности)
   Logic inside(Node &x, Node &y);
   //! Отношение "x в y" (x непосредсвено вложен в контейнер y)
   Logic in    (Node &x, Node &y);
   //! Является ли x пустым контейнером
   Logic empty (Node &x);

   //! Отношение "x лежит на y"
   Logic on    (Node &x, Node &y);
   //! Отношение "x находится выше y"
   Logic above (Node &x, Node &y);
   //! Отношение "x находится ниже y"
   Logic below (Node &x, Node &y);

};
//=======================================================================================
#endif
