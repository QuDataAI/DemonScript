/*!                           �������

(�) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef AxiomH
#define AxiomH

#include "Atom.h"
#include <vector>

class Axiom {
public:
   Axiom() {}
   Axiom(UInt id) :_id(id) {}
   /*!
   ������ ������� �� ���������
   \param ex ���������, ���������� �������
   */
   bool parse(ExprPtr ex);
   /*!
   ��������� �������
   \return true - ���� ��� ������ ��� �������� �������
   */
   bool check();
   /*!
   ������� �������
   \return �������
   */
   string print();
private:
   /*!
   ������ ����� �������
   \param arg ��������, ���������� ����� �������
   \param _not ���� ��������� 
   */
   void parseAtoms(ExprPtr ex, bool _not);
   UInt              _id = 0;          //!< ������������� �������
   vector<Atom>      _atoms;           //!< �����, ���������� ���������� ���
   ExprPtr           _axiomExpr;       //!< �������� ��������� ������� ��� ������ �������
};

#endif
