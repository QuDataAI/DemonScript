/*!               Вспомогательные функции используемые скриптом

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
*/

#ifndef UtilsH
#define UtilsH


#if defined(_WIN64)
#define __WINDOWS__
#elif defined(_WIN32)
#define __WINDOWS__
#endif


#include "OTypes.h"
#include <string>
#include <vector>

using namespace std;

class Utils
{
public:
   /*!
   Получить расширение файла из полного имени
   \param fileName имя файла
   \return расширение
   */
   static string fileExt(const string &fileName);
   /*!
   Приостановить работу кода на ms
   \param ms время на которое приостанавливаем работу
   */
   static void sleep(UInt ms);
   /*!
   Генерация перестановок
   */
   static void permutations(vector<int>& ar, vector<vector<int>>& all, int lf);
};

#endif