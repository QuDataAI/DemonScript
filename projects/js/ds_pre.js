"use strict";
if ("function" === typeof importScripts) {}
onmessage = (function(e) {
   if (e.data.kind == 'file')
   {
      //��������� ���������� � �������� ������� �++
      FS.writeFile(e.data.name,e.data.data);
   }
   else
   {
      //��������� �������� � �++
      var str = JSON.stringify(e.data);
      Module.postMessage(str);
   }
});

var global_DemonScript_instance = {};
global_DemonScript_instance.onMessage = function(msg){
 //��������� �������� � �������� �����
 postMessage(JSON.parse(msg));
}
