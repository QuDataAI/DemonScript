'use strict';
if('function' === typeof importScripts) {         // подключаем внешние модули
//importScripts('queue.js');                      // должны быть, где и worker.js
}
var workerFinderSolution = new FinderSolution();
/****************************************************************************************
* Функция, получающая сообщения из внешнего управляющего модуля (html-страницы)
* e.data - содержит структуру данных, необходимых для вычислений
*/
onmessage = function(e)
{
   console.log('Message received from main script');
   switch(e.data.kind){                           // разновидность сообщения
      case "start":                               // начало вычислений
         workerFinderSolution.prog = e.data.st;
         workerFinderSolution.timer();            // запускаем таймер
         break;
   }
}
/****************************************************************************************
* 
*/
function FinderSolution()
{
}
/****************************************************************************************
* Таймерная функций
*/
FinderSolution.prototype.timer = function ()
{
   postMessage({kind:"end",  st: this.prog});
}
