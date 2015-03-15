/**
\author		Korotkov Andrey aka DRON
\date		24.03.2011 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#include "Client.h"

class CClientEvent : public IClientEvent
{
	E_EVENT_TYPE_DNET _eEvType;

public:

	CClientEvent(E_EVENT_TYPE_DNET eEvType):
	_eEvType(eEvType){}

	HRESULT CALLBACK GetEventType(E_EVENT_TYPE &eEvType)
	{
	  eEvType = ET_UNKNOWN;
	  return S_OK;
	}

	HRESULT CALLBACK GetUnknownEventType(uint &uiUnknEvType)
	{
	  uiUnknEvType = (uint)_eEvType;
	  return S_OK;
	}

	IDGLE2_BASE_IMPLEMENTATION(IClientEvent)
};

CClient::CClient(IEngineCore *pEngineCore, INet *pNet):
_pEngineCore(pEngineCore), _pNet(pNet), _uiUID(-1)
{
	_stSocket.ePort = SPT_ANY;
	if(FAILED(_pNet->CreateSocket(_stSocket))) throw(string("Failed to create socket."));
}

CClient::~CClient()
{
	if(_IsConnected())
	{
		TNetMsg msg;
		msg.stHeader.eMsgType = PMT_DISCONNECTED;
		_pNet->SendPacket(_stSocket, _stServerSocket, msg);
		Sleep(20);
		_pNet->ForceUpdateNetwork();
	}

	_pNet->CloseSocket(_stSocket);

	while(_stUserMessages.size() != 0)
	{
		if(_stUserMessages.front().pData)
			delete[] _stUserMessages.front().pData;
		_stUserMessages.pop();
	}
}

bool CClient::_IsConnected()
{
	return _uiUID != -1 && _uiUID != 0;
}

const TSocket& CClient::Socket()
{
	return _stSocket;
}

string CClient::Address()
{
	return string(string(_stSocket.cIP) + ":" + IntToStr(_stSocket.uiPort));
}

string CClient::ServerAddress()
{
	if(_IsConnected())
		return string(string(_stServerSocket.cIP) + ":" + IntToStr(_stServerSocket.uiPort));
	else
		return string("None");
}

void CClient::Update()
{
	TSocket src;
	TNetMsg msg;
	
	static ubyte buff_in[MAX_PACKET_SIZE - sizeof(TNetMsgHeader)];
	msg.pData = buff_in;
	msg.SetMsgSize(MAX_PACKET_SIZE - sizeof(TNetMsgHeader));

	uint client_uid = -1;

	uint32 tick = GetTickCount();

	while(SUCCEEDED(_pNet->ReceivePacket(_stSocket, src, msg)))
	if(strcmp(src.cIP, _stServerSocket.cIP) == 0)
	{
		TNetMsg answer_msg;
		static ubyte buff_out[MAX_PACKET_SIZE - sizeof(TNetMsgHeader)];
		answer_msg.pData = buff_out;
		bool do_answer = false;

		switch(msg.stHeader.eMsgType)
		{
		case PMT_USER_DATA:
			memcpy(&client_uid, msg.pData, sizeof(uint));
			if(client_uid == _uiUID)
			{
				_uiLastRcvdMsgTick = tick;
				
				if(msg.stHeader.ePacketType == SPT_COMMON)
				{
					if(msg.stHeader.uiTimeStamp < _uiRelevantTimeStamp)//reject too old packets
						break;

					_uiRelevantTimeStamp = msg.stHeader.uiTimeStamp;
				}

				TNetMsg q_msg;
				q_msg.stHeader = msg.stHeader;
				q_msg.SetMsgSize(msg.uiDataSize - sizeof(uint));
				if(q_msg.uiDataSize > 0)
				{
					q_msg.pData = new ubyte[q_msg.uiDataSize];
					memcpy(q_msg.pData, msg.pData + sizeof(uint), q_msg.uiDataSize);
				}

				_stUserMessages.push(q_msg);

				_pEngineCore->CastEvent(ET_UNKNOWN, (IBaseEvent*)&CClientEvent(ETD_CL_PACKET_RECEIVED));
			}
			break;

		case PMT_ALIVE:
			memcpy(&client_uid, msg.pData, sizeof(uint));
			if(client_uid == _uiUID)
			{
				_uiLastRcvdMsgTick = tick;
				_uiPing = tick - _uiLastSendPingMsgTick;
			}
			break;

		case PMT_PING:
			memcpy(&client_uid, msg.pData, sizeof(uint));

			if(client_uid == _uiUID)
				_uiLastRcvdMsgTick = tick;

			answer_msg.stHeader.eMsgType = PMT_ALIVE;
			memcpy(answer_msg.pData, &_uiUID, sizeof(uint));
			answer_msg.SetMsgSize(sizeof(uint));
			do_answer = true;
			break;

		case PMT_CONNECTION_ACCEPTED:
			_uiLastRcvdMsgTick = tick;
			_uiLastSendPingMsgTick = 0;
			_uiRelevantTimeStamp = msg.stHeader.uiTimeStamp;
			memcpy(&_uiUID, buff_in, sizeof(uint));
			_pEngineCore->CastEvent(ET_UNKNOWN, (IBaseEvent*)&CClientEvent(ETD_CL_CONNECTED_TO_SERVER));
			break;

		case PMT_CONNECTION_REJECTED:
			_uiUID = 0;
			break;

		case PMT_DISCONNECTED:
			_uiUID = -1;
			_pEngineCore->CastEvent(ET_UNKNOWN, (IBaseEvent*)&CClientEvent(ETD_CL_DISCONNECTED_FROM_SERVER));
			break;
		}

		if(do_answer && SUCCEEDED(_pNet->SendPacket(_stSocket, src, answer_msg)))
			_uiLastSendMsgTick = tick;
	}

	if(_IsConnected())
	{
		if(tick - _uiLastRcvdMsgTick > MAX_CONNECTION_TIMEOUT)
		{
			TNetMsg msg;
			msg.stHeader.eMsgType = PMT_DISCONNECTED;
			_pNet->SendPacket(_stSocket, _stServerSocket, msg);
			_pEngineCore->CastEvent(ET_UNKNOWN, (IBaseEvent*)&CClientEvent(ETD_CL_SERVER_TIMEDOUT));
			_uiUID = -1;
		}
		else
			if((tick - _uiLastRcvdMsgTick > MAX_NONPACKET_PAUSE && tick - _uiLastSendMsgTick > MIN_PING_INTERVAL) ||
				tick - _uiLastSendPingMsgTick > MAX_PING_INTERVAL)
			{
				_uiLastSendMsgTick = tick;
				_uiLastSendPingMsgTick = tick;

				TNetMsg msg;
				msg.stHeader.eMsgType = PMT_PING;
				msg.pData = (ubyte*)&_uiUID;
				msg.SetMsgSize(sizeof(uint));
				_pNet->SendPacket(_stSocket, _stServerSocket, msg);
			}
	}
}

HRESULT CALLBACK CClient::ConnectToServer(char *pcServerIP, bool &bSuccessful)
{
	if(_IsConnected())
		return E_FAIL;

	_stServerSocket = TSocket(pcServerIP, SPT_SERVER, INVALID_SOCKET);

	TNetMsg msg;

	msg.stHeader.eMsgType = PMT_CONNECT_TO_SERVER;

	bSuccessful = false;

	_uiLastSendMsgTick = GetTickCount();

	if(SUCCEEDED(_pNet->SendPacket(_stSocket, _stServerSocket, msg)))
	{
		DWORD tick = GetTickCount();
		while(GetTickCount() - tick < MAX_CONNECTING_AWAIT_TIME)
		{
			Sleep(20);

			_pNet->ForceUpdateNetwork();

			if(_uiUID != -1)
			{
				bSuccessful = true;
				return S_OK;
			}
			else
				if(_uiUID == 0)//Server rejects connection
				{
					_uiUID = -1;
					return S_FALSE;
				}
		}

		return S_FALSE;
	}
	else
		return S_FALSE;
}

HRESULT CALLBACK CClient::Disconnect()
{
	if(!_IsConnected())
		return S_FALSE;

	TNetMsg msg;
	msg.stHeader.eMsgType = PMT_DISCONNECTED;
	msg.stHeader.ePacketType = SPT_RELIABLE;
	_pNet->SendPacket(_stSocket, _stServerSocket, msg);

	_uiUID = -1;

	return S_OK;
}

HRESULT CALLBACK CClient::IsConnected(bool &bConnected)
{
	bConnected = _IsConnected();
	return S_OK;
}

HRESULT CALLBACK CClient::GetPing(uint &uiPing)
{
	if(!_IsConnected())
		return E_FAIL;

	uiPing = (uint)_uiPing;

	return S_OK;
}

HRESULT CALLBACK CClient::SearchServersOnLAN(uint &uiCount)
{
	_clLANServers.clear();
	uiCount = 0;

	TNetMsg msg;
	msg.stHeader.ePacketType = SPT_BROADCAST;
	msg.stHeader.eMsgType = PMT_PING;
	msg.pData = (ubyte*)&_uiUID;
	msg.SetMsgSize(sizeof(uint));

	if(SUCCEEDED(_pNet->SendPacket(_stSocket, TSocket("0.0.0.0", SPT_SERVER, INVALID_SOCKET), msg)))
	{
		_pNet->ForceUpdateNetwork(true);

		DWORD tick = GetTickCount();
		while(GetTickCount() - tick < MAX_CONNECTING_AWAIT_TIME)
		{
			TSocket src;
			if(SUCCEEDED(_pNet->ReceivePacket(_stSocket, src, msg)) && msg.stHeader.eMsgType == PMT_ALIVE)
				_clLANServers.push_back(src);
		}

		uiCount = _clLANServers.size();
	}
	else
		return S_FALSE;

	return S_OK;
}

HRESULT CALLBACK CClient::GetFoundOnLANServerIP(uint uiIdx, char *pcIP, uint uiCharsCount)
{
	if(uiIdx >= _clLANServers.size() || uiCharsCount < strlen(_clLANServers[uiIdx].cIP))
		return E_INVALIDARG;

	strcpy(pcIP, _clLANServers[uiIdx].cIP);
 
	return S_OK;
}

HRESULT CALLBACK CClient::SendImmediatePacket(char *pcServerIP, bool &bSuccessful, ubyte *pSendData, uint uiSendDataSize, ubyte *pReceivedData, uint &uiReceivedDataSize)
{
	bSuccessful = false;

	TSocket prev_server = _stServerSocket;

	if(pcServerIP)
		_stServerSocket = TSocket(pcServerIP, SPT_SERVER, INVALID_SOCKET);

	if(!_stUserMessages.empty() || FAILED(SendPacket(pSendData, uiSendDataSize, bSuccessful, false)))
	{
		_stServerSocket = prev_server;
		return E_ABORT;
	}

	_stServerSocket = prev_server;

	DWORD tick = GetTickCount();
	while(GetTickCount() - tick < MAX_CONNECTING_AWAIT_TIME)
	{
		_pNet->ForceUpdateNetwork();
		if(!_stUserMessages.empty())
		{
			ReceivePacket(pReceivedData, uiReceivedDataSize, bSuccessful);
			return S_OK;
		}
	}

	return S_FALSE;
}

HRESULT CALLBACK CClient::SendPacket(ubyte *pSendData, uint uiSendDataSize, bool &bSuccessful, bool bReliable)
{
	bSuccessful = false;

	if(!_IsConnected())
		return E_FAIL;

	if(uiSendDataSize >  MAX_PACKET_SIZE - sizeof(TNetMsgHeader) - sizeof(uint))
		return E_INVALIDARG;

	static char buff[MAX_PACKET_SIZE];

	TNetMsg msg;
	msg.stHeader.eMsgType = PMT_USER_DATA;
	if(bReliable) msg.stHeader.ePacketType = SPT_RELIABLE;
	memcpy(buff, &_uiUID, sizeof(uint));
	memcpy(buff + sizeof(uint), pSendData, uiSendDataSize);
	msg.pData = (ubyte*)buff;
	msg.SetMsgSize(uiSendDataSize + sizeof(uint));

	bSuccessful = SUCCEEDED(_pNet->SendPacket(_stSocket, _stServerSocket, msg));

	return S_OK;
}

HRESULT CALLBACK CClient::ReceivePacket(ubyte *pReceivedData, uint &uiReceivedDataSize, bool &bSuccessful)
{
	bSuccessful = false;

	if(!_IsConnected())
		return E_FAIL;

	if(_stUserMessages.empty())
		return S_OK;

	if(_stUserMessages.front().uiDataSize > uiReceivedDataSize)
	{
		uiReceivedDataSize = _stUserMessages.front().uiDataSize;
		return E_INVALIDARG;
	}

	TNetMsg &cur = _stUserMessages.front();
	_stUserMessages.pop();

	uiReceivedDataSize = cur.uiDataSize;
	memcpy(pReceivedData, cur.pData, cur.uiDataSize);

	if(cur.pData)
		delete[] cur.pData;

	bSuccessful = true;

	return S_OK;
}