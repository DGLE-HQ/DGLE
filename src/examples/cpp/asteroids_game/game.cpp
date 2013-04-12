#include "Game.h"
#include <string>
#include <algorithm>

// game main resource path
#ifdef _DEBUG
#	define RESOURCE_PATH "..\\..\\..\\..\\..\\resources\\"
#else
#	define RESOURCE_PATH "..\\..\\..\\..\\resources\\"
#endif

// logical game resolution
#define GAME_VP_WIDTH	800u
#define GAME_VP_HEIGHT	600u

// names of some game resources
#define MESH_PLANET		"MeshPlanet"
#define TEX_PLANET		"TexPlanet"
#define TEX_CLOUDS		"TexClouds"
#define TEX_STARS		"TexStars"
#define MESH_SHIP		"MeshShip"
#define TEX_SHIP		"TexShip"
#define MESH_ASTEROID	"MeshAsteroid"
#define TEX_ASTEROID	"TexAsteroid"
#define TEX_SPARK		"TexSpark"
#define SND_EXPLO		"SndExplo"
#define SND_FIRE		"SndFire"
#define FNT_MAIN		"FntMain"

// sprite config
#define TEX_EXPLO "TexExplo"
#define EXPLO_ANIMATION_FRAMES_COUNT 16
#define EXPLO_ANIMATION_FRAME_SIZE 128

// some helper macroses
#define TO_RAD (float)M_PI / 180.f
#define CLAMP(val, min, max) (val < min ? min : (val > max ? max : val))

// CGameObject
// Base game object.
class CGameObject
{
public:

	enum E_GAME_OBJECT_TYPE { GOT_UNKNOWN, GOT_PLAYER, GOT_ASTEROID, GOT_SHOT };

protected:

	E_GAME_OBJECT_TYPE _objType;

	IGameObjectManager *_pObjMan;
	IEngineCore *_pEngineCore;
	IRender2D *_pRender2D;
	
	TPoint2 _stPos; // object position on screen
	
	float _fSize, // object drawing size
		_fColScale, // scale of object collision bounds
		_fAngle; // angle of the object (it's direction)

	uint _uiCounter; // increases every update by one
	int _iRenderLayer; // controls order of drawing

public:

	CGameObject(IGameObjectManager *pObjMan, IEngineCore *pEngCore):
	_objType(GOT_UNKNOWN), _pObjMan(pObjMan), _pEngineCore(pEngCore),
	_fSize(10.f), _fColScale(1.f), _fAngle(0.f),
	_uiCounter(0), _iRenderLayer(0)
	{
		IRender *p_render;
		_pEngineCore->GetSubSystem(ESS_RENDER, (IEngineSubSystem *&)p_render);
		p_render->GetRender2D(_pRender2D);
	}

	virtual ~CGameObject()
	{}

	static bool s_CompareFunction(CGameObject *obj1, CGameObject *obj2)
	{
		return (obj1->_iRenderLayer < obj2->_iRenderLayer);
	}

	E_GAME_OBJECT_TYPE GetType() const
	{
		return _objType;
	}

	virtual void Update()
	{
		++_uiCounter;
	}
	
	virtual void Draw()
	{
		// draw debug circles if needed
		if (_pObjMan->IsDebugMode())
		{
			_pRender2D->DrawCircle(_stPos, (uint)(_fSize / 2.f), 32, ColorWhite());
			_pRender2D->DrawCircle(_stPos, (uint)(_fSize * _fColScale / 2.f), 32, ColorRed());
		}
	}

	virtual bool CollisionCheck(CGameObject *pObjWith)
	{
		// make fast and simple circle based collision detection
		return _stPos.DistTo(pObjWith->_stPos) < _fSize / 2.f * _fColScale + pObjWith->_fSize / 2.f * pObjWith->_fColScale;
	}

	virtual void OnCollision(CGameObject *pObjWith)
	{}

	// every object defines for itself if it needs collision check or not
	virtual bool IsCollidable() const
	{
		return false;
	}
};

// CBackground
class CBackground : public CGameObject
{
private:

	IMesh *_pMeshPlanet;
	ITexture *_pTexSpace, *_pTexEarth, *_pTexClouds;

public:

	CBackground(IGameObjectManager *pObjMan, IEngineCore *pEngCore):
	CGameObject(pObjMan, pEngCore)
	{
		_iRenderLayer = -1; // Must be the first object to draw!
		
		IResourceManager *p_res_man;
		_pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)p_res_man);
		p_res_man->GetResourceByName(MESH_PLANET, (IEngineBaseObject *&)_pMeshPlanet);
		p_res_man->GetResourceByName(TEX_STARS, (IEngineBaseObject *&)_pTexSpace);
		p_res_man->GetResourceByName(TEX_PLANET, (IEngineBaseObject *&)_pTexEarth);
		p_res_man->GetResourceByName(TEX_CLOUDS, (IEngineBaseObject *&)_pTexClouds);
	}

	void Draw()
	{
		// draw background stars
		_pTexSpace->Draw2D(0, 0, GAME_VP_WIDTH, GAME_VP_HEIGHT);

		// draw planet
		_pRender2D->DrawMesh(_pMeshPlanet, _pTexEarth, TPoint2(GAME_VP_WIDTH / 2.f, GAME_VP_HEIGHT / 2.f), TPoint3(350, 350, 350),
			TPoint3(0.f, 1.f, 0.f), (float)_uiCounter / 2.f, EF_DEFAULT, false);

		// draw atmosphere
		_pRender2D->SetColorMix(TColor4(255, 255, 255, 150));
		_pRender2D->DrawMesh(_pMeshPlanet, _pTexClouds, TPoint2(GAME_VP_WIDTH / 2.f, GAME_VP_HEIGHT / 2.f), TPoint3(375, 375, 375),
			TPoint3(0.35f, -0.4f, 0.25f),(float)_uiCounter, (E_EFFECT2D_FLAGS)(EF_BLEND | EF_COLOR_MIX), false, 90.f, true /* after rendering clear depth buffer */ );
	}
};

// CExplo
class CExplo : public CGameObject
{
private:

	ITexture *_pTexExplo;

public:

	CExplo(IGameObjectManager *pObjMan, IEngineCore *pEngCore, const TPoint2 &stPos, float fSize):
	CGameObject(pObjMan, pEngCore)
	{
		_iRenderLayer = 4;
		_fSize = fSize * 1.5f;
		_stPos = stPos;
		_fAngle	= (float)(rand() % 360);

		IResourceManager *p_res_man;
		_pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)p_res_man);
		p_res_man->GetResourceByName(TEX_EXPLO, (IEngineBaseObject *&)_pTexExplo);
		
		ISoundSample *p_snd;
		p_res_man->GetResourceByName(SND_EXPLO, (IEngineBaseObject *&)p_snd);
		p_snd->Play();
	}

	void Update()
	{
		CGameObject::Update();

		if (_uiCounter / 2 == EXPLO_ANIMATION_FRAMES_COUNT)
			_pObjMan->RemoveObject(this);
	}

	void Draw()
	{
		_pRender2D->SetBlendMode(BE_ADD);
		_pRender2D->DrawTextureSprite(_pTexExplo, TPoint2(_stPos.x - _fSize / 2.f, _stPos.y - _fSize / 2.f), TPoint2(_fSize, _fSize), _uiCounter / 2, _fAngle, EF_BLEND);
	}
};

// CScorePopup
class CScorePopup : public CGameObject
{
private:

	uint _uiScore;
	IBitmapFont *_pFnt;

public:

	CScorePopup(IGameObjectManager *pObjMan, IEngineCore *pEngCore, const TPoint2 &stPos, float fSize, uint uiScore):
	CGameObject(pObjMan, pEngCore), _uiScore(uiScore)
	{
		_pObjMan->IncreaseGameScore(uiScore);

		_iRenderLayer = 5;
		_fSize = CLAMP(fSize / 100.f, 0.25f, 5.f); // used as scale in this class
		_stPos = stPos;

		IResourceManager *p_res_man;
		_pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)p_res_man);
		p_res_man->GetResourceByName(FNT_MAIN, (IEngineBaseObject *&)_pFnt);
	}

	void Update()
	{		
		CGameObject::Update();

		_fSize *= 1.025f;

		if (_uiCounter == 50)
			_pObjMan->RemoveObject(this);
	}

	void Draw()
	{
		char res[16];
		sprintf_s(res, "%u", _uiScore);

		uint w, h;

		_pFnt->SetScale(_fSize);
		_pFnt->GetTextDimensions(res, w, h);	
		_pRender2D->SetBlendMode(BE_NORMAL);
		_pFnt->Draw2D(_stPos.x - w / 2.f, _stPos.y - h / 2.f, res, ColorWhite(255 - _uiCounter * 5));
	}
};

// CSpark
class CSpark : public CGameObject
{
private:

	ITexture *_pTexSpark;

public:

	CSpark(IGameObjectManager *pObjMan, IEngineCore *pEngCore, const TPoint2 &stPos):
	CGameObject(pObjMan, pEngCore)
	{
		_iRenderLayer = 1;
		_fSize = 96.f;
		_stPos = stPos;
		_fAngle	= (float)(rand() % 360);

		IResourceManager *p_res_man;
		_pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)p_res_man);
		p_res_man->GetResourceByName(TEX_SPARK, (IEngineBaseObject *&)_pTexSpark);
	}

	void Update()
	{		
		CGameObject::Update();

		if (_uiCounter == 20)
			_pObjMan->RemoveObject(this);
	}

	void Draw()
	{
		_pRender2D->SetBlendMode(BE_ADD);
		_pRender2D->SetColorMix(TColor4(255, 128, 40, 255 - _uiCounter * 12));
		_pRender2D->DrawTexture(_pTexSpark, TPoint2(_stPos.x - _fSize / 2.f, _stPos.y - _fSize / 2.f), TPoint2(_fSize, _fSize), _fAngle, E_EFFECT2D_FLAGS(EF_COLOR_MIX | EF_BLEND));
	}
};

// CAsteroid
class CAsteroid : public CGameObject
{
private:

	IMesh *_pMesh;
	ITexture *_pTex;

public:

	CAsteroid(IGameObjectManager *pObjMan, IEngineCore *pEngCore, const TPoint2 &stPos, float fSize):
	CGameObject(pObjMan, pEngCore)
	{
		_objType = GOT_ASTEROID;

		_uiCounter = rand() % 500; // to randomize asteroid drawing starting rotation angle
		_iRenderLayer = 2;
		_fSize = fSize;
		_fColScale = 0.8f;
		_stPos = stPos;
		_fAngle	= (float)(rand() % 360);

		IResourceManager *p_res_man;
		_pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)p_res_man);
		p_res_man->GetResourceByName(MESH_ASTEROID, (IEngineBaseObject *&)_pMesh);
		p_res_man->GetResourceByName(TEX_ASTEROID, (IEngineBaseObject *&)_pTex);
	}

	void Update()
	{		
		CGameObject::Update();

		// move asteroid
		_stPos.x += cosf(_fAngle * TO_RAD) * CLAMP(100.f / _fSize, 0.5f, 50.f);
		_stPos.y += sinf(_fAngle * TO_RAD) * CLAMP(100.f / _fSize, 0.5f, 50.f);

		// asteroids fly beyond screen
		if (_stPos.x + _fSize / 2.f < 0) _stPos.x = (float)GAME_VP_WIDTH + _fSize / 2.f;
		if (_stPos.x - _fSize / 2.f > GAME_VP_WIDTH) _stPos.x = -_fSize / 2.f;
		if (_stPos.y + _fSize / 2.f < 0) _stPos.y = (float)GAME_VP_HEIGHT + _fSize / 2.f;
		if (_stPos.y - _fSize / 2.f > GAME_VP_HEIGHT) _stPos.y = -_fSize / 2.f;
	}

	void Draw()
	{
		_pRender2D->DrawMesh(_pMesh, _pTex, _stPos, TPoint3(_fSize, _fSize, _fSize), TVector3(0.6f, 0.2f, 0.4f), (float)_uiCounter * (250.f / _fSize), EF_DEFAULT, false);
		CGameObject::Draw();
	}

	bool IsCollidable() const
	{
		return true;
	}

	void OnCollision(CGameObject *pObjWith)
	{
		// collision with shot is fatal
		if (pObjWith->GetType() == GOT_SHOT || pObjWith->GetType() == GOT_PLAYER)
		{
			// spawn little children asteroids
			if (_fSize > 25.f)
			{
				uint count = 2 + rand() % 3;
				for (uint i = 0; i < count; ++i)
					_pObjMan->AddObject(new CAsteroid(_pObjMan, _pEngineCore,
					
					// randomize children asteroids position
					TPoint2(_stPos.x - _fSize / 4.f + rand() % (uint)(_fSize / 2.f), _stPos.y - _fSize / 4.f + rand() % (uint)_fSize / 2),
					
					_fSize * 0.5f));
			}

			_pObjMan->AddObject(new CExplo(_pObjMan, _pEngineCore, _stPos, _fSize));
			_pObjMan->AddObject(new CScorePopup(_pObjMan, _pEngineCore, _stPos, _fSize, (uint)_fSize * 5));
			_pObjMan->RemoveObject(pObjWith);
			_pObjMan->RemoveObject(this);
		}
	}
};

// CShot
class CShot : public CGameObject
{
private:

	const float _c_fSpeed;
	ITexture *_pTexSpark;

public:

	CShot(IGameObjectManager *pObjMan, IEngineCore *pEngCore, const TPoint2 &stPos, float fAngle):
	CGameObject(pObjMan, pEngCore), _c_fSpeed(20.f)
	{
		_objType = GOT_SHOT;

		_iRenderLayer = 3;
		_fSize = 64.f;
		_fColScale = 0.5f;
		_stPos = stPos;
		_fAngle	= fAngle;

		IResourceManager *p_res_man;
		_pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)p_res_man);
		p_res_man->GetResourceByName(TEX_SPARK, (IEngineBaseObject *&)_pTexSpark);
		
		ISoundSample *p_snd;
		p_res_man->GetResourceByName(SND_FIRE, (IEngineBaseObject *&)p_snd);
		p_snd->Play();
	}

	void Update()
	{		
		CGameObject::Update();

		// move shot spark
		_stPos.x += cosf(_fAngle * TO_RAD) * _c_fSpeed;
		_stPos.y += sinf(_fAngle * TO_RAD) * _c_fSpeed;

		// die if out of screen bounds
		if (_stPos.x < 0 || _stPos.x > GAME_VP_WIDTH || _stPos.y < 0 || _stPos.y > GAME_VP_HEIGHT)
			_pObjMan->RemoveObject(this);
	}

	void Draw()
	{
		_pRender2D->SetBlendMode(BE_ADD);
		_pRender2D->SetColorMix(ColorRed());
		_pRender2D->DrawTexture(_pTexSpark, TPoint2(_stPos.x - _fSize / 2.f, _stPos.y - _fSize / 2.f), TPoint2(_fSize, _fSize), (float)_uiCounter * 2.f, E_EFFECT2D_FLAGS(EF_COLOR_MIX | EF_BLEND));

		CGameObject::Draw();
	}

	bool IsCollidable() const
	{
		return true;
	}
};

// CPLayer
class CPlayer : public CGameObject
{
private:

	const float _c_uiShootingDelay;

	IInput *_pInput;
	IMesh *_pMesh;
	ITexture *_pTex, *_pTexSpark;
	float _fVelocity;
	uint _uiShotPause;

	void _FireEngine(int side /* -1 is left, 1 is right*/ )
	{
		const float multip = (float)CLAMP(side, -1, 1);
		
		_pObjMan->AddObject(new CSpark(_pObjMan, _pEngineCore,
			TPoint2(_stPos.x - cosf((_fAngle - multip * 20.f) * TO_RAD) * 75.f, _stPos.y - sinf((_fAngle - multip * 20.f) * TO_RAD) * 75.f)));
	}

	void _MakeShot()
	{
		if (_uiShotPause > _c_uiShootingDelay)
		{
			_pObjMan->AddObject(new CShot(_pObjMan, _pEngineCore,
				TPoint2(_stPos.x + cosf(_fAngle * TO_RAD) * 65.f, _stPos.y + sinf(_fAngle * TO_RAD) * 65.f), _fAngle));

			_uiShotPause = 0;
		}
	}

public:

	CPlayer(IGameObjectManager *pObjMan, IEngineCore *pEngCore):
	CGameObject(pObjMan, pEngCore), _c_uiShootingDelay(15),
	_fVelocity(0.f), _uiShotPause(15)
	{
		_objType = GOT_PLAYER;

		_stPos = TPoint2(GAME_VP_WIDTH / 2.f, GAME_VP_HEIGHT / 2.f);
		_fSize = 150.f;
		_fColScale = 0.6f;

		_pEngineCore->GetSubSystem(ESS_INPUT, (IEngineSubSystem *&)_pInput);
	
		IResourceManager *p_res_man;
		_pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)p_res_man);

		p_res_man->GetResourceByName(MESH_SHIP, (IEngineBaseObject *&)_pMesh);
		p_res_man->GetResourceByName(TEX_SHIP, (IEngineBaseObject *&)_pTex);
		p_res_man->GetResourceByName(TEX_SPARK, (IEngineBaseObject *&)_pTexSpark);
	}

	void Update()
	{
		CGameObject::Update();

		++_uiShotPause;

		// grab user input
		bool is_pressed;
		
		_pInput->GetKey(KEY_SPACE, is_pressed);
		if (is_pressed) _MakeShot();

		_pInput->GetKey(KEY_LEFT, is_pressed);
		if (is_pressed)
		{
			_fAngle -= 4.f;
			_FireEngine(-1); // fire left engine
		}

		_pInput->GetKey(KEY_RIGHT, is_pressed);
		if (is_pressed)
		{
			_fAngle += 4.f;
			_FireEngine(1); // fire right engine
		}

		_pInput->GetKey(KEY_UP, is_pressed);
		if (is_pressed)
		{
			_fVelocity = CLAMP(_fVelocity + 0.2f, 0.f, 8.f);

			// fire both engines
			_FireEngine(-1);
			_FireEngine(1);
		}
		else
		{
			_fVelocity = CLAMP(_fVelocity - 0.25f, 0.f, 8.f);
			
			// fire both engines sometimes
			if (_uiCounter % 4 == 0)
			{
				_FireEngine(-1);
				_FireEngine(1);
			}
		}

		// change player position
		_stPos.x = CLAMP(_stPos.x + cosf(_fAngle * TO_RAD) * _fVelocity, 0.f, GAME_VP_WIDTH);
		_stPos.y = CLAMP(_stPos.y + sinf(_fAngle * TO_RAD) * _fVelocity, 0.f, GAME_VP_HEIGHT);
	}

	void Draw()
	{
		_pRender2D->DrawMesh(_pMesh, _pTex, _stPos, TPoint3(_fSize, _fSize, _fSize), TVector3(0.f, 0.f, 1.f), _fAngle - 90.f, EF_DEFAULT, false);
	
		_pRender2D->SetBlendMode(BE_ADD);
		_pRender2D->SetColorMix(ColorRed());
		
		const float spark_scale = CLAMP((float)_uiShotPause / (float)_c_uiShootingDelay, 0.f, 1.f);
		_pRender2D->DrawTexture(_pTexSpark, TPoint2(_stPos.x - 32.f * spark_scale + cosf(_fAngle * TO_RAD) * 65.f, _stPos.y - 32.f * spark_scale + sinf(_fAngle * TO_RAD) * 65.f),
			TPoint2(64.f * spark_scale, 64.f * spark_scale), (float)_uiCounter * 2.f, E_EFFECT2D_FLAGS(EF_COLOR_MIX | EF_BLEND));

		CGameObject::Draw();
	}

	void OnCollision(CGameObject *pObjWith)
	{
		// collision with asteroid is fatal
		if (pObjWith->GetType() == GOT_ASTEROID)
		{
			_pObjMan->AddObject(new CExplo(_pObjMan, _pEngineCore, _stPos, _fSize));
			_pObjMan->RemoveObject(this);
		}
	}

	bool IsCollidable() const
	{
		return true;
	}
};

// CGame
// This class is simple game objects manager.
CGame::CGame(IEngineCore *pEngineCore) :
_pEngineCore(pEngineCore), _pInput(NULL), _iDebug(0)
{
	_pEngineCore->AddUserCallback(this);
	_pEngineCore->ConsoleRegisterVariable("game_debug", "Display game debug information.", &_iDebug, 0, 1);
}

CGame::~CGame()
{
	_ClearGame();
}

void CGame::_StartGame()
{
	_uiScore = 0;
	AddObject(new CBackground(this, _pEngineCore));
	AddObject(new CPlayer(this, _pEngineCore));
	AddObject(new CAsteroid(this, _pEngineCore, TPoint2(200.f, 100.f), 300.f));
}

void CGame::_ClearGame()
{
	for (size_t i = 0; i < _clObjects.size(); ++i)
		delete _clObjects[i];

	_clObjects.clear();
}

bool CGame::IsDebugMode() const
{
	return _iDebug == 1;
}

void CGame::IncreaseGameScore(uint uiByValue)
{
	_uiScore += uiByValue;
}

void CGame::AddObject(CGameObject *pObj)
{
	_clObjects.push_back(pObj);
}

void CGame::RemoveObject(CGameObject *pObj)
{
	_clRemovePendingList.push_back(pObj);
}

bool CGame::_IsPlayerExists() const
{
	for (size_t i = 0; i < _clObjects.size(); ++i)
		if (_clObjects[i]->GetType() == CGameObject::GOT_PLAYER)
			return true;

	return false;
}

bool CGame::_AreAsteroidsRemain() const
{
	for (size_t i = 0; i < _clObjects.size(); ++i)
		if (_clObjects[i]->GetType() == CGameObject::GOT_ASTEROID)
			return true;
	
	return false;
}

DGLE_RESULT DGLE_API CGame::Initialize()
{
	IRender *p_render;
	_pEngineCore->GetSubSystem(ESS_RENDER, (IEngineSubSystem *&)p_render);
	p_render->GetRender2D(_pRender2D);

	// All game logic is developed for this screen resolution, so will keep it (on logical level).
	_pRender2D->SetResolutionCorrection(GAME_VP_WIDTH, GAME_VP_HEIGHT);
	
	_pEngineCore->GetSubSystem(ESS_INPUT, (IEngineSubSystem *&)_pInput);
	_pInput->Configure(ICF_HIDE_CURSOR);
	
	IResourceManager *p_res_man;
	_pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)p_res_man);

	// Load resources and give them names to use in future via resource manager.
	
	p_res_man->Load(RESOURCE_PATH"\\fonts\\Times_New_Roman_18_Bold.dft", (IEngineBaseObject *&)_pFnt, RES_LOAD_DEFAULT, FNT_MAIN);
	
	IEngineBaseObject *p_tmp_obj;
	
	p_res_man->Load(RESOURCE_PATH"\\meshes\\planets\\planet.dmd", p_tmp_obj, MMLF_FORCE_MODEL_TO_MESH, MESH_PLANET);
	p_res_man->Load(RESOURCE_PATH"\\meshes\\planets\\clouds.jpg", p_tmp_obj, TEXTURE_LOAD_DEFAULT_3D, TEX_CLOUDS);
	p_res_man->Load(RESOURCE_PATH"\\meshes\\planets\\earth.jpg", p_tmp_obj, TEXTURE_LOAD_DEFAULT_3D, TEX_PLANET);
	p_res_man->Load(RESOURCE_PATH"\\sprites\\space_bg.jpg", p_tmp_obj, TEXTURE_LOAD_DEFAULT_2D, TEX_STARS);
	p_res_man->Load(RESOURCE_PATH"meshes\\space_ship\\ship.dmd", p_tmp_obj, MMLF_FORCE_MODEL_TO_MESH, MESH_SHIP);
	p_res_man->Load(RESOURCE_PATH"meshes\\planets\\asteroid.dmd", p_tmp_obj, MMLF_FORCE_MODEL_TO_MESH, MESH_ASTEROID);
	p_res_man->Load(RESOURCE_PATH"meshes\\space_ship\\ship.jpg", p_tmp_obj, TEXTURE_LOAD_DEFAULT_3D, TEX_SHIP);
	p_res_man->Load(RESOURCE_PATH"meshes\\planets\\asteroid.jpg", p_tmp_obj, TEXTURE_LOAD_DEFAULT_3D, TEX_ASTEROID);
	p_res_man->Load(RESOURCE_PATH"sprites\\spark.bmp", p_tmp_obj, TEXTURE_LOAD_DEFAULT_2D, TEX_SPARK);
	p_res_man->Load(RESOURCE_PATH"sounds\\explo.wav", p_tmp_obj, RES_LOAD_DEFAULT, SND_EXPLO);
	p_res_man->Load(RESOURCE_PATH"sounds\\fire.wav", p_tmp_obj, RES_LOAD_DEFAULT, SND_FIRE);

	p_res_man->Load(RESOURCE_PATH"sprites\\explo.jpg", p_tmp_obj, TEXTURE_LOAD_DEFAULT_2D, TEX_EXPLO);
	((ITexture *)p_tmp_obj)->SetFrameSize(EXPLO_ANIMATION_FRAME_SIZE, EXPLO_ANIMATION_FRAME_SIZE);

	_StartGame();

	return S_OK;
}

DGLE_RESULT DGLE_API CGame::Free()
{
	return S_OK;
}

DGLE_RESULT DGLE_API CGame::Update(uint uiDeltaTime)
{
	bool is_pressed;

	// exit by pressing "Esc" key
	_pInput->GetKey(KEY_ESCAPE, is_pressed);
	if (is_pressed)
	{
		_pEngineCore->QuitEngine();
		return S_OK;
	}

	// if player is dead restart game on pressing "Enter"
	_pInput->GetKey(KEY_RETURN, is_pressed);
	if (is_pressed && !_IsPlayerExists())
	{
		_ClearGame();
		_StartGame();
	}

	// update all game objects
	for (size_t i = 0; i < _clObjects.size(); ++i)
		_clObjects[i]->Update();

	// do collision check
	size_t objs_count = _clObjects.size(); // save object count not to iterate through objects being created in collision handlers
	for (size_t i = 0; i < objs_count; ++i)
		if (_clObjects[i]->IsCollidable())
			for (size_t j = 0; j < _clObjects.size(); ++j)
				if (_clObjects[j]->IsCollidable() && i != j
					&& _clObjects[i]->CollisionCheck(_clObjects[j]))
					_clObjects[i]->OnCollision(_clObjects[j]);

	// remove objects waiting to be removed
	for (size_t i = 0; i < _clRemovePendingList.size(); ++i)
		for (size_t j = 0; j < _clObjects.size(); ++j)
			if (_clObjects[j] == _clRemovePendingList[i])
			{
				delete _clObjects[j];
				_clObjects.erase(_clObjects.begin() + j);
				break;
			}
	
	_clRemovePendingList.clear();

	// sort objects by render layer before drawing
	sort(_clObjects.begin(), _clObjects.end(), CGameObject::s_CompareFunction);

	// if all asteroids are destroyed spawn new
	if (!_AreAsteroidsRemain())
		AddObject(new CAsteroid(this, _pEngineCore, TPoint2(-290.f, GAME_VP_HEIGHT / 2.f), 300.f));

	return S_OK;
}

DGLE_RESULT DGLE_API CGame::Render()
{
	// render game objects
	for (size_t i = 0; i < _clObjects.size(); ++i)
		_clObjects[i]->Draw();
	
	// render in-game user interface

	_pRender2D->SetBlendMode(BE_NORMAL);
	_pFnt->SetScale(1.f);

	// if player is dead draw message
	if (!_IsPlayerExists())
	{
		const char ac_txt[] = "You are dead! Press \"Enter\" to restart.";
		uint w, h;
		_pFnt->GetTextDimensions(ac_txt, w, h);
		_pFnt->Draw2DSimple((GAME_VP_WIDTH - w) / 2, (GAME_VP_HEIGHT - h) / 2, ac_txt, ColorRed());
	}

	// draw help before game start
	if (_uiScore == 0)
	{
		const char ac_txt[] = "Use \"Arrows\" to move and \"Space Bar\" to shoot.";
		uint w, h;
		_pFnt->GetTextDimensions(ac_txt, w, h);
		_pFnt->Draw2DSimple((GAME_VP_WIDTH - w) / 2, GAME_VP_HEIGHT - h, ac_txt, ColorWhite());
	}

	// output score
	char res[7 + 16];
	sprintf_s(res, "Score: %u", _uiScore);
	uint w, h;
	_pFnt->GetTextDimensions(res, w, h);
	_pFnt->Draw2DSimple((GAME_VP_WIDTH - w) / 2, 0, res, ColorWhite());

	return S_OK;
}

DGLE_RESULT DGLE_API CGame::OnEvent(E_EVENT_TYPE eEventType, IBaseEvent *pEvent)
{
	return S_OK;
}