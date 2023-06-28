/*!               TCPServer - windows-реализация протокола tcp-сервера

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef TCPServerWinH
#define TCPServerWinH

#include "TCPServer.h"
#include <WinSock2.h>

//=======================================================================================
//! TCP-сервер
class TCPServerWin : public TCPServer
{
   /*!
   Запустить сервер
   \return результат запуска
   */
   virtual bool start();
   /*!
   Прервать соединение с клиентом
   */
   virtual void disconnect(UInt clientID);
   /*!
   Отправить в сокет клиента данные
   \param clientID идентификатор клиента, которому нужно передать данные
   \param buf указатель на начало данных
   \param len размер отправляемых данных
   \return число отправленных байт
   */
   virtual size_t sendToClient(UInt clientID, const char* buf, size_t len);

   /*!
   Получить данные из сокета клиента
   \param clientID идентификатор клиента, от которого нужно получить данные
   \param buf указатель на начало данных
   \param len размер буфера
   \return число полученных байт
   */
   virtual Int receiveFromClient(UInt clientID, char* buf, size_t len);

   /*!
   Установлено ли соединение с клиентом
   \param clientID идентификатор клиента
   \return состояние соединения
   */
   virtual bool connected(UInt clientID);
private:
   SPTR_DEF(TCPServerClient)
   class TCPServerClient
   {
   public:
      TCPServerClient(SOCKET socket, SOCKADDR_IN addr);
      ~TCPServerClient();
      SOCKET      _socket;       //!< сокет клиента
      SOCKADDR_IN _addr;         //!< адрес клиента
   };
   /*!
   Ожидаем соединение с клиентом
   */
   void handle();
   /*!
   Регистрируем нового клиента
   */
   void registerClient(SOCKET socket, SOCKADDR_IN addr);

   SOCKET                     _socket;          //!< сокет соединения
   WSAData                    _wsaData;
   vector<TCPServerClientPtr> _clients;         //!< подключенные клиенты

};

#endif