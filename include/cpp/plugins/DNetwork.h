/**
\file		DNetwork.h
\author		Korotkov Andrey aka DRON
\version	alpha 1
\date		24.03.2011 (c)Korotkov Andrey

\brief		Provides networking support for DGLE2.

This header requires connection of the DNetwork plugin.

*/

#ifndef _DGLE2_DNETWORK
#define _DGLE2_DNETWORK

#include "DGLE2.h"

namespace DGLE2
{
	enum E_EVENT_TYPE_DNET//Extends standart DGLE2::E_EVENT_TYPE
	{
		//Server Events
		ETD_SV_CLIENT_CONNECTED = 4444,
		ETD_SV_CLIENT_DISCONNECTED,
		ETD_SV_CLIENT_TIMEDOUT,
		ETD_SV_PACKET_RECEIVED,

		//Client Events
		ETD_CL_CONNECTED_TO_SERVER,
		ETD_CL_DISCONNECTED_FROM_SERVER,
		ETD_CL_SERVER_TIMEDOUT,
		ETD_CL_PACKET_RECEIVED
	};

	// {F418BAD9-1791-46a3-90FF-368FCD29FC62}
	static const GUID IID_IServerEvent = 
	{ 0xf418bad9, 0x1791, 0x46a3, { 0x90, 0xff, 0x36, 0x8f, 0xcd, 0x29, 0xfc, 0x62 } };

	class IServerEvent : public IBaseEvent
	{
	public:
		virtual HRESULT CALLBACK GetClientUID(uint &uiUID) = 0;
	};

	// {02DE3EB3-15EB-49fa-A103-5AC49C872D08}
	static const GUID IID_IClientEvent = 
	{ 0x2de3eb3, 0x15eb, 0x49fa, { 0xa1, 0x3, 0x5a, 0xc4, 0x9c, 0x87, 0x2d, 0x8 } };

	class IClientEvent : public IBaseEvent
	{
	public:
	};

	class IServer;
	class IClient;

	// {6C351684-8C38-48a3-9A1B-5C4DE1A97163}
	static const GUID IID_INetwork = 
	{ 0x6c351684, 0x8c38, 0x48a3, { 0x9a, 0x1b, 0x5c, 0x4d, 0xe1, 0xa9, 0x71, 0x63 } };

	class INetwork : public IPlugin
	{
	public:
		virtual HRESULT CALLBACK GetMyIP(char *pcIP, uint uiCharsCount) = 0;
		virtual HRESULT CALLBACK GetMaxPacketSize(uint &uiSize) = 0;
		
		virtual HRESULT CALLBACK StartLocalServer(IServer *&pServer) = 0;
		virtual HRESULT CALLBACK ShutdownServer() = 0;

		virtual HRESULT CALLBACK OpenLocalClient(IClient *&pClient) = 0;
		virtual HRESULT CALLBACK CloseLocalClient() = 0;
	};

	// {F63A001F-9239-4b4f-B8EF-38A3E1DEDE85}
	static const GUID IID_IServer = 
	{ 0xf63a001f, 0x9239, 0x4b4f, { 0xb8, 0xef, 0x38, 0xa3, 0xe1, 0xde, 0xde, 0x85 } };

	class IServer : public IDGLE2_Base
	{
	public:
		virtual HRESULT CALLBACK SetMaxClients(uint uiMaxClientsCnt) = 0;
		virtual HRESULT CALLBACK GetConnectedClientsCount(uint &uiCount) = 0;
		virtual HRESULT CALLBACK AllowConnections(bool bAllow) = 0;
		virtual HRESULT CALLBACK GetClientUID(uint uiClientIdx, uint &uiUID) = 0;
		virtual HRESULT CALLBACK DisconnectClient(uint uiClientUID) = 0;	
		virtual HRESULT CALLBACK GetClientPing(uint uiClientUID, uint &uiPing) = 0;
		virtual HRESULT CALLBACK SendImmediatePacket(uint uiClientUID/*-1 meens to all clients*/, bool &bSuccessful, ubyte *pSendData, uint uiSendDataSize, ubyte *pReceivedData, uint &uiReceivedDataSize) = 0;		
		virtual HRESULT CALLBACK SendPacket(uint uiClientUID/*-1 meens to all clients*/, ubyte *pSendData, uint uiSendDataSize, bool &bSuccessful, bool bReliable = false) = 0;
		virtual HRESULT CALLBACK ReceivePacket(uint &uiClientUID, ubyte *pReceivedData, uint &uiReceivedDataSize, bool &bSuccessful) = 0;
	};

	// {1A9C292D-C9CE-4553-9D04-1BD00EC692F9}
	static const GUID IID_IClient = 
	{ 0x1a9c292d, 0xc9ce, 0x4553, { 0x9d, 0x4, 0x1b, 0xd0, 0xe, 0xc6, 0x92, 0xf9 } };

	class IClient : public IDGLE2_Base
	{
	public:
		virtual HRESULT CALLBACK ConnectToServer(char *pcServerIP, bool &bSuccessful) = 0;
		virtual HRESULT CALLBACK Disconnect() = 0;
		virtual HRESULT CALLBACK IsConnected(bool &bConnected) = 0;
		virtual HRESULT CALLBACK GetPing(uint &uiPing) = 0;

		virtual HRESULT CALLBACK SearchServersOnLAN(uint &uiCount) = 0;
		virtual HRESULT CALLBACK GetFoundOnLANServerIP(uint uiIdx, char *pcIP, uint uiCharsCount) = 0;

		virtual HRESULT CALLBACK SendImmediatePacket(char *pcServerIP/*if NULL sends to connected server*/, bool &bSuccessful, ubyte *pSendData, uint uiSendDataSize, ubyte *pReceivedData, uint &uiReceivedDataSize) = 0;		
		virtual HRESULT CALLBACK SendPacket(ubyte *pSendData, uint uiSendDataSize, bool &bSuccessful, bool bReliable = false) = 0;
		virtual HRESULT CALLBACK ReceivePacket(ubyte *pReceivedData, uint &uiReceivedDataSize, bool &bSuccessful) = 0;
	};
}

#endif