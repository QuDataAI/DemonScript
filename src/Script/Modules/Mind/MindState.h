/*!            MindState - ������� ��������� ������� ������

(�) 2018-oct: steps: synset.com, qudata.com
****************************************************************************************/
#ifndef MindStateH
#define MindStateH

#include "MindNames.h"
#include "MindAxioms.h"
#include "MindTransaction.h"

SPTR_DEF(MindVarAxiom)

//=======================================================================================
//! ������� ��������� ������� ������
//
class MindState
{
public:
   enum MindStateMode {
      MODE_SET,      //!< ����������� ��������� �����
      MODE_CHECK,    //!< ����������� �������� ������������ �������
      MODE_COMPILE,  //!< ����������� ������ ������
   };
   MindState(MindStateMode mode, Value &graphValue, vector<UInt> &vals, MindNames & names, int max_num_sets = -1, int verbose = 0, bool show_false = false, int out_undef = 0);
   MindStateMode           _mode;                  //!< ������� �����
   Value &                 _graphValue;            //!< �������� ���� ������
   MindVarAxiomPtr         _axiom;                 //!< ������� �������
   vector<UInt>            _vals;                  //!< ������� �������� ����������
   MindNames &             _names;                 //!< ����� ����������
   int                     _verbose;               //!< ������� �������
   bool                    _show_false;            //!< ���������� ������
   int                     _out_undef;             //!< �������� �������������� �����
   bool                    _blocked;               //!< ����� ������������
   int                     _num_sets;              //!< ����� ����������� ����������
   int                     _max_num_sets;          //!< ������������ ����� ����������� ����������
   vector<MindTransaction> _pendingTransactions;   //!< ���������� ���������� � ������ MODE_CHECK
   MindAxioms*             _axioms;                //!< ������� ��� ������ MODE_COMPILE
   /*!
   �������� ���������� ��������� �����
   \param axiom ����������� �������
   \param atomIndex ������ �����, ������� ����� ���������� � ������
   */
   void addTransaction(MindVarAxiomPtr axiom, int atomIndex);
   /*!
   ���������� ���� � �������� ��������
   \param axiom ������� �����
   \param atom ����
   \param val ���������� �����
   */
   void setAtom(MindVarAxiomPtr axiom, int atomIndex, vector<UInt> &vals, bool val = true);
   /*!
   �������� ��������� ������������� �����
   \param axiom ������� �����
   \param atom ����
   \param val ���������� �����
   \return ��������� ������������� �����
   */
   string atomToStr(MindVarAxiomPtr axiom, int atomIndex, vector<UInt> &vals, bool val = true);
   /*!
   ������� ���������� � ���������� �����
   \param axiom ������� �����
   \param atom ����
   */
   void printSetAtom(MindVarAxiomPtr axiom, int atomIndex, vector<UInt> &vals);
   /*!
   ������������� ���������� �����
   */
   void setBlock();
};

#endif