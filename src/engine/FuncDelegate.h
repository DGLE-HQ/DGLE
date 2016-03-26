/**
\author		Korotkov Andrey aka DRON
\date		26.03.2016 (c)Korotkov Andrey

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

const char *FormWin32ExceptionString(DWORD dwCode);

#define CATCH_ALL_EXCEPTIONS(doCatch, instIdx, expression) \
if (!doCatch) expression else \
__try { expression }\
__except (EXCEPTION_EXECUTE_HANDLER)\
{\
	LogWrite(instIdx, FormWin32ExceptionString(GetExceptionCode()), LT_FATAL, __FILE__, __LINE__);\
}

#else

#define CATCH_ALL_EXCEPTIONS(doCatch, instIdx, expression) \
if (!doCatch) expression else \
try { expression }\
catch (const std::exception &exc)\
{\
	LogWrite(instIdx, (std::string("We are very sorry, but program crashed! Unhandled std exception occured with message \"") + exc.what() + "\".").c_str(), LT_FATAL, __FILE__, __LINE__);\
}\
catch (...)\
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

template<class T1, class T2>
class TCFuncDelegate;

template<typename F>
class CFunctor final: public CFunctorImpl<F>
{
	template<class T1, class T2>
	friend class TCFuncDelegate;

	using CFunctorImpl<F>::CFunctorImpl;
};

template<class T1, class T2>
class TCFuncDelegate final
{
	friend class CFunctorImpl<T2>;

	const uint _uiInstIdx;
	bool _bCatchExpts,
		 _bAllowInvoke;

	std::vector<std::pair<T1, void *>> _funcList;

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
		_funcList.emplace_back(pFunc, pParameter);
	}

	void Remove(T1 pFunc, void *pParameter)
	{
		// NOTE: removes first occurrence only (in FIFO fashion)
		const auto found = std::find(_funcList.cbegin(), _funcList.cend(), std::make_pair(pFunc, pParameter));
		if (found != _funcList.cend())
			_funcList.erase(found);
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

// Call loop moved to _Call() methods in order to cope with C2712 compiler error due to __try usage.

template<>
class CFunctorImpl<void ()>: CFunctorBase<TPProc, void ()>
{
	void _Call()
	{
#if 0
		for (const auto &func : _parent._funcList)
			func.first(func.second);
#else
		for (std::size_t i = 0; i < _parent._funcList.size(); ++i)
			_parent._funcList[i].first(_parent._funcList[i].second);
#endif
	}
protected:
	CFunctorImpl(TCFuncDelegate<TPProc, void ()> &parent): CFunctorBase(parent) {}
public:
	void operator ()()
	{
		if (_parent._bAllowInvoke)
			CATCH_ALL_EXCEPTIONS(_parent._bCatchExpts, _parent._uiInstIdx, _Call();)
	}
};

template<>
class CFunctorImpl<void (const TWindowMessage &)>: CFunctorBase<TPMsgProc, void (const TWindowMessage &)>
{
	void _Call(const TWindowMessage &stMsg)
	{
#if 0
		for (const auto &func : _parent._funcList)
			func.first(func.second, stMsg);
#else
		for (std::size_t i = 0; i < _parent._funcList.size(); ++i)
			_parent._funcList[i].first(_parent._funcList[i].second, stMsg);
#endif
	}
protected:
	CFunctorImpl(TCFuncDelegate<TPMsgProc, void (const TWindowMessage &)> &parent): CFunctorBase(parent) {}
public:
	void operator ()(const TWindowMessage &stMsg)
	{
		if (_parent._bAllowInvoke)
			CATCH_ALL_EXCEPTIONS(_parent._bCatchExpts, _parent._uiInstIdx, _Call(stMsg);)
	}
};

template<>
class CFunctorImpl<void (IBaseEvent *)>: CFunctorBase<TPEventProc, void (IBaseEvent *)>
{
	void _Call(IBaseEvent *pEvent)
	{
#if 0
		for (const auto &func : _parent._funcList)
			func.first(func.second, pEvent);
#else
		for (std::size_t i = 0; i < _parent._funcList.size(); ++i)
			_parent._funcList[i].first(_parent._funcList[i].second, pEvent);
#endif
	}
protected:
	CFunctorImpl(TCFuncDelegate<TPEventProc, void (IBaseEvent *)> &parent): CFunctorBase(parent) {}
public:
	void operator ()(IBaseEvent *pEvent)
	{
		if (_parent._bAllowInvoke)
			CATCH_ALL_EXCEPTIONS(_parent._bCatchExpts, _parent._uiInstIdx, _Call(pEvent);)
	}
};

typedef TCFuncDelegate<TPProc, void ()> TProcDelegate;
typedef TCFuncDelegate<TPMsgProc, void (const TWindowMessage &)> TMsgProcDelegate;
typedef TCFuncDelegate<TPEventProc, void (IBaseEvent *)> TEventProcDelegate;

}

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif