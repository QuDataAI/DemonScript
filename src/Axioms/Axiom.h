/*!                           Аксиома

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
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
   Парсим аксиому из выражения
   \param ex выражение, содержащее аксиому
   */
   bool parse(ExprPtr ex);
   /*!
   Проверяем аксиому
   \return true - если нет ошибок при проверке аксиомы
   */
   bool check();
   /*!
   Выводим аксиому
   \return аксиома
   */
   string print();
private:
   /*!
   Парсим атомы аксиомы
   \param arg аргумент, содержащий атомы аксиомы
   \param _not флаг отрицания 
   */
   void parseAtoms(ExprPtr ex, bool _not);
   UInt              _id = 0;          //!< идентификатор аксиомы
   vector<Atom>      _atoms;           //!< атомы, соединённые логическим ИЛИ
   ExprPtr           _axiomExpr;       //!< исходное выражение аксиомы для вывода отладки
};

#endif
