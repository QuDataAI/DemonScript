/*!            MindVarAxiom - ������� ������� �� ������, ���������� ���������� ���

����� ������� ��������� �����������, ������� ������������ � ������ ������

(�) 2018-oct: steps: synset.com, qudata.com
****************************************************************************************/
#ifndef MindVarAxiomH
#define MindVarAxiomH

#include "MindVarAtom.h"
#include "MindNames.h"
#include "Expr.h"

class MindState;

SPTR_DEF(MindVarAxiom)

//=======================================================================================
//! ������� ������� �� ������, ���������� ���������� ���
//
class MindVarAxiom
{
public:
   Float              _p;                         //!< ����������� �������
   UInt               _num_vars;                  //!< ����� ���������� � �������
   UInt               _index;                     //!< ���������� ����� ������� � ����� *.ds
   UInt               _num_used;                  //!< ������� ��� ���� ���������
   UInt               _num_checked;               //!< ������� ��� ���� ������������
   Int                _group;                     //!< � ����� ������ ������ ���������
   bool               _all_vars_ok;               //!< ������� ������� ��� ���� ����������
   vector<MindVarAtom>_atoms;                     //!< �����, ���������� ���������� ���
   ExprPtr            _axiomExpr;                 //!< �������� ��������� ������� ��� ������ �������
   int                _notSplittedAtoms;          //!< ����� ������������� ������ � �������
   MindVarAxiomPtr    _parent;                    //!< ������������ ������� (������������ ��� ����������� ������)

   MindVarAxiom();

   //! �������� �� ��������� ex �������� � ���������� �������
   Bool is_valid(ExprPtr ex);
   //! �������� �� ��������� ex ���������, �.�. X.@rel.Y, X==Y ��� �� ���������
   Bool is_atom(ExprPtr ex);
   //! �������� �� ��������� ex ������  �������� ��������  kind � �����
   Bool is_kind(ExprPtr ex, Expr::Kind kind);
   /*!
   ��������� ��� ����� ��������� ex � ������ _atoms
   \param ex ��������� ��� ��������
   \param not_ ��������� �����
   \param p ����������� ����� (������������ � �������� "usual")
   \param names ����� ����������
   */
   void get_atoms(ExprPtr ex, Bool not_, Logic p, MindNames & names);
   void get_atom(ExprPtr lf, ExprPtr rt, MindNames & names, MindVarAtom &a);

   //! ������������� ��������� � ������ ������ _atoms (���)
   Bool set(ExprPtr ex, MindNames & names);

   //! ������� ���������� ���������� � ���� A & B ... -> C
   string to_str(Graph &g, vector<UInt> &vals, MindNames & names, int id, bool val = true);
   //! ������� ���������� ���������� � ��� �����
   string to_str_cnf(Graph &g, vector<UInt> &vals, MindNames & names);
   //! ��������� ������� � ����������� vals,  return -1, ���� ���� ������������
   Int check_rule1(MindState &state);
   //! ��������� ������� � ����������� vals, � �������� ������
   Int check_rule2(MindState &state);
   //! ��� ���������� �� ����� ������
   friend bool  operator <  (const MindVarAxiom &x, const MindVarAxiom &y)
   {
      return x._num_vars < y._num_vars;
   }
};

#endif