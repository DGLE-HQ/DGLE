/**
\author		Korotkov Andrey aka DRON
\date		24.03.2011 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#include "Server.h"

class CServerEvent : public IServerEvent
{
	E_EVENT_TYPE_DNET _eEvType;
	uint _uiUID;

public:

	CServerEvent(E_EVENT_TYPE_DNET eEvType, uint uiUID):
	_eEvType(eEvType), _uiUID(uiUID)
	{}

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

	HRESULT CALLBACK GetClientUID(uint &uiUID)
	{
		uiUID = _uiUID;
		return S_OK;
	}

	IDGLE2_BASE_IMPLEMENTATION(IServerEvent)
};

CServer::CServer(IEngineCore *pEngineCore, INet *pNet):
_pEngineCore(pEngineCore),
_pNet(pNet), _uiUIDCounter(0),
_bAllowConnections(true),
_uiMaxClients(MAX_CLIENTS)
{
	_stSocket.ePort = SPT_SERVER;
	if(FAILED(_pNet->CreateSocket(_stSocket))) throw(string("Failed to create socket."));
}

CServer::~CServer()
{
	if(_stClients.size() > 0)
	{
		for(size_t i = 0; i < _stClients.size(); i++)
		{
			TNetMsg msg;
			msg.stHeader.eMsgType = PMT_DISCONNECTED;
			_pNet->SendPacket(_stSocket, _stClients[i].stSocket, msg);
		}

		Sleep(20);

		_pNet->ForceUpdateNetwork();
	}

	_pNet->CloseSocket(_stSocket);

	while(_stUserMessages.size() != 0)
	{
		if(_stUserMessages.front().msg.pData)
			delete[] _stUserMessages.front().msg.pData;
		_stUserMessages.pop();
	}
}

uint CServer::_GetClientIdx(uint uiUID)
{
	for(size_t i = 0; i < _stClients.size(); i++)
		if(_stClients[i].uiUID == uiUID)
			return i;

	return -1;
}

string CServer::Address()
{
	return string(string(_stSocket.cIP) + ":" + IntToStr(_stSocket.uiPort));
}

void CServer::Update()
{
	DWORD tick = GetTickCount();

	TSocket src;
	TNetMsg msg;

	static ubyte buff_in[MAX_PACKET_SIZE - sizeof(TNetMsgHeader)];
	msg.pData = buff_in;
	msg.SetMsgSize(MAX_PACKET_SIZE - sizeof(TNetMsgHeader));

	while(SUCCEEDED(_pNet->ReceivePacket(_stSocket, src, msg)))
	{
		TNetMsg answer_msg;
		static ubyte buff_out[MAX_PACKET_SIZE - sizeof(TNetMsgHeader)];
		answer_msg.pData = buff_out;
		bool do_answer = false;

		uint client_idx = -1, client_uid = -1;

		switch(msg.stHeader.eMsgType)
		{
		case PMT_USER_DATA:
			memcpy(&client_uid, msg.pData, sizeof(uint));
			client_idx = _GetClientIdx(client_uid);

			if(client_idx != -1)
			{
				_stClients[client_idx].uiLastRcvdMsgTick = tick;

				if(msg.stHeader.ePacketType == SPT_COMMON)
				{
					if(msg.stHeader.uiTimeStamp < _stClients[client_idx].uiRelevantTimeStamp)//reject too old packets
						break;

					_stClients[client_idx].uiRelevantTimeStamp = msg.stHeader.uiTimeStamp;
				}

				TQueueMsg q_msg;
				q_msg.uid = client_uid;
				q_msg.msg.stHeader = msg.stHeader;
				q_msg.msg.SetMsgSize(msg.uiDataSize - sizeof(uint));
				if(q_msg.msg.uiDataSize > 0)
				{
					q_msg.msg.pData = new ubyte[q_msg.msg.uiDataSize];
					memcpy(q_msg.msg.pData, msg.pData + sizeof(uint), q_msg.msg.uiDataSize);
				}

				_stUserMessages.push(q_msg);

				_pEngineCore->CastEvent(ET_UNKNOWN, (IBaseEvent*)&CServerEvent(ETD_SV_PACKET_RECEIVED, client_uid));
			}
			break;

		case PMT_ALIVE:
			memcpy(&client_uid, msg.pData, sizeof(uint));
			client_idx = _GetClientIdx(client_uid);
			if(client_idx != -1)
			{
				_stClients[client_idx].uiLastRcvdMsgTick = tick;
				_stClients[client_idx].uiPing = tick - _stClients[client_idx].uiLastSendPingMsgTick;
			}
			break;

		case PMT_PING:
			memcpy(&client_uid, msg.pData, sizeof(uint));
			client_idx = _GetClientIdx(client_uid);
			
			if(client_idx != -1)
				_stClients[client_idx].uiLastRcvdMsgTick = tick;

			answer_msg.stHeader.eMsgType = PMT_ALIVE;
			memcpy(answer_msg.pData, &client_uid, sizeof(uint));
			answer_msg.SetMsgSize(sizeof(uint));
			do_answer = true;
			break;

		case PMT_CONNECT_TO_SERVER:
			bool flag;
			flag = false;
			for(size_t i = 0; i < _stClients.size(); i++)
				if(strcmp(_stClients[i].stSocket.cIP, src.cIP) == 0 && _stClients[i].stSocket.uiPort == src.uiPort)//Client already connected
				{
					flag = true;
					break;
				}

			if(!flag && _bAllowConnections && _stClients.size() < _uiMaxClients)
			{
				answer_msg.stHeader.eMsgType = PMT_CONNECTION_ACCEPTED;
				answer_msg.stHeader.ePacketType = SPT_RELIABLE;
				_uiUIDCounter++;
				memcpy(answer_msg.pData, &_uiUIDCounter, sizeof(uint));
				answer_msg.SetMsgSize(sizeof(uint));
				
				TClient new_client;
				new_client.uiUID = _uiUIDCounter;//uiUID must not be 0 or -1
				new_client.uiLastRcvdMsgTick = tick;
				new_client.uiLastSendMsgTick = tick;
				new_client.uiLastSendPingMsgTick = 0;
				new_client.uiRelevantTimeStamp = msg.stHeader.uiTimeStamp;
				new_client.uiPing = 0;
				new_client.stSocket = src;

				_stClients.push_back(new_client);
				_pEngineCore->CastEvent(ET_UNKNOWN, (IBaseEvent*)&CServerEvent(ETD_SV_CLIENT_CONNECTED, _uiUIDCounter));
			}
			else
				answer_msg.stHeader.eMsgType = PMT_CONNECTION_REJECTED;

			do_answer = true;
			break;

		case PMT_DISCONNECTED:
			for(size_t i = 0; i < _stClients.size(); i++)
				if(strcmp(_stClients[i].stSocket.cIP, src.cIP) == 0 && _stClients[i].stSocket.uiPort == src.uiPort)
				{
					_pEngineCore->CastEvent(ET_UNKNOWN, (IBaseEvent*)&CServerEvent(ETD_SV_CLIENT_DISCONNECTED, _stClients[i].uiUID));
					_stClients.erase(_stClients.begin() + i);
					break;
				}
			break;
		}

		if(do_answer && SUCCEEDED(_pNet->SendPacket(_stSocket, src, answer_msg)))
			if(client_idx != -1)
				_stClients[client_idx].uiLastSendMsgTick = tick;
	}

	for(size_t i = 0; i < _stClients.size(); i++)
		if(tick - _stClients[i].uiLastRcvdMsgTick > MAX_CONNECTION_TIMEOUT)
		{
			TNetMsg msg;
			msg.stHeader.eMsgType = PMT_DISCONNECTED;
			_pNet->SendPacket(_stSocket, _stClients[i].stSocket, msg);
			_pEngineCore->CastEvent(ET_UNKNOWN, (IBaseEvent*)&CServerEvent(ETD_SV_CLIENT_TIMEDOUT, _stClients[i].uiUID));
			_stClients.erase(_stClients.begin() + i);
			break;
		}
		else
			if((tick - _stClients[i].uiLastRcvdMsgTick > MAX_NONPACKET_PAUSE && tick - _stClients[i].uiLastSendMsgTick > MIN_PING_INTERVAL) ||
				tick - _stClients[i].uiLastSendPingMsgTick > MAX_PING_INTERVAL)
			{
				_stClients[i].uiLastSendMsgTick = tick;
				_stClients[i].uiLastSendPingMsgTick = tick;

				TNetMsg msg;
				msg.stHeader.eMsgType = PMT_PING;
				msg.pData = (ubyte*)&_stClients[i].uiUID;
				msg.SetMsgSize(sizeof(uint));
				_pNet->SendPacket(_stSocket, _stClients[i].stSocket, msg);
			}
}

HRESULT CALLBACK CServer::SetMaxClients(uint uiMaxClientsCnt)
{
	if(uiMaxClientsCnt > MAX_CLIENTS)
		return E_INVALIDARG;

	_uiMaxClients = uiMaxClientsCnt;
	
	return S_OK;
}

HRESULT CALLBACK CServer::AllowConnections(bool bAllow)
{
	_bAllowConnections = bAllow;
	return S_OK;
}

HRESULT CALLBACK CServer::GetConnectedClientsCount(uint &uiCount)
{
	uiCount = _stClients.size();
	return S_OK;
}

HRESULT CALLBACK CServer::GetClientUID(uint uiClientIdx, uint &uiUID)
{
	if(uiClientIdx >= _stClients.size())
		return E_INVALIDARG;

	uiUID = _stClients[uiClientIdx].uiUID;

	return S_OK;
}

HRESULT CALLBACK CServer::DisconnectClient(uint uiClientUID)
{
	uint idx = _GetClientIdx(uiClientUID);

	if(idx == -1)
		return E_INVALIDARG;

	TNetMsg msg;
	msg.stHeader.ePacketType = SPT_RELIABLE;
	msg.stHeader.eMsgType = PMT_DISCONNECTED;
	_pNet->SendPacket(_stSocket, _stClients[idx].stSocket, msg);
	_stClients.erase(_stClients.begin() + idx);
	
	return S_OK;
}

HRESULT CALLBACK CServer::GetClientPing(uint uiClientUID, uint &uiPing)
{
	uint idx = _GetClientIdx(uiClientUID);

	if(idx == -1)
		return E_INVALIDARG;

	uiPing = _stClients[idx].uiPing;

	return S_OK;
}

HRESULT CALLBACK CServer::SendImmediatePacket(uint uiClientUID, bool &bSuccessful, ubyte *pSendData, uint uiSendDataSize, ubyte *pReceivedData, uint &uiReceivedDataSize)
{
	bSuccessful = false;

	if(!_stUserMessages.empty() || FAILED(SendPacket(uiClientUID, pSendData, uiSendDataSize, bSuccessful, false)))
		return E_ABORT;

	DWORD tick = GetTickCount();
	while(GetTickCount() - tick < MAX_CONNECTING_AWAIT_TIME)
	{
		_pNet->ForceUpdateNetwork();
		if(!_stUserMessages.empty())
		{
			ReceivePacket(uiClientUID, pReceivedData, uiReceivedDataSize, bSuccessful);
			return S_OK;
		}
	}

	return S_FALSE;
}

HRESULT CALLBACK CServer::SendPacket(uint uiClientUID, ubyte *pSendData, uint uiSendDataSize, bool &bSuccessful, bool bReliable)
{
	bSuccessful = false;
	uint idx = _GetClientIdx(uiClientUID);

	if(uiSendDataSize >  MAX_PACKET_SIZE - sizeof(TNetMsgHeader) - sizeof(uint) || (uiClientUID != -1 && idx == -1))
		return E_INVALIDARG;

	static char buff[MAX_PACKET_SIZE];

	TNetMsg msg;
	msg.stHeader.eMsgType = PMT_USER_DATA;
	if(bReliable) msg.stHeader.ePacketType = SPT_RELIABLE;
	memcpy(buff + sizeof(uint), pSendData, uiSendDataSize);
	msg.pData = (ubyte*)buff;
	msg.SetMsgSize(uiSendDataSize + sizeof(uint));

	if(idx != -1)
	{	
		memcpy(buff, &uiClientUID, sizeof(uint));
		bSuccessful = SUCCEEDED(_pNet->SendPacket(_stSocket, _stClients[idx].stSocket, msg));
	}
	else
	{
		if(_stClients.size() == 0)
			return S_FALSE;

		bSuccessful = true;
		
		for(size_t i = 0; i < _stClients.size(); i++)
		{
			memcpy(buff, &_stClients[i].uiUID, sizeof(uint));
			_pNet->SendPacket(_stSocket, _stClients[i].stSocket, msg);
		}
	}

	return S_OK;
}

HRESULT CALLBACK CServer::ReceivePacket(uint &uiClientUID, ubyte *pReceivedData, uint &uiReceivedDataSize, bool &bSuccessful)
{
	bSuccessful = false;
	
	if(_stUserMessages.empty())
		return S_OK;

	if(_stUserMessages.front().msg.uiDataSize > uiReceivedDataSize)
	{
		uiReceivedDataSize = _stUserMessages.front().msg.uiDataSize;
		return E_INVALIDARG;
	}

	TQueueMsg &cur = _stUserMessages.front();
	_stUserMessages.pop();

	uiClientUID = cur.uid;
	uiReceivedDataSize = cur.msg.uiDataSize;
	memcpy(pReceivedData, cur.msg.pData, cur.msg.uiDataSize);

	if(cur.msg.pData)
		delete[] cur.msg.pData;

	bSuccessful = true;

	return S_OK;
}