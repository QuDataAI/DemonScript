"use strict";
if ("function" === typeof importScripts) {}
onmessage = (function(e) {
   if (e.data.kind == 'file')
   {
      //добавляем содержимое в файловую систему с++
      FS.writeFile(e.data.name,e.data.data);
   }
   else
   {
      //транзитом передаем в с++
      var str = JSON.stringify(e.data);
      Module.postMessage(str);
   }
});

var global_DemonScript_instance = {};
global_DemonScript_instance.onMessage = function(msg){
 //транзитом передаем в основной поток
 postMessage(JSON.parse(msg));
}
