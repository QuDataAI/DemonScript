/*!               Debugger - базовый класс клиента отладки


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef DebuggerClientH
#define DebuggerClientH

class DebuggerMessenger;

class DebuggerClient
{
public:
   DebuggerClient(DebuggerMessenger *messenger) : _messenger(messenger) {}
   /*!
   Запустить отладчик
   */
   virtual void run() = 0;
protected:
   DebuggerMessenger * _messenger;                                   //!< обменник сообщениями между потоком приложения и клиентом отладки
};

#endif