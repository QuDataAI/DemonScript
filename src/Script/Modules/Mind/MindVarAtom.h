/*!            MindVarAtom - элементарная часть аксиомы может содержать отрицание (или нет), после которого:

   1) отношение (предикат) X.@E.Y
   2) равенстово X==Y
   3) функция isa: X.isa(Y)
   4) произвольная функция fun(X,Y,Z,..)

Атом оперирует переменными, значения которых перебираются в момент вывода

(с) 2018-oct: steps: synset.com, qudata.com
****************************************************************************************/

#ifndef MindVarAtomH
#define MindVarAtomH

#include "MindDemon.h"
#include "MindNames.h"
#include "Value.h"
#include "Logic.h"

SPTR_DEF(MindVarAtom)

//=======================================================================================
//! Атом - элементарная часть аксиомы может содержать отрицание (или нет)
//
class MindVarAtom
{
public:
   bool         _not;        //!< есть ли перед атомом отрицание
   int          _edge;       //!< отношение, если ==0, то это X==Y
   int          _x1, _x2;    //!< номер переменной, если <=, то это константа
   MindDemonPtr _demon;      //!< произвольный демон
   Logic        _val;        //!< значение атома, после calc()
   Logic        _p;          //!< заданная вероятность атома для "usual" аксиом 
   bool         _needSplit;  //!< признак того, что атом нужно разделить так-как его вероятность не равна 1 (_p!=Logic::True)

   MindVarAtom();
   MindVarAtom(const MindVarAtom &a) { _demon = 0;  copy(a); }

   //! Вычислить атом-demon для данных значений переменных vals
   Logic calc_demon(Value &graphValue, vector<UInt> &vals, MindNames & names, Logic *value = 0);
   //! Вычислить атом для данных значений переменных vals
   void calc(Value &graphValue, vector<UInt> &vals, MindNames & names);

   //! копирование
   void copy(const MindVarAtom & a)
   {
      _not = a._not; _edge = a._edge;
      _x1 = a._x1;  _x2 = a._x2;
      _val = a._val;
      _p = a._p;
      _needSplit = a._needSplit;
      if (_demon) {
         _demon = 0;
      }
      if (a._demon) {
         _demon = SPTR_MAKE(MindDemon)();
         _demon->_fun = a._demon->_fun;
         _demon->_vars = a._demon->_vars;
         if (_demon->_fun->_codeSet->_commands.size() > 0)
            _demon->_set = true;                  // демон с блоком set
      }
   }
   //! перегрузка присваивания
   MindVarAtom & operator = (const MindVarAtom & a)
   {
      copy(a);
      return *this;
   }
   //! упорядочивание атомов
   friend bool  operator <  (const MindVarAtom &x, const MindVarAtom &y)
   {
      if (x._not == y._not)                       // сначала все с отрицаниями
         return x._edge < y._edge == Logic::True; // и в них по имени ребра
      else
         return x._not > y._not;
   }

   //! Вывести атом для графа g со значениями переменных vals и констнат names, 
   //! поставив перед ним !, если not_==true
   string to_str(Graph &g, vector<UInt> &vals, MindNames & names, bool not_);
};

#endif