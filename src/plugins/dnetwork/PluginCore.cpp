/**
\author		Korotkov Andrey aka DRON
\date		25.03.2011 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#include "PluginCore.h"

#pragma message("Linking with \"ws2_32.lib\".")
#pragma comment(linker, "/defaultlib:ws2_32.lib")

CPluginCore::CPluginCore(IEngineCore *pEngineCore):
_pEngineCore(pEngineCore), _iDrawProfiler(0),
_uiPacketsSend(0), _uiBytesSend(0),
_uiPacketsRcvd(0), _uiBytesRcvd(0),
_pServer(NULL), _pClient(NULL)
{
	_pEngineCore->GetInstanceIdx(_uiInstIdx);

	_pEngineCore->AddProcedure(EPT_PROCESS,&_s_Process, (void*)this);
	_pEngineCore->AddProcedure(EPT_INIT,&_s_Init, (void*)this);
	_pEngineCore->AddProcedure(EPT_FREE,&_s_Free, (void*)this);
	_pEngineCore->AddEventListner(ET_ON_WIN_MESSAGE,&_s_EventHandler, (void*)this);
	_pEngineCore->AddEventListner(ET_ON_PROFILER_DRAW,&_s_EventHandler, (void*)this);

	_pEngineCore->ConsoleRegComValue("net_profiler", "Displays Network plugin profiler.", &_iDrawProfiler, 0, 3);
}

CPluginCore::~CPluginCore()
{
	_pEngineCore->RemoveProcedure(EPT_PROCESS,&_s_Process, (void*)this);
	_pEngineCore->RemoveProcedure(EPT_INIT,&_s_Init, (void*)this);
	_pEngineCore->RemoveProcedure(EPT_FREE,&_s_Free, (void*)this);
	_pEngineCore->AddEventListner(ET_ON_WIN_MESSAGE,&_s_EventHandler, (void*)this);
	_pEngineCore->AddEventListner(ET_ON_PROFILER_DRAW,&_s_EventHandler, (void*)this);

	_pEngineCore->ConsoleUnregCom("net_profiler");
}

void CPluginCore::_Process()
{
	if(_pServer)
		_pServer->Update();

	if(_pClient)
		_pClient->Update();

	int i = 0;
	while(i < (int)_stRelPackets.size())
	{
		DWORD tick = GetTickCount();

		switch(_stRelPackets[i].eStatus)
		{
		case RPS_IN_PROGRESS:
			if(tick - _stRelPackets[i].uiPrcsdTick > MAX_RELIABLE_PACKET_AWAIT_TIME)
			{
				_stRelPackets[i].eStatus = RPS_TIMEDOUT;
				_stRelPackets[i].uiPrcsdTick = tick;
			}
			else
				if(tick - _stRelPackets[i].uiLastActionTick > MAX_RELIABLE_RESPOND_INTERVAL)
				{
					_stRelPackets[i].uiLastActionTick = tick;
					SendPacket(_stRelPackets[i].stFromSocket, _stRelPackets[i].stToSocket, *_stRelPackets[i].pNetMsg);
				}
			break;
		case RPS_ACKNOWLEDGED:
		case RPS_DONE:
			if(tick - _stRelPackets[i].uiLastActionTick > MAX_RELIABLE_RESPOND_INTERVAL)
			{
				TNetMsg answer_msg;
				answer_msg.stHeader.eMsgType = PMT_RELIABLE_DONE;
				answer_msg.pData = (ubyte*)&_stRelPackets[i].uiUID;					
				answer_msg.SetMsgSize(sizeof(uint64));
				SendPacket(_stRelPackets[i].stFromSocket, _stRelPackets[i].stToSocket, answer_msg);
			}
			break;
		case RPS_TIMEDOUT:
			if(tick - _stRelPackets[i].uiPrcsdTick < MAX_CONNECTION_TIMEOUT)
				break;
		case RPS_CAN_DELETE:
			if(_stRelPackets[i].pNetMsg)
			{
				if(_stRelPackets[i].pNetMsg->pData)
					delete[] _stRelPackets[i].pNetMsg->pData;

				delete _stRelPackets[i].pNetMsg;
			}
			_stRelPackets.erase(_stRelPackets.begin() + i);
			i--;
			break;
		}

		i++;
	}
}

void CPluginCore::_Init()
{
	LOG("Initializing Network ...", LT_INFO);

	char buf[64];
	hostent *p_host;

	if(0 == WSAStartup(MAKEWORD(2,0), &_stWinSockData) && 0 == gethostname(buf, 64) && (p_host = gethostbyname(buf), p_host != NULL))
	{
		_stHostName = string(buf);

		struct in_addr addr;
		memcpy(&addr, p_host->h_addr_list[0], sizeof(in_addr));
		_stHostIP = string(inet_ntoa(addr));

		LOG("Hostname: \"" + _stHostName + "\" IP: " + _stHostIP, LT_INFO);

		_pEngineCore->AllowPause(false);

		LOG("Network initialized.", LT_INFO);
	}
	else
		LOG("Network initialization failed!", LT_FATAL);
}

void CPluginCore::_Free()
{
	if(_pClient)
		CloseLocalClient();

	if(_pServer)
		ShutdownServer();

	for(size_t i = 0; i < _stRelPackets.size(); i++)
		if(_stRelPackets[i].pNetMsg)
		{
			if(_stRelPackets[i].pNetMsg->pData)
				delete[] _stRelPackets[i].pNetMsg->pData;

			delete _stRelPackets[i].pNetMsg;
		}

	_stRelPackets.clear();

	while(_stLoopBackMessages.size() != 0)
	{
		if(_stLoopBackMessages.front().msg.pData)
			delete[] _stLoopBackMessages.front().msg.pData;
		_stLoopBackMessages.pop();
	}

	if(0 != WSACleanup())
		LOG("Winsock finalization failed!", LT_ERROR);

	LOG("Network finalized.", LT_INFO);
}

void CPluginCore::_DrawProfiler()
{
	if(_iDrawProfiler == 0)
		return;

	_pEngineCore->RenderProfilerTxt("=====Network  Profiler=====");
	_pEngineCore->RenderProfilerTxt(("IP: " + _stHostIP).c_str());
	_pEngineCore->RenderProfilerTxt(("Send: " + IntToStr(_uiPacketsSend) + " packets / " + IntToStr(_uiBytesSend) + " bytes").c_str());
	_pEngineCore->RenderProfilerTxt(("Received: " + IntToStr(_uiPacketsRcvd) + " packets / " + IntToStr(_uiBytesRcvd) + " bytes").c_str());
	_pEngineCore->RenderProfilerTxt(("Reliable packets stack size: " + IntToStr(_stRelPackets.size())).c_str());

	if(_iDrawProfiler > 1)
	{
		if(_pServer)
		{
			_pEngineCore->RenderProfilerTxt("--------Server Info--------");
			_pEngineCore->RenderProfilerTxt(_pServer->Address().c_str());
			_pEngineCore->RenderProfilerTxt((string("Status: ") + (_pServer->Active() ? "Active" : "Inactive")).c_str());
			_pEngineCore->RenderProfilerTxt(("Clients: " + IntToStr(_pServer->ClientsCount()) + " / " + IntToStr(_pServer->MaxClients())).c_str());
			if(_iDrawProfiler > 2)
			{
				_pEngineCore->RenderProfilerTxt("--------- Clients ---------");
				for(uint i = 0; i < _pServer->ClientsCount(); i++)
				{
					uint uid, ping;
					_pServer->GetClientUID(i, uid);
					_pServer->GetClientPing(uid, ping);
					_pEngineCore->RenderProfilerTxt(("Client: " + IntToStr(i) + " UID: " + IntToStr(uid) + " Ping: " + IntToStr(ping)).c_str());
				}
			}
		}

		if(_pClient)
		{
			_pEngineCore->RenderProfilerTxt("--------Client Info--------");
			_pEngineCore->RenderProfilerTxt(_pClient->Address().c_str());
			
			bool cntd = _pClient->UID() != -1 && _pClient->UID();

			_pEngineCore->RenderProfilerTxt((string("Status: ") + (cntd ? "Connected" : "Disconnected")).c_str());
			if(cntd)
			{
				_pEngineCore->RenderProfilerTxt(("Server: " + _pClient->ServerAddress()).c_str());
				_pEngineCore->RenderProfilerTxt(("UID: " + IntToStr(_pClient->UID())).c_str());
				uint ping;
				_pClient->GetPing(ping);
				_pEngineCore->RenderProfilerTxt(("Ping: " + IntToStr(ping)).c_str());

			}
		}
	}

	_pEngineCore->RenderProfilerTxt("===========================");
}

HRESULT CPluginCore::CloseSocket(const TSocket &stSocket)
{
	bool done = closesocket(stSocket.socket) != SOCKET_ERROR;

	if(!done)
		LOG("Error closing socket.", LT_ERROR);

	return done ? S_OK : E_FAIL;
}

HRESULT CPluginCore::ReceivePacket(const TSocket &stDestSocket, TSocket &stSourceSocket, TNetMsg &stPacket)
{
	if(!_stLoopBackMessages.empty())//check loopback packets
	{
		TLoopBackMsg &msg = _stLoopBackMessages.front();

		if(stDestSocket.uiPort == msg.dest.uiPort)
		{
			if(msg.msg.stHeader.uiPacketDataSize > stPacket.uiDataSize)
			{
				if(msg.msg.pData)
					delete[] msg.msg.pData;
				_stLoopBackMessages.pop();
 				return E_INVALIDARG;
			}

			stSourceSocket = msg.src;
			stPacket.stHeader = msg.msg.stHeader;

			stPacket.SetMsgSize(msg.msg.uiDataSize);
			memcpy(stPacket.pData, msg.msg.pData, msg.msg.uiDataSize);

			if(msg.msg.pData)
				delete[] msg.msg.pData;

			_stLoopBackMessages.pop();

			_uiPacketsRcvd++;
			_uiBytesRcvd +=  msg.msg.uiDataSize + sizeof(TNetMsgHeader);

			return S_OK;
		}
	}

	sockaddr from;
	int fromlen = sizeof(sockaddr);
	
	static char buff[MAX_PACKET_SIZE];

	int result = recvfrom(stDestSocket.socket, buff, MAX_PACKET_SIZE, 0, &from, &fromlen);

	if(result != SOCKET_ERROR)
	{
		if(result < sizeof(TNetMsgHeader))//not our packet or corrupted data or connection closed
			return E_ABORT;

		E_SOCKET_PORT_TYPE port;
		uint16 ui_port = ntohs(((sockaddr_in *)&from)->sin_port);

		switch(ui_port)
		{
		case SERVER_UDP_PORT: port = SPT_SERVER; break;
		case CLIENT_UDP_PORT: port = SPT_CLIENT; break;
		default: port = SPT_ANY; break;
		}
		
		stSourceSocket = TSocket(inet_ntoa(((sockaddr_in *)&from)->sin_addr), port, INVALID_SOCKET);
		stSourceSocket.uiPort = ui_port;

		memcpy(&stPacket.stHeader, buff, sizeof(TNetMsgHeader));

		if(strcmp(stPacket.stHeader.cVersion, DNET_PROTOCOL_VERSION) != 0)//Wrong protocol version
			return E_ABORT;

		if(stPacket.stHeader.uiPacketDataSize > 0)
		{
			if(stPacket.stHeader.uiPacketDataSize > stPacket.uiDataSize)
				return E_INVALIDARG;

			if(result - sizeof(TNetMsgHeader) < stPacket.stHeader.uiPacketDataSize)//packet seems to be corrupted (must never happen)
				return E_ABORT;

			memcpy(stPacket.pData, (char*)(&buff[0] + sizeof(TNetMsgHeader)), stPacket.stHeader.uiPacketDataSize);

			stPacket.uiDataSize = stPacket.stHeader.uiPacketDataSize;
		}

		_uiPacketsRcvd++;
		_uiBytesRcvd += result;

		if(stPacket.stHeader.ePacketType == SPT_RELIABLE)
		{
			TNetMsg answer_msg;
			answer_msg.stHeader.eMsgType = PMT_RELIABLE_CONFIRM;
			answer_msg.pData = (ubyte*)&stPacket.stHeader.uiUID;					
			answer_msg.SetMsgSize(sizeof(uint64));

			for(size_t i = 0; i < _stRelPackets.size(); i++)
				if(stPacket.stHeader.uiUID == _stRelPackets[i].uiUID && _stRelPackets[i].eStatus == RPS_ACKNOWLEDGED)
					return SUCCEEDED(SendPacket(stDestSocket, stSourceSocket, answer_msg)) ? E_ABORT : E_FAIL;
				
			TReliablePacketInfo rpi;
			rpi.eStatus = RPS_ACKNOWLEDGED;
			rpi.uiUID = stPacket.stHeader.uiUID;
			rpi.uiPrcsdTick = GetTickCount();
			rpi.uiLastActionTick = rpi.uiPrcsdTick;
			rpi.pNetMsg = NULL;
			rpi.stFromSocket = stDestSocket;
			rpi.stToSocket = stSourceSocket;
			_stRelPackets.push_back(rpi);
			
			if(FAILED(SendPacket(stDestSocket, stSourceSocket, answer_msg)))
				return S_FALSE;
			else
				return S_OK;
		}

		if(stPacket.stHeader.eMsgType == PMT_RELIABLE_CONFIRM || stPacket.stHeader.eMsgType == PMT_RELIABLE_DONE)
		{
			uint64 uid;
			memcpy(&uid, stPacket.pData, sizeof(uint64));
			
			TNetMsg answer_msg;
			answer_msg.stHeader.eMsgType = PMT_RELIABLE_DONE;
			answer_msg.pData = (ubyte*)&uid;					
			answer_msg.SetMsgSize(sizeof(uint64));

			for(size_t i = 0; i < _stRelPackets.size(); i++)
				if(uid == _stRelPackets[i].uiUID)
				{
					_stRelPackets[i].uiPrcsdTick = GetTickCount();
					_stRelPackets[i].uiLastActionTick = _stRelPackets[i].uiPrcsdTick;

					if(stPacket.stHeader.eMsgType == PMT_RELIABLE_CONFIRM)
						_stRelPackets[i].eStatus = RPS_ACKNOWLEDGED;
					else
						if(stPacket.stHeader.eMsgType == PMT_RELIABLE_DONE)
						{
							if(_stRelPackets[i].eStatus == RPS_ACKNOWLEDGED)
								_stRelPackets[i].eStatus = RPS_DONE;
							else
								if(_stRelPackets[i].eStatus == RPS_DONE)
									_stRelPackets[i].eStatus = RPS_CAN_DELETE;
						}

					return SUCCEEDED(SendPacket(stDestSocket, stSourceSocket, answer_msg)) ? E_ABORT : E_FAIL;
				}
		
			return E_FAIL;
		}
		else
			return S_OK;
	}
	else
	{
		int error = WSAGetLastError();

		if(error == WSAEWOULDBLOCK || error == WSAEMSGSIZE || result == MAX_PACKET_SIZE)
			return E_ABORT;

		return E_FAIL;
	}
}

HRESULT CPluginCore::SendPacket(const TSocket &stFromSocket, const TSocket &stToSocket, const TNetMsg &stPacket)
{
	if(
		strlen(stToSocket.cIP) == 0 || stPacket.stHeader.uiPacketDataSize != stPacket.uiDataSize ||
		stPacket.uiDataSize > MAX_PACKET_SIZE - sizeof(TNetMsgHeader) || 
		((stPacket.stHeader.eMsgType == PMT_RELIABLE_CONFIRM || stPacket.stHeader.eMsgType == PMT_RELIABLE_DONE) && stPacket.stHeader.ePacketType != SPT_COMMON)||
		(stPacket.stHeader.ePacketType == SPT_LOOPBACK && strcmp(stFromSocket.cIP, stToSocket.cIP) != 0)
		)
		return E_INVALIDARG;

	if( (strcmp(stFromSocket.cIP, stToSocket.cIP) == 0)&&
		(
		 (_pClient && stToSocket.uiPort == _pClient->Socket().uiPort) || 
		 (_pServer && stToSocket.uiPort == SERVER_UDP_PORT)
		)
	   )//loopback packet
	{
		TLoopBackMsg msg;
		msg.src = stFromSocket;
		msg.dest = stToSocket;
		msg.msg.stHeader = stPacket.stHeader;
		msg.msg.stHeader.ePacketType = SPT_LOOPBACK;

		if(stPacket.stHeader.uiPacketDataSize > 0)
		{
			msg.msg.SetMsgSize(stPacket.stHeader.uiPacketDataSize);
			msg.msg.pData = new ubyte[msg.msg.uiDataSize];
			memcpy(msg.msg.pData, stPacket.pData, msg.msg.uiDataSize);
		}

		_stLoopBackMessages.push(msg);

		_uiPacketsSend++;
		_uiBytesSend +=  msg.msg.uiDataSize + sizeof(TNetMsgHeader);

		return S_OK;
	}

	sockaddr_in addr;

	addr.sin_family = AF_INET;

	switch(stToSocket.ePort)
	{
	case SPT_SERVER: addr.sin_port = htons(SERVER_UDP_PORT); break;
	case SPT_CLIENT: addr.sin_port = htons(CLIENT_UDP_PORT); break;
	case SPT_ANY: addr.sin_port = htons(stToSocket.uiPort); break;
	}

	if(stPacket.stHeader.ePacketType == SPT_BROADCAST)
		addr.sin_addr.s_addr = INADDR_BROADCAST;
	else
		addr.sin_addr.s_addr = inet_addr(stToSocket.cIP);

	static char buff[MAX_PACKET_SIZE];

	if(stPacket.stHeader.uiUID == -1)
	{	
		static uint64 counter = GetTickCount();
		time_t tm = time(NULL);
		const_cast<TNetMsg*>(&stPacket)->stHeader.uiUID = (tm  + ++counter)/(rand()%(stFromSocket.uiPort + stToSocket.uiPort));
		const_cast<TNetMsg*>(&stPacket)->stHeader.uiTimeStamp = tm;	
	}

	if(stPacket.stHeader.ePacketType == SPT_RELIABLE)
	{
		TReliablePacketInfo rpi;
		rpi.eStatus = RPS_IN_PROGRESS;
		rpi.uiUID = stPacket.stHeader.uiUID;
		rpi.uiPrcsdTick = GetTickCount();
		rpi.uiLastActionTick = rpi.uiPrcsdTick;

		rpi.pNetMsg = new TNetMsg();
		
		rpi.pNetMsg->stHeader = stPacket.stHeader;
		rpi.pNetMsg->SetMsgSize(stPacket.uiDataSize);
		
		if(stPacket.uiDataSize > 0)
		{
			rpi.pNetMsg->pData = new ubyte[stPacket.uiDataSize];
			memcpy(rpi.pNetMsg->pData, stPacket.pData, stPacket.uiDataSize);
		}

		rpi.stFromSocket = stFromSocket;
		rpi.stToSocket = stToSocket;

		_stRelPackets.push_back(rpi);
	}

	memcpy(buff, &stPacket.stHeader, sizeof(TNetMsgHeader));
	memcpy((char *)(&buff[0] + sizeof(TNetMsgHeader)), stPacket.pData, stPacket.uiDataSize);

	int result = sendto(stFromSocket.socket, buff, sizeof(TNetMsgHeader) + stPacket.stHeader.uiPacketDataSize, 0, (sockaddr*)&addr, sizeof(addr));

	if(result == SOCKET_ERROR)
	{
		int error = WSAGetLastError();

		if(error == WSAEADDRNOTAVAIL)
			return S_FALSE;

		return E_FAIL;
	}
	else
	{
		_uiPacketsSend++;
		_uiBytesSend += result;
	}

	return S_OK;
}

HRESULT CPluginCore::CreateSocket(TSocket &stSocket)
{
	if(stSocket.socket != INVALID_SOCKET)
		return E_INVALIDARG;

	stSocket.socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	if(stSocket.socket == INVALID_SOCKET)
	{
		LOG("Error creating UDP socket.", LT_ERROR);
		return E_FAIL;
	}

	u_long flag = true;
	if(ioctlsocket(stSocket.socket, FIONBIO, &flag) == SOCKET_ERROR)
	{
		LOG("Error setting nonblocking socket.", LT_ERROR);
		closesocket(stSocket.socket);
		stSocket.socket = INVALID_SOCKET;
		return E_FAIL;
	}

	int i = 1;
	if(setsockopt(stSocket.socket, SOL_SOCKET, SO_BROADCAST, (char *)&i, sizeof(i)) == SOCKET_ERROR)
	{
		LOG("Error setting broadcast socket.", LT_ERROR);
		closesocket(stSocket.socket);
		stSocket.socket = INVALID_SOCKET;
		return E_FAIL;
	}

	sockaddr_in addr;
	ZeroMemory(&addr, sizeof(sockaddr_in));

	addr.sin_family = AF_INET;
	
	switch(stSocket.ePort)
	{
	case SPT_SERVER: addr.sin_port = htons(SERVER_UDP_PORT); break;
	case SPT_CLIENT: addr.sin_port = htons(CLIENT_UDP_PORT); break;
	case SPT_ANY: addr.sin_port = 0; break;
	}
	
	addr.sin_addr.s_addr = inet_addr(_stHostIP.c_str());

	if(bind(stSocket.socket, (struct sockaddr *)&addr, sizeof( sockaddr_in)) == SOCKET_ERROR)
	{
		LOG("Error binding socket.", LT_ERROR);
		closesocket(stSocket.socket);
		stSocket.socket = INVALID_SOCKET;
		return E_ABORT;
	}

	sockaddr name;
	int namelen = sizeof(sockaddr);

	if(SOCKET_ERROR == getsockname(stSocket.socket, &name, &namelen))
	{
		LOG("Failed to get socket name.", LT_ERROR);
		strcpy(stSocket.cIP, _stHostIP.c_str());
		return S_FALSE;
	}

	strcpy(stSocket.cIP, inet_ntoa(((sockaddr_in *)&name)->sin_addr));
	stSocket.uiPort = ntohs(((sockaddr_in *)&name)->sin_port);

	return S_OK;
}

HRESULT CPluginCore::GetReliableMsgStatus(uint64 uiUID, E_RELIABLE_PACKET_STATUS &eMsgStatus)
{
	for(size_t i = 0; i < _stRelPackets.size(); i++)
		if(uiUID == _stRelPackets[i].uiUID)
		{
			eMsgStatus = _stRelPackets[i].eStatus;
			return S_OK;
		}

	return E_INVALIDARG;
}

HRESULT CPluginCore::ForceUpdateNetwork(bool bOnlyServer)
{
	if(bOnlyServer)
	{
		if(_pServer)
			_pServer->Update();
	}
	else
		_Process();

	return S_OK;
}

HRESULT CALLBACK CPluginCore::GetMyIP(char *pcIP, uint uiCharsCount)
{
	if(uiCharsCount < _stHostIP.length())
		return E_INVALIDARG;

	strcpy(pcIP, _stHostIP.c_str());

	return S_OK;
}

HRESULT CALLBACK CPluginCore::GetMaxPacketSize(uint &uiSize)
{
	uiSize = MAX_PACKET_SIZE - sizeof(TNetMsgHeader) - sizeof(uint);
	return S_OK;
}

HRESULT CALLBACK CPluginCore::StartLocalServer(IServer *&pServer)
{
	pServer = NULL;

	if(_pServer)
		return E_INVALIDARG;

	try
	{
		_pServer = new CServer(_pEngineCore, (INet*)this);
	}
	catch(string msg)
	{
		LOG("Failed to create local server. " + msg, LT_ERROR);
		return E_ABORT;
	}

	pServer = (IServer*)_pServer;

	return S_OK;
}

HRESULT CALLBACK CPluginCore::ShutdownServer()
{
	if(!_pServer)
		return S_FALSE;

	delete _pServer;
	_pServer = NULL;

	return S_OK;
}

HRESULT CALLBACK CPluginCore::OpenLocalClient(IClient *&pClient)
{
	pClient = NULL;

	if(_pClient)
	{
		pClient = (IClient*)_pClient;
		return S_FALSE;
	}

	try
	{
		_pClient = new CClient(_pEngineCore, (INet*)this);
	}
	catch(string msg)
	{
		LOG("Failed to create local client. " + msg, LT_ERROR);
		return E_ABORT;
	}

	pClient = (IClient*)_pClient;

	return S_OK;
}

HRESULT CALLBACK CPluginCore::CloseLocalClient()
{
	if(!_pClient)
		return S_FALSE;

	delete _pClient;
	_pClient = NULL;

	return S_OK;
}

HRESULT CALLBACK CPluginCore::GetPluginInfo(TPluginInfo &stInfo)
{
	strcpy(stInfo.cName,PLUGIN_NAME);
	strcpy(stInfo.cVersion,PLUGIN_VERSION);
	strcpy(stInfo.cVendor,PLUGIN_VENDOR);
	strcpy(stInfo.cDiscription,PLUGIN_DISCRIPTION);
	stInfo.btPluginSDKVersion = _DGLE2_PLUGIN_SDK_VER_;
	return S_OK;
}

HRESULT CALLBACK CPluginCore::GetPluginInterfaceName(char* pcName, uint uiCharsCount)
{
	if(uiCharsCount<strlen(PLUGIN_INTERFACE_NAME))
		return E_INVALIDARG;

	strcpy(pcName, PLUGIN_INTERFACE_NAME);
	
	return S_OK;
}

void CALLBACK CPluginCore::_s_Process(void *pParametr)
{
	((CPluginCore*)pParametr)->_Process();
}

void CALLBACK CPluginCore::_s_Init(void *pParametr)
{
	((CPluginCore*)pParametr)->_Init();
}

void CALLBACK CPluginCore::_s_Free(void *pParametr)
{
	((CPluginCore*)pParametr)->_Free();
}

void CALLBACK CPluginCore::_s_EventHandler(void *pParametr, IBaseEvent *pEvent)
{
	E_EVENT_TYPE ev_type;

	pEvent->GetEventType(ev_type);

	switch(ev_type)
	{
	case ET_ON_PROFILER_DRAW:
		((CPluginCore*)pParametr)->_DrawProfiler();
		break;
	}
}