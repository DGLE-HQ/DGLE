/**
\author		Korotkov Andrey aka DRON
\date		25.03.2011 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _PCORE_H
#define _PCORE_H

#include "stdafx.h"
#include "Server.h"
#include "Client.h"

struct TLoopBackMsg
{
	TSocket dest, src;
	TNetMsg msg;
};

class CPluginCore : public INetwork, public INet
{
	friend void LogWrite(uint uiInstIdx, const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber);

private:

	uint		_uiInstIdx;
	IEngineCore *_pEngineCore;
	int			_iDrawProfiler;

	WSADATA		_stWinSockData;

	string		_stHostName, _stHostIP;

	vector<TReliablePacketInfo> _stRelPackets;
	queue<TLoopBackMsg> _stLoopBackMessages;

	uint32		_uiPacketsSend, _uiBytesSend,
				_uiPacketsRcvd, _uiBytesRcvd;

	CServer		*_pServer;
	CClient		*_pClient;

	void _Process();
	void _Init();
	void _Free();
	void _DrawProfiler();

	static void CALLBACK _s_EventHandler(void *pParametr, IBaseEvent *pEvent);
	static void CALLBACK _s_Process(void *pParametr);
	static void CALLBACK _s_Init(void *pParametr);
	static void CALLBACK _s_Free(void *pParametr);

public:

	CPluginCore(IEngineCore *pEngineCore);
	~CPluginCore();

	HRESULT CreateSocket(TSocket &stSocket);
	HRESULT CloseSocket(const TSocket &stSocket);
	HRESULT ReceivePacket(const TSocket &stDestSocket, TSocket &stSourceSocket, TNetMsg &stPacket);
	HRESULT SendPacket(const TSocket &stFromSocket, const TSocket &stToSocket, const TNetMsg &stPacket);
	HRESULT GetReliableMsgStatus(uint64 uiUID, E_RELIABLE_PACKET_STATUS &eMsgStatus);
	HRESULT ForceUpdateNetwork(bool bOnlyServer);

	HRESULT CALLBACK GetMyIP(char *pcIP, uint uiCharsCount);
	HRESULT CALLBACK GetMaxPacketSize(uint &uiSize);
	HRESULT CALLBACK StartLocalServer(IServer *&pServer);
	HRESULT CALLBACK ShutdownServer();
	HRESULT CALLBACK OpenLocalClient(IClient *&pClient);
	HRESULT CALLBACK CloseLocalClient();
	HRESULT CALLBACK GetPluginInfo(TPluginInfo &stInfo);
	HRESULT CALLBACK GetPluginInterfaceName(char* pcName, uint uiCharsCount);

	IDGLE2_BASE_IMPLEMENTATION(IPlugin)
};

#endif //_PCORE_H