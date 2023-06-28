/*!            MindGraphModel - ������ �����, ��������� �� ������ �� ���� � ��� �����������

(�) 2018-oct: steps: synset.com, qudata.com
****************************************************************************************/

#ifndef MindGraphModelH
#define MindGraphModelH

#include "Value.h"
#include "ValueMap.h"

SPTR_DEF(MindGraphModel)

//=======================================================================================
//! ������ �����, ��������� �� ������ �� ���� � ��� �����������
//
class MindGraphModel
{
public:
   Float        _p;            //!< ����������� �����
   UInt         _n;            //!< ������� ��������� ��� ���������� �������� ���������������
   Value        _graphValue;   //!< �������� �����
   Value        _log;          //!< ��� ��������� ������
   MindGraphModel();
   MindGraphModel(const Value &gVal, Float p = 1.0, UInt n = 1);
   /*!
   ������� ������ ��� ������ � ����� ������ "p" - ����������� � "graph" - �������� �����
   \return ����� � ����� ������ "p" - ����������� � "graph" - �������� �����
   */
   Value get_Value();
   /*!
   ������� ����� ������
   \return ������ �� �����
   */
   MindGraphModelPtr copy();
   /*!
   �������� ��� � ������
   \return ������ �� �����
   */
   void pushLog(const string &str);
private:
   ValueMapPtr    _valMap;       //!< �������� ������
   ValueMapKey    _valKeyP;      //!< ���� � �����������
   ValueMapKey    _valKeyN;      //!< ���� � ������� ���������
   ValueMapKey    _valKeyGraph;  //!< ���� � �����   
   ValueMapKey    _valKeyLog;    //!< ���� � ���� ����������   
};

#endif