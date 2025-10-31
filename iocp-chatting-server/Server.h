#pragma once  
#include "stdafx.h"  

struct ServerConfig;

class Server  
{  
   struct ServerConfig  
   {  
       wchar_t Ip[INET_ADDRSTRLEN];
       int Port;  

       int MaxRecvQsize;  
       int MaxSendQsize;  
       int BackLogQsize;  
       int MaxSession;  

       char PktHeaderCode;  
   };  

public:  
   virtual bool Start() = 0;  
   virtual void Stop() = 0;  

protected:  
   void LoadConfig();  

   ServerConfig config_ = {};
};
