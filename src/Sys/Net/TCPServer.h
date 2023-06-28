/*!               TCPServer - платформонезависимый интерфейс сервера подключения tcp-клиентов

Пока используется для удаленной отладки скрипта внешним редактором Visual Studio Code
Редактор подключается на определенный порт и передает скрипту отладочные команды

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef TCPServerH
#define TCPServerH

#include "OTypes.h"
#include <vector>
#include <memory>

using namespace std;

class TCPServerListener;

//=======================================================================================
//! TCP-сервер
class TCPServer
{
public:
   TCPServer();
   ~TCPServer();
   /*!
   Создать экземпляр сервера
   \return указатель на объект сервера
   */
   static shared_ptr<TCPServer> create();
   /*!
   Инициализировать сервер
   \param port номер порта на котором сервер будет слушать входящие соединения
   \param maximumClients максимальное число соединений, которые сервер может принять
   \param listener слушатель результатов подключений
   \return результат инициализации
   */
   bool init(UInt16 port, int maximumClients, TCPServerListener *listener);

   /*!
   Запустить сервер
   \return результат запуска
   */
   virtual bool start() { return false; }

   /*!
   Прервать соединение с клиентом
   */
   virtual void disconnect(UInt clientID) {}

   /*!
   Отправить в сокет клиента данные
   \param clientID идентификатор клиента, которому нужно передать данные
   \param buf указатель на начало данных
   \param len размер отправляемых данных
   \return число отправленных байт
   */
   virtual size_t sendToClient(UInt clientID, const char* buf, size_t len) { return 0; }

   /*!
   Получить данные из сокета клиента
   \param clientID идентификатор клиента, от которого нужно получить данные
   \param buf указатель на начало данных
   \param len размер буфера
   \return число полученных байт
   */
   virtual Int receiveFromClient(UInt clientID, char* buf, size_t len) { return 0; }

   /*!
   Установлено ли соединение с клиентом
   \param clientID идентификатор клиента
   \return состояние соединения
   */
   virtual bool connected(UInt clientID) { return false; }

protected:
   UInt16                    _port;            //!< порт на который сервер будет ждать подключения
   UInt                      _maximumClients;  //!< максимальное число клиентов, которым разрешено подключение
   TCPServerListener*        _listener;        //!< обработчик событий сервера
   UInt                      _clientCount;     //!< число клиентов
};

class TCPServerListener {
public:
   virtual ~TCPServerListener() {}
   /*!
   Соединение с клиентом успешно установлено
   */
   virtual void onClientConnect(int clientID) {}
   /*!
   Соединение с клиентом прервано
   */
   virtual void onClientDisconnect(int clientID) {}
};

#endif
