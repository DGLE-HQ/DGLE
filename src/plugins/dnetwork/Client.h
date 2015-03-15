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

#ifndef _CCLIENT_H
#define _CCLIENT_H

#include "stdafx.h"

class CClient : public IClient
{
	IEngineCore
			*_pEngineCore;
	INet	*_pNet;
	
	TSocket	_stSocket, _stServerSocket;
	
	uint32 _uiLastRcvdMsgTick, _uiLastSendMsgTick,
		   _uiLastSendPingMsgTick;
	uint64 _uiRelevantTimeStamp;

	uint _uiUID;

	uint _uiPing;

	queue<TNetMsg> _stUserMessages;
	vector<TSocket> _clLANServers;

	bool _IsConnected();

public:

	CClient(IEngineCore *pEngineCore, INet *pNet);
	~CClient();

	void Update();
	string Address();
	string ServerAddress();
	uint UID(){ return _uiUID; }
	const TSocket& Socket();

	HRESULT CALLBACK ConnectToServer(char *pcServerIP, bool &bSuccessful);
	HRESULT CALLBACK Disconnect();
	HRESULT CALLBACK IsConnected(bool &bConnected);
	HRESULT CALLBACK GetPing(uint &uiPing);
	HRESULT CALLBACK SearchServersOnLAN(uint &uiCount);
	HRESULT CALLBACK GetFoundOnLANServerIP(uint uiIdx, char *pcIP, uint uiCharsCount) ;
	HRESULT CALLBACK SendImmediatePacket(char *pcServerIP, bool &bSuccessful, ubyte *pSendData, uint uiSendDataSize, ubyte *pReceivedData, uint &uiReceivedDataSize);
	HRESULT CALLBACK SendPacket(ubyte *pSendData, uint uiSendDataSize, bool &bSuccessful, bool bReliable);
	HRESULT CALLBACK ReceivePacket(ubyte *pReceivedData, uint &uiReceivedDataSize, bool &bSuccessful);

	IDGLE2_BASE_IMPLEMENTATION(IClient)
};

#endif