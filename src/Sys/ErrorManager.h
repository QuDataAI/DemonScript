/*!               Менеджер ошибок интерпретатора

Менеджер принимает ошибки интерпретатора и записывает их в _lastErrorMsg и
сбрасывает флаг _lastErrorProcessed до вызова функции wasError

Для Unit тестов это дает возможность проверить из скрипта не произошла ли ошибка
Пример:

System.error()                               // сбрасываем флаг _lastErrorProcessed
var a = 5 / 0                                // ошибаемся
if System.error()                            // узнаем что ошиблись
{
   a = 0/5                                   // исправляемся
   out System.errorMsg()                     // выводим текст ошибки
}

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
*/

#ifndef ErrorManagerH
#define ErrorManagerH

#include "Singletone.h"
#include <string>
#include <vector>
using namespace std;

class ErrorManager
{
   IMPLEMENTATION_SINGLETONE(ErrorManager)
public:
   /*!
   Ошибка при работе сткрипта, о наличии которой можно позже спросить из модуля System.error()
   \param msg сообщение
   */
   void error(string msg);
   /*!
   Проверяем есть ли необработанная ошибка и сбрасываем флаг обработки
   \param msg сообщение
   */
   bool wasError(bool resetFlag = true) { 
      bool errorProcessed = _lastErrorProcessed;
      if (resetFlag)
         _lastErrorProcessed = true;
      return !errorProcessed;
   }
   /*!
   Текст последней ошибки скрипта
   \return текст последней ошибки скрипта
   */
   string lastErrorMsg() {
      return _lastErrorMsg;
   }

private:
   bool           _lastErrorProcessed = true;  //!< признак того, что ошибка обработана (используется при Unit-тесте)
   string         _lastErrorMsg;               //!< текст последней ошибки скрипта
};
//=======================================================================================
#endif

