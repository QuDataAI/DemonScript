/*!            MindState - текущее состо€ние системы вывода

(с) 2018-oct: steps: synset.com, qudata.com
****************************************************************************************/
#ifndef MindStateH
#define MindStateH

#include "MindNames.h"
#include "MindAxioms.h"
#include "MindTransaction.h"

SPTR_DEF(MindVarAxiom)

//=======================================================================================
//! “екущее состо€ние системы вывода
//
class MindState
{
public:
   enum MindStateMode {
      MODE_SET,      //!< выполн€етс€ установка ребер
      MODE_CHECK,    //!< выполн€етс€ проверка срабатывани€ аксиомы
      MODE_COMPILE,  //!< выполн€етс€ сборка аксиом
   };
   MindState(MindStateMode mode, Value &graphValue, vector<UInt> &vals, MindNames & names, int max_num_sets = -1, int verbose = 0, bool show_false = false, int out_undef = 0);
   MindStateMode           _mode;                  //!< текущий режим
   Value &                 _graphValue;            //!< основной граф вывода
   MindVarAxiomPtr         _axiom;                 //!< текуща€ аксиома
   vector<UInt>            _vals;                  //!< текущие значени€ переменных
   MindNames &             _names;                 //!< имена переменных
   int                     _verbose;               //!< уровень отладки
   bool                    _show_false;            //!< отображать ошибки
   int                     _out_undef;             //!< выводить неопределенные атомы
   bool                    _blocked;               //!< вывод заблокирован
   int                     _num_sets;              //!< число добавленных транзакций
   int                     _max_num_sets;          //!< максимальное число добавленных транзакций
   vector<MindTransaction> _pendingTransactions;   //!< отложенные транзакции в режиме MODE_CHECK
   MindAxioms*             _axioms;                //!< аксиомы дл€ режима MODE_COMPILE
   /*!
   ƒобавить транзакцию установки атома
   \param axiom сработавша€ аксиома
   \param atomIndex индекс атома, который нужно установить в истину
   */
   void addTransaction(MindVarAxiomPtr axiom, int atomIndex);
   /*!
   ”становить атом в истинное значение
   \param axiom аксиома атома
   \param atom атом
   \param val истинность атома
   */
   void setAtom(MindVarAxiomPtr axiom, int atomIndex, vector<UInt> &vals, bool val = true);
   /*!
   ѕолучить строковое представление атома
   \param axiom аксиома атома
   \param atom атом
   \param val истинность атома
   \return строковое представление атома
   */
   string atomToStr(MindVarAxiomPtr axiom, int atomIndex, vector<UInt> &vals, bool val = true);
   /*!
   ¬ывести информацию о применении атома
   \param axiom аксиома атома
   \param atom атом
   */
   void printSetAtom(MindVarAxiomPtr axiom, int atomIndex, vector<UInt> &vals);
   /*!
   «аблокировать дальнейший вывод
   */
   void setBlock();
};

#endif