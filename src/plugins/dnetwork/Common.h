/**
\author		Korotkov Andrey aka DRON
\date		25.03.2010 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _STDAFX_H
#define _STDAFX_H

//Sys. defines

#define _SECURE_SCL 0
#define _CRT_SECURE_NO_WARNINGS

//Includes

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x05000400 //Request Windows 2000 SP4
#endif

#define NOCOMM

#include <WinSock2.h>
#include <Windows.h>
#include <time.h>

#include "DGLE2.h"
#include "DGLE2_helpers.h"
#include "DNetwork.h"
using namespace DGLE2;

#include <string>
#include <vector>
#include <queue>
using namespace std;

#include "Utils.h"

//Defines

#define DNET_PROTOCOL_VERSION "DNET_1.0"

#define BASE_PORT 4444
#define SERVER_UDP_PORT	BASE_PORT
#define CLIENT_UDP_PORT	BASE_PORT+2
#define CLIENT_HTTP_PORT 80

#define MAX_PACKET_SIZE 512
#define MAX_CLIENTS 8

#define MAX_CONNECTING_AWAIT_TIME 1500
#define MAX_CONNECTION_TIMEOUT 20000

#define MAX_RELIABLE_PACKET_AWAIT_TIME MAX_CONNECTION_TIMEOUT/2
#define MAX_RELIABLE_RESPOND_INTERVAL 150

#define MAX_NONPACKET_PAUSE MAX_CONNECTION_TIMEOUT/4

#define MIN_PING_INTERVAL 200
#define MAX_PING_INTERVAL 5000

//Typdefs

enum E_PROTOCOL_MESSAGE_TYPE
{
	PMT_PING = 0,
	PMT_ALIVE,
	PMT_RELIABLE_CONFIRM,	//System message. Must not be send manually!
	PMT_RELIABLE_DONE,		//System message. Must not be send manually!
	PMT_CONNECT_TO_SERVER,
	PMT_CONNECTION_ACCEPTED,
	PMT_CONNECTION_REJECTED,
	PMT_DISCONNECTED,
	PMT_USER_DATA
};

enum E_SEND_PACKET_TYPE
{
	SPT_COMMON = 0,
	SPT_RELIABLE,
	SPT_BROADCAST,
	SPT_LOOPBACK
};

#pragma pack( push, 1 )

struct TNetMsgHeader
{
	char cVersion[12];
	uint64 uiUID;
	uint64 uiTimeStamp;
	uint uiPacketDataSize;

	E_PROTOCOL_MESSAGE_TYPE eMsgType;
	E_SEND_PACKET_TYPE ePacketType;

	TNetMsgHeader():uiUID(-1), uiTimeStamp(0), eMsgType(PMT_PING), ePacketType(SPT_COMMON), uiPacketDataSize(0)
	{
		strcpy(cVersion, DNET_PROTOCOL_VERSION);
	}
};

#pragma pack( pop )

struct TNetMsg
{
	TNetMsgHeader stHeader;
	
	ubyte *pData;
	uint uiDataSize;

	TNetMsg():pData(NULL), uiDataSize(0){}
	void SetMsgSize(uint size)
	{
		stHeader.uiPacketDataSize = size;
		uiDataSize = size;
	}
};

enum E_SOCKET_PORT_TYPE
{
	SPT_SERVER = 0,
	SPT_CLIENT,
	SPT_ANY
};

struct TSocket
{
	char			cIP[16];
	E_SOCKET_PORT_TYPE
					ePort;
	SOCKET			socket;
	uint16			uiPort;

	TSocket(const char *_pcIP, E_SOCKET_PORT_TYPE _ePort, SOCKET _socket):
	ePort(_ePort), socket(_socket)
	{
		if(strlen(_pcIP) < 16)
			strcpy(cIP, _pcIP);
		else
			strcpy(cIP, "");

		switch(ePort)
		{
		case SPT_SERVER: uiPort = SERVER_UDP_PORT; break;
		case SPT_CLIENT: uiPort = CLIENT_UDP_PORT; break;
		case SPT_ANY: uiPort = 0; break;
		}
	}

	TSocket():socket(INVALID_SOCKET), uiPort(0)
	{
		strcpy(cIP, "");
	}
};

enum E_RELIABLE_PACKET_STATUS
{
	RPS_IN_PROGRESS = 0,
	RPS_ACKNOWLEDGED,
	RPS_DONE,
	RPS_CAN_DELETE,
	RPS_TIMEDOUT
};

struct TReliablePacketInfo
{
	E_RELIABLE_PACKET_STATUS eStatus;
	uint64 uiUID;
	uint32 uiPrcsdTick, uiLastActionTick;

	TNetMsg *pNetMsg;
	TSocket stFromSocket, stToSocket;
};

class INet
{
public:
	virtual HRESULT CreateSocket(TSocket &stSocket) = 0;
	virtual HRESULT CloseSocket(const TSocket &stSocket) = 0;
	virtual HRESULT ReceivePacket(const TSocket &stDestSocket, TSocket &stSourceSocket, TNetMsg &stPacket) = 0;
	virtual HRESULT SendPacket(const TSocket &stFromSocket, const TSocket &stToSocket, const TNetMsg &stPacket) = 0;
	virtual HRESULT GetReliableMsgStatus(uint64 uiUID, E_RELIABLE_PACKET_STATUS &eMsgStatus) = 0;
	virtual HRESULT ForceUpdateNetwork(bool bOnlyServer = false) = 0;
};

//Plugin defines

#define PLUGIN_NAME "DNetwork"
#define PLUGIN_VERSION (string("alpha 1 (")+string(__DATE__)+string(")")).c_str()
#define PLUGIN_VENDOR "DRON"
#define PLUGIN_DISCRIPTION "Provides networking support via TCP/IP and UDP/IP network protocols."

#define PLUGIN_INTERFACE_NAME "INetwork"

//Externals

extern HMODULE	hModule;
void LogWrite(uint uiInstIdx, const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber);

//Macroses

#define LOG(txt, type) LogWrite(_uiInstIdx, string(txt).c_str(), type, __FILE__, __LINE__)

#endif //_STDAFX_H