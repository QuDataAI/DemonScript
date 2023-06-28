/*!            MindVarAtom - ������������ ����� ������� ����� ��������� ��������� (��� ���), ����� ��������:

   1) ��������� (��������) X.@E.Y
   2) ���������� X==Y
   3) ������� isa: X.isa(Y)
   4) ������������ ������� fun(X,Y,Z,..)

���� ��������� �����������, �������� ������� ������������ � ������ ������

(�) 2018-oct: steps: synset.com, qudata.com
****************************************************************************************/

#ifndef MindVarAtomH
#define MindVarAtomH

#include "MindDemon.h"
#include "MindNames.h"
#include "Value.h"
#include "Logic.h"

SPTR_DEF(MindVarAtom)

//=======================================================================================
//! ���� - ������������ ����� ������� ����� ��������� ��������� (��� ���)
//
class MindVarAtom
{
public:
   bool         _not;        //!< ���� �� ����� ������ ���������
   int          _edge;       //!< ���������, ���� ==0, �� ��� X==Y
   int          _x1, _x2;    //!< ����� ����������, ���� <=, �� ��� ���������
   MindDemonPtr _demon;      //!< ������������ �����
   Logic        _val;        //!< �������� �����, ����� calc()
   Logic        _p;          //!< �������� ����������� ����� ��� "usual" ������ 
   bool         _needSplit;  //!< ������� ����, ��� ���� ����� ��������� ���-��� ��� ����������� �� ����� 1 (_p!=Logic::True)

   MindVarAtom();
   MindVarAtom(const MindVarAtom &a) { _demon = 0;  copy(a); }

   //! ��������� ����-demon ��� ������ �������� ���������� vals
   Logic calc_demon(Value &graphValue, vector<UInt> &vals, MindNames & names, Logic *value = 0);
   //! ��������� ���� ��� ������ �������� ���������� vals
   void calc(Value &graphValue, vector<UInt> &vals, MindNames & names);

   //! �����������
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
            _demon->_set = true;                  // ����� � ������ set
      }
   }
   //! ���������� ������������
   MindVarAtom & operator = (const MindVarAtom & a)
   {
      copy(a);
      return *this;
   }
   //! �������������� ������
   friend bool  operator <  (const MindVarAtom &x, const MindVarAtom &y)
   {
      if (x._not == y._not)                       // ������� ��� � �����������
         return x._edge < y._edge == Logic::True; // � � ��� �� ����� �����
      else
         return x._not > y._not;
   }

   //! ������� ���� ��� ����� g �� ���������� ���������� vals � �������� names, 
   //! �������� ����� ��� !, ���� not_==true
   string to_str(Graph &g, vector<UInt> &vals, MindNames & names, bool not_);
};

#endif