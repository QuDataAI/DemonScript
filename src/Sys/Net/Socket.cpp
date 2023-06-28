#include "Socket.h"

Socket::Socket():
_curl(NULL),
_connected(false),
_handler(NULL),
_errorCode(SOCKET_ERR_NONE)
{
}

Socket::~Socket()
{
   disconnect();
}

bool Socket::connect(const char * hostName, int port, SocketEventHandler* handler, void * userdata)
{
   CURLcode res;

   _handler = handler;
   _curl    = curl_easy_init();

   string url = hostName + string(":") + std::to_string(port);

   if (_curl) {
      curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
      /* Do not do the transfer - only connect to host */
      curl_easy_setopt(_curl, CURLOPT_CONNECT_ONLY, 1L);
      res = curl_easy_perform(_curl);

      if (res != CURLE_OK) {
         setError(res);
         if (_handler)
         {
            _handler->onConnectError(_errorCode, _errorString);
         }
         return false;
      }

      /* Extract the socket from the curl handle - we'll need it for waiting. */
      res = curl_easy_getinfo(_curl, CURLINFO_ACTIVESOCKET, &_sockfd);

      if (res != CURLE_OK) {
         setError(res);
         if (_handler)
         {
            _handler->onConnectError(_errorCode, _errorString);
         }
         return false;
      }
      _connected = true;
   }
   else
   {
      _errorCode   = SOCKET_ERR_UNSUPPORTED;
      _errorString = "Socket is unsuppoted";
      if (_handler)
      {
         _handler->onConnectError(_errorCode, _errorString);
      }
      return false;
   }

   if (_handler)
   {
      _handler->onConnectSuccess();
   }
   return true;
}

void Socket::disconnect()
{
   if (_curl)
   {
      curl_easy_cleanup(_curl);
   }
   _curl = 0;
}

void Socket::setError(CURLcode errCode)
{
#define CURL_TO_SOCKET_ERR_CODE(curlErrCode,s3eSocketErrCode) case curlErrCode: socketError = s3eSocketErrCode;  break;
   SocketError socketError = SOCKET_ERR_NONE;

   switch (errCode)
   {
      CURL_TO_SOCKET_ERR_CODE(CURLE_OK,                     SOCKET_ERR_NONE);
      CURL_TO_SOCKET_ERR_CODE(CURLE_AGAIN,                  SOCKET_ERR_WOULDBLOCK);
      CURL_TO_SOCKET_ERR_CODE(CURLE_UNSUPPORTED_PROTOCOL,   SOCKET_ERR_UNSUPPORTED);
      CURL_TO_SOCKET_ERR_CODE(CURLE_OPERATION_TIMEDOUT,     SOCKET_ERR_TIMEDOUT);
      CURL_TO_SOCKET_ERR_CODE(CURLE_COULDNT_CONNECT,        SOCKET_ERR_NOTCONN);
      CURL_TO_SOCKET_ERR_CODE(CURLE_COULDNT_RESOLVE_HOST,   SOCKET_ERR_UNKNOWN_HOST);
   default:
      socketError = SOCKET_ERR_UNCOTEGORIZED;
   }
#undef CURL_ERR_CODE_TO_S3E_SOCKET_ERR_CODE

   _errorCode   = socketError;
   _errorString = curl_easy_strerror(errCode);
}

size_t Socket::send(const char* buf, size_t len)
{
   if (!_connected)
      return 0;

   size_t sentBytes = 0;

   CURLcode res = curl_easy_send(_curl, buf, len, &sentBytes);

   if (res != CURLE_OK) {
      setError(res);
      return -1;
   }

   return sentBytes;
}

size_t Socket::receive(char* buf, size_t len)
{
   if (!_connected)
      return 0;

   size_t receivedBytes;

   CURLcode res = curl_easy_recv(_curl, buf, len, &receivedBytes);

   if (res != CURLE_OK) {
      setError(res);
      return (_errorCode == Socket::SOCKET_ERR_WOULDBLOCK)?0:-1;
   }

   return receivedBytes;
}
