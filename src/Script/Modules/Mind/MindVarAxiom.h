/*!            MindVarAxiom - аксиома состоит из атомов, соединённых логическим ИЛИ

Атомы аксиомы оперируют переменными, которые перебираются в момент вывода

(с) 2018-oct: steps: synset.com, qudata.com
****************************************************************************************/
#ifndef MindVarAxiomH
#define MindVarAxiomH

#include "MindVarAtom.h"
#include "MindNames.h"
#include "Expr.h"

class MindState;

SPTR_DEF(MindVarAxiom)

//=======================================================================================
//! Аксиома состоит из атомов, соединённых логическим ИЛИ
//
class MindVarAxiom
{
public:
   Float              _p;                         //!< вероятность аксиомы
   UInt               _num_vars;                  //!< число переменных в аксиоме
   UInt               _index;                     //!< порядковый номер аксиомы в файле *.ds
   UInt               _num_used;                  //!< сколько раз была применена
   UInt               _num_checked;               //!< сколько раз была использована
   Int                _group;                     //!< к какой группе аксиом относится
   bool               _all_vars_ok;               //!< аксиома истинна при всех переменных
   vector<MindVarAtom>_atoms;                     //!< атомы, соединённые логическим ИЛИ
   ExprPtr            _axiomExpr;                 //!< исходное выражение аксиомы для вывода отладки
   int                _notSplittedAtoms;          //!< число неразделенных атомов в аксиоме
   MindVarAxiomPtr    _parent;                    //!< родительская аксиома (используется для разделенных аксиом)

   MindVarAxiom();

   //! Является ли выражение ex аксиомой в правильном формате
   Bool is_valid(ExprPtr ex);
   //! Является ли выражение ex атомарным, т.е. X.@rel.Y, X==Y или их отрицание
   Bool is_atom(ExprPtr ex);
   //! Содержит ли выражение ex только  бинарные операции  kind и атомы
   Bool is_kind(ExprPtr ex, Expr::Kind kind);
   /*!
   Поместить все атомы выражения ex в вектор _atoms
   \param ex выражение для парсинга
   \param not_ отрицание атома
   \param p вероятность атома (используется в аксиомах "usual")
   \param names имена переменных
   */
   void get_atoms(ExprPtr ex, Bool not_, Logic p, MindNames & names);
   void get_atom(ExprPtr lf, ExprPtr rt, MindNames & names, MindVarAtom &a);

   //! Преобразовать выражение в список атомов _atoms (КНФ)
   Bool set(ExprPtr ex, MindNames & names);

   //! Вывести отладочную информацию в виде A & B ... -> C
   string to_str(Graph &g, vector<UInt> &vals, MindNames & names, int id, bool val = true);
   //! Вывести отладочную информацию в КНФ форме
   string to_str_cnf(Graph &g, vector<UInt> &vals, MindNames & names);
   //! Применить аксиому с переменными vals,  return -1, если было противоречие
   Int check_rule1(MindState &state);
   //! Применить аксиому с переменными vals, в нечёткой логике
   Int check_rule2(MindState &state);
   //! Для сортировки по числу аксиом
   friend bool  operator <  (const MindVarAxiom &x, const MindVarAxiom &y)
   {
      return x._num_vars < y._num_vars;
   }
};

#endif