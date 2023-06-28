#include "Utils.h"

#ifdef __WINDOWS__
#include <windows.h>
#else
#include <unistd.h>
#endif


string Utils::fileExt(const string & fileName)
{
   return fileName.substr(fileName.find_last_of(".") + 1);
}

void Utils::sleep(UInt ms)
{
#ifdef __WINDOWS__
   Sleep((DWORD)ms);
#else
   usleep(ms * 1000);   // usleep takes sleep time in us (1 millionth of a second)
#endif
}

void Utils::permutations(vector<int>& ar, vector<vector<int>>& all, int lf)
{
   static auto swap = [](vector<int>& ar, int i, int j)
   {
      int a = ar[i];
      ar[i] = ar[j];
      ar[j] = a;
   };

   if (lf >= ar.size()) {                                // перестановки окончены
      all.push_back(ar);
      return;
   }
   permutations(ar, all, lf + 1);                        // перестановки элементов справа от lf
   int i = lf + 1;
   while (i < ar.size()) {                               // теперь каждый элемент ar[i], i > lf
      swap(ar, lf, i);                                   // меняем местами с ar[lf]
      permutations(ar, all, lf + 1);                     // и снова переставляем всё справа
      swap(ar, lf, i);                                   // возвращаем элемент ar[i] назад
      i = i + 1;
   }
}
