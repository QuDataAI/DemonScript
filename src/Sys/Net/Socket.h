/*!               Socket - обмен данными по сети через механизм сокетов


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef SocketH
#define SocketH
#include "curl.h"
#include "OTypes.h"
#include <string>

using namespace std;

class SocketEventHandler;

//=======================================================================================
//! Сокет
class Socket
{   
public:
   Socket();
   ~Socket();
   /// Перечень ошибок при работе с сокетом
   enum SocketError
   {
      SOCKET_ERR_NONE = 0,             ///< No Error.
      SOCKET_ERR_PARAM = 1,            ///< Invalid parameter.
      SOCKET_ERR_TOO_MANY = 2,         ///< Maximum number of callbacks exceeded, or maximum number of sockets reached.
      SOCKET_ERR_ALREADY_REG = 3,      ///< The specified callback is already registered
      SOCKET_ERR_NOT_FOUND = 4,        ///< The specified callback was not found for removal
      SOCKET_ERR_UNAVAIL = 5,          ///< SOCKET is unavailable or feature is not supported.
      SOCKET_ERR_UNSUPPORTED = 7,      ///< The action requested is not supported
      SOCKET_ERR_WOULDBLOCK = 1000,    ///< The operation would block, please try again later.
      SOCKET_ERR_INPROGRESS = 1001,    ///< The operation is now in progress.
      SOCKET_ERR_ALREADY = 1002,       ///< The operation was already in progress.
      SOCKET_ERR_NOTSOCK = 1003,       ///< That object passed in is not a socket object.
      SOCKET_ERR_MSGSIZE = 1004,       ///< The UDP message is to long for the underlying layer.
      SOCKET_ERR_ADDRINUSE = 1005,     ///< The requested address is already in use.
      SOCKET_ERR_NETDOWN = 1006,       ///< The network is down.
      SOCKET_ERR_CONNRESET = 1007,     ///< The connection with the peer was reset.
      SOCKET_ERR_ISCONN = 1008,        ///< The socket is connected and the requested operation requires an unconnected socket
      SOCKET_ERR_NOTCONN = 1009,       ///< The socket is not connected and the requested operation requires a connected socket
      SOCKET_ERR_SHUTDOWN = 1010,      ///< The socket has been shutdown.
      SOCKET_ERR_TIMEDOUT = 1011,      ///< The requested operation timed out.
      SOCKET_ERR_CONNREFUSED = 1012,   ///< The connection was refused.
      SOCKET_ERR_UNKNOWN_HOST = 1013,  ///< The DNS system failed to lookup the hostname.
      SOCKET_ERR_NOTPERM = 1014,       ///< THe operation is not permitted
      SOCKET_ERR_UNCOTEGORIZED = 5000
   };

   /*!
   Установить соединение с сокетом
   \param hostName имя или адресс сервера
   \param port номер порта
   \param func функция которая будет вызвана при изменении статуса подключения
   \param userdata пользовательские данные, которые будут переданы в func параметром userdata
   \return результат соединения
   */
   bool connect(const char* hostName, int port, SocketEventHandler* handler, void* userdata);

   /*!
   Прервать соединение с сокетом
   */
   void disconnect();

   /*!
   Отправить в сокет данные
   \param buf указатель на начало данных
   \param len размер отправляемых данных
   \return число отправленных байт
   */
   size_t send(const char* buf, size_t len);

   /*!
   Получить данные из сокета
   \param buf указатель на начало данных
   \param len размер буфера
   \return число полученных байт
   */
   size_t receive(char* buf, size_t len);

   /*!
   Установлено ли соединение с сокетом
   \return состояние соединения
   */
   bool connected() { return _connected; }

   /*!
   Получить код ошибки установки соединения
   \return код ошибки установки соединения
   */
   SocketError errorCode() { return _errorCode; }

   /*!
   Получить строковое представление ошибки установки соединения
   \return строковое представление ошибки установки соединения
   */
   string errorString() { return _errorString; }

private:
   /*!
   Установить текущую ошибку
   \param errCode код ошибки CURL
   */
   void setError(CURLcode errCode);
   SocketEventHandler*      _handler;              //!< обработчик событий сокета
   CURL*                    _curl;                 //!< библиотека через которую выполняется соединение
   bool                     _connected;            //!< флаг наличия соединения
   curl_socket_t            _sockfd;               //!< дескриптор сокета
   SocketError              _errorCode;            //!< код ошибки
   string                   _errorString;          //!< строка ошибки
};

//=======================================================================================
//! Обработчик событий сокета
class SocketEventHandler {
public:
   /*!
   Соединение с сервером успешно установлено
   */
   void onConnectSuccess() {}
   /*!
   При установке соединения произошла ошибка
   */
   void onConnectError(Socket::SocketError errorCode, string &errorString) {}
};
#endif
