/**
\author		Korotkov Andrey aka DRON
\date		24.03.2011 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _CSERVER_H
#define _CSERVER_H

#include "stdafx.h"

struct TClient
{
	uint uiUID;
	uint32	uiLastRcvdMsgTick, uiLastSendMsgTick,
			uiLastSendPingMsgTick;
	uint64 uiRelevantTimeStamp;
	uint uiPing;
	TSocket stSocket;
};

struct TQueueMsg
{
	uint uid;
	TNetMsg msg;
};

class CServer : public IServer
{
	IEngineCore 
			*_pEngineCore;
	INet	*_pNet;

	TSocket _stSocket;

	uint	_uiUIDCounter,
			_uiMaxClients;

	vector<TClient> _stClients;
	queue<TQueueMsg> _stUserMessages;

	bool _bAllowConnections;

	uint _GetClientIdx(uint uiUID);

public:

	CServer(IEngineCore *pEngineCore, INet *pNet);
	~CServer();

	void Update();
	
	string Address();
	uint ClientsCount(){ return _stClients.size(); }
	uint MaxClients(){ return _uiMaxClients; }
	bool Active(){ return _bAllowConnections; }

	HRESULT CALLBACK SetMaxClients(uint uiMaxClientsCnt);
	HRESULT CALLBACK AllowConnections(bool bAllow);
	HRESULT CALLBACK GetConnectedClientsCount(uint &uiCount);
	HRESULT CALLBACK GetClientUID(uint uiClientIdx, uint &uiUID);
	HRESULT CALLBACK DisconnectClient(uint uiClientUID);
	HRESULT CALLBACK GetClientPing(uint uiClientUID, uint &uiPing);
	HRESULT CALLBACK SendImmediatePacket(uint uiClientUID, bool &bSuccessful, ubyte *pSendData, uint uiSendDataSize, ubyte *pReceivedData, uint &uiReceivedDataSize);		
	HRESULT CALLBACK SendPacket(uint uiClientUID, ubyte *pSendData, uint uiSendDataSize, bool &bSuccessful, bool bReliable);
	HRESULT CALLBACK ReceivePacket(uint &uiClientUID, ubyte *pReceivedData, uint &uiReceivedDataSize, bool &bSuccessful);

	IDGLE2_BASE_IMPLEMENTATION(IServer)
};

#endif