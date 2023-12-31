/*!            MindNames - �������� �������� � ����������, �������������� � ��������

(�) 2018-oct: steps: synset.com, qudata.com
****************************************************************************************/
#ifndef MindNamesH
#define MindNamesH

#include "Value.h"
#include "Variable.h"

#include <vector>

//=======================================================================================
//! �������� �������� � ����������, �������������� � ��������
//
class MindNames
{
public:
   vector<Value>       _consts;                   //!< ������ ��������
   vector<VariablePtr> _vars;                     //!< ������ ����������

   MindNames() {}

   void clear()
   {
      _consts.clear();   _vars.clear();
   }
   //------------------------------------------------------------------------------------
   //! �������� ������������� ���������� ��� ���������. ������ ���������� ���� � 1
   int get(ExprPtr ex);

   //------------------------------------------------------------------------------------
   //! �������� ������������� ���������
   int get(const Value & v)
   {
      for (size_t i = 0; i < _consts.size(); i++) {// ���� ����� ���������
         if ((_consts[i] == v) == Logic::True)
            return int(i);
      }
      _consts.push_back(v);                       // ����� ��������� - ���������
      return ((int)_consts.size()) - 1;
   }
   //------------------------------------------------------------------------------------
   //! �������� ������������� ����������
   int get(VariablePtr v)
   {
      for (size_t i = 0; i < _vars.size(); i++) { // ���� ����� ����������
         if (_vars[i] == v)
            return int(i);
      }
      _vars.push_back(v);                         // ����� ���������� - ���������
      return ((int)_vars.size()) - 1;
   }
};

#endif