#include "Model.h"
//=======================================================================================
//                          Процедурные обыденные знания
//=======================================================================================
// Отношение "x внутри y" (x внутри контейнера y с учётом транзитивности)
//
Logic Model::inside(Node &x, Node &y)
{
   if (x == y)                              // сам в себе находится не может
      return Logic(1,0);

   if (path(x, "in", y)   == Logic(0,1))    // y находится выше x
      return Logic(0,1);

   if (path(y, "in", x)   == Logic(0,1))    // x находится выше y
      return Logic(1,0);

   if (common("in", x, y) == Logic(0,1))    // имеют общего предка
      return Logic(1,0);

   return Logic(0, 0);                      // не известно
}
//=======================================================================================
// Отношение "x в y" (x непосредствено вложен в контейнер y)
//
Logic Model::in(Node &x, Node &y)
{
   if(x == y)                               // сам в себе находится не может
      return Logic(1,0);

   if( val(x, "in", y) != Logic(0,0) )      // явно находится или есть запрет
      return val(x, "in", y);

   if (val(y, "in", x) == Logic(0,1) )      // наоборот, y непосредственно в x
      return Logic(1,0);

   vector<Edge> objs; get_out(x,"in",objs); // все выходящие из x in-рёбра
   for (UInt i = 0; i < objs.size(); i++)   // если x в ком-то есть,
      if(val(x, "in", objs[i]).p1() > 0)    // то он не может быть в y
         return Logic(1, 0);                // (x,in,y) сработало выше

   if (path(x,"in",y) == Logic(0,1) )       // x находится выше y
      return Logic(1,0);

   if (path(y,"in", x) == Logic(0,1) )      // y находится выше x
      return Logic(1,0);

   if (common("in", x, y) == Logic(0,1) )   // есть общий предок на дереве
      return Logic(1,0);

   return Logic(0,0);                       // не известно
}
//=======================================================================================
// Является ли x пустым контейнером
//
Logic Model::empty(Node &x)
{
   Float p = 0; vector<Edge> objs;
   get_in(x, "in", objs);                   // все входящие в x in-рёбра
   for (UInt i = 0; i < objs.size(); i++)
      p = Max(p,  val(objs[i], "in", x).p1());
   return Logic(p,0);
}
//=======================================================================================
// Отношение "x лежит на y"
//
Logic Model::on(Node &x, Node &y)
{
   if (val(x, "on", y) != Logic(0, 0))      // явно x находится на y или есть запрет
      return val(x, "on", y);

   if (val(y, "on", x) != Logic(0, 0))      // наоборот, y на x
      return !val(y, "on", x);

//   if( inside() )

   return Logic(0,0);                       // не известно
}
//=======================================================================================
// Отношение "x находится выше y"
//
Logic Model::above(Node &x, Node &y)
{
   if( on(x,y) != Logic(0,0) )
      return on(x,y);

   if( inside(x,y).p1() > 0 )              // если один находится в другом,
      return Logic(1,0);                   // то они не могут лежать друг на друге

   if( inside(y,x).p1() > 0 )
      return Logic(1,0);

   return Logic(0,0);                       // не известно
}
//=======================================================================================
// Отношение "x находится ниже y"
//
Logic Model::below(Node &x, Node &y)
{
   if( on(x,y) != Logic(0,0) )
      return !on(x,y);

   if( inside(x,y).p1() > 0 )              // если один находится в другом,
      return Logic(1,0);                   // то они не могут лежать друг на друге

   if( inside(y,x).p1() > 0 )
      return Logic(1,0);

   return Logic(0,0);                       // не известно
}
//=======================================================================================
