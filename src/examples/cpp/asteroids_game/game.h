#pragma once

#include <DGLE.h>
#include <vector>

using namespace DGLE;

class CGameObject;

class IGameObjectManager
{
public:
	virtual bool IsDebugMode() const = 0;
	virtual void IncreaseGameScore(uint uiByValue) = 0;
	virtual void AddObject(CGameObject *pObj) = 0;
	virtual void RemoveObject(CGameObject *pObj) = 0;
};

class CGame : private IUserCallback, private IGameObjectManager
{	
	IEngineCore *_pEngineCore;
	IInput *_pInput;
	IRender2D *_pRender2D;
	IBitmapFont *_pFnt;

	int _iDebug;
	uint _uiScore;

	std::vector<CGameObject *> _clObjects;
	std::vector<CGameObject *> _clRemovePendingList;

	bool _IsPlayerExists() const;
	bool _AreAsteroidsRemain() const;

	void _StartGame();
	void _ClearGame();

	bool IsDebugMode() const;
	void IncreaseGameScore(uint uiByValue);
	void AddObject(CGameObject *pObj);
	void RemoveObject(CGameObject *pObj);
	
	DGLE_RESULT DGLE_API Initialize();
	DGLE_RESULT DGLE_API Free();
	DGLE_RESULT DGLE_API Update(uint64 ui64DeltaTime);
	DGLE_RESULT DGLE_API Render();
	DGLE_RESULT DGLE_API OnEvent(E_EVENT_TYPE eEventType, IBaseEvent *pEvent);

public:

	CGame(IEngineCore *pEngineCore);
	~CGame();

	IDGLE_BASE_IMPLEMENTATION(IUserCallback, INTERFACE_IMPL_END)
};