/**
\author		Korotkov Andrey aka DRON
\date		07.04.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4355) //this used in member init. list
#endif

#include <DGLE.h>
#include <exception>
#include <string>

void LogWrite(uint uiInstIdx, const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber);

#ifdef PLATFORM_WINDOWS

const char* FormWin32ExceptionString(DWORD dwCode);

#define CATCH_ALL_EXCEPTIONS(doCatch, instIdx, expression) \
if(!doCatch) expression else \
__try{ expression }\
__except(EXCEPTION_EXECUTE_HANDLER)\
{\
	LogWrite(instIdx, FormWin32ExceptionString(GetExceptionCode()), LT_FATAL, __FILE__, __LINE__);\
}

#else

#define CATCH_ALL_EXCEPTIONS(doCatch, instIdx, expression) \
if(!doCatch) expression else \
try{ expression }\
catch(const std::exception &exc)\
{\
	LogWrite(instIdx, (std::string("We are very sorry, but program crashed! Unhandled std exception occured with message \"") + std::string(exc.what()) + "\".").c_str(), LT_FATAL, __FILE__, __LINE__);\
}\
catch(...)\
{\
	LogWrite(instIdx, "We are very sorry, but program crashed! Unhandled cpp exception occured.", LT_FATAL, __FILE__, __LINE__);\
}

#endif

namespace DGLE
{

typedef void (DGLE_API *TPProc)(void *pParameter),
	(DGLE_API *TPEventProc)(void *pParameter, IBaseEvent *pEvent),
	(DGLE_API *TPMsgProc)(void *pParameter, const TWindowMessage &stMsg);

template<typename F>
class CFunctorImpl;

template<typename F>
class CFunctor: public CFunctorImpl<F>
{
	template<class T1, class T2>
	friend class TCFuncDelegate;

	template<class T1, class T2>
	CFunctor(TCFuncDelegate<T1, T2> &parent): CFunctorImpl<F>(parent) {}
};

template<class T1, class T2>
class TCFuncDelegate
{
	friend class CFunctorImpl<T2>;

	const uint _uiInstIdx;
	bool _bCatchExpts,
		 _bAllowInvoke;

	struct _TFunc
	{
		void *pParameter;
		T1	pFunc;
	};

	std::vector<_TFunc> _funcList;

public:

	TCFuncDelegate(uint uiInstIdx):
	  _uiInstIdx(uiInstIdx), _bAllowInvoke(true), _bCatchExpts(false), Invoke(*this)
	{}

	void CatchExceptions(bool bDoCatch)
	{
		_bCatchExpts = bDoCatch;
	}

	void AllowInvoke(bool bAllow)
	{
		_bAllowInvoke = bAllow;
	}

	inline bool IsNull()
	{
		return _funcList.empty();
	}

	void Add(T1 pFunc, void *pParameter)
	{
		_TFunc tmp;
		tmp.pParameter = pParameter;
		tmp.pFunc = pFunc;
		_funcList.insert(_funcList.begin(), tmp); // ToDo: was change for mouse wheel to work correct but need to be reconsidered cause can possible create errors in future.
		//_funcList.push_back(tmp);
	}

	void Remove(T1 pFunc, void *pParameter)
	{
		for (std::size_t i = 0; i < _funcList.size(); ++i)
			if (_funcList[i].pParameter == pParameter && _funcList[i].pFunc == pFunc)
			{
				_funcList.erase(_funcList.begin() + i);
				break;
			}
	}

	CFunctor<T2> Invoke;
};

template<class T1, class T2>
class CFunctorBase
{
protected:
	CFunctorBase(TCFuncDelegate<T1, T2> &parent): _parent(parent) {}
	TCFuncDelegate<T1, T2> &_parent;
};

template<>
class CFunctorImpl<void ()>: CFunctorBase<TPProc, void ()>
{
protected:
	CFunctorImpl(TCFuncDelegate<TPProc, void ()> &parent): CFunctorBase(parent) {}
public:
	void operator ()()
	{
		if (_parent._bAllowInvoke)
			CATCH_ALL_EXCEPTIONS(_parent._bCatchExpts, _parent._uiInstIdx,
			for (std::size_t i = 0; i < _parent._funcList.size(); ++i)
				(*_parent._funcList[i].pFunc)(_parent._funcList[i].pParameter);
			)
	}
};

template<>
class CFunctorImpl<void (const TWindowMessage &)>: CFunctorBase<TPMsgProc, void (const TWindowMessage &)>
{
protected:
	CFunctorImpl(TCFuncDelegate<TPMsgProc, void (const TWindowMessage &)> &parent): CFunctorBase(parent) {}
public:
	void operator ()(const TWindowMessage &stMsg)
	{
		if (_parent._bAllowInvoke)
			CATCH_ALL_EXCEPTIONS(_parent._bCatchExpts, _parent._uiInstIdx,
			for (std::size_t i = 0; i < _parent._funcList.size(); ++i)
				(*_parent._funcList[i].pFunc)(_parent._funcList[i].pParameter, stMsg);
			)
	}
};

template<>
class CFunctorImpl<void (IBaseEvent *)>: CFunctorBase<TPEventProc, void (IBaseEvent *)>
{
protected:
	CFunctorImpl(TCFuncDelegate<TPEventProc, void (IBaseEvent *)> &parent): CFunctorBase(parent) {}
public:
	void operator ()(IBaseEvent *pEvent)
	{
		if (_parent._bAllowInvoke)
			CATCH_ALL_EXCEPTIONS(_parent._bCatchExpts, _parent._uiInstIdx,
			for (std::size_t i = 0; i < _parent._funcList.size(); ++i)
				(*_parent._funcList[i].pFunc)(_parent._funcList[i].pParameter, pEvent);
			)
	}
};

typedef TCFuncDelegate<TPProc, void ()> TProcDelegate;
typedef TCFuncDelegate<TPMsgProc, void (const TWindowMessage &)> TMsgProcDelegate;
typedef TCFuncDelegate<TPEventProc, void (IBaseEvent *)> TEventProcDelegate;

}

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif