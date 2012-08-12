/**
\author		Korotkov Andrey aka DRON
\date		07.04.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/

#ifndef _FUNCDEL_H
#define _FUNCDEL_H

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4355) //this used in member init. list
#endif

#include <DGLE2.h>
#include <exception>
#include <string>

void LogWrite(uint uiInstIdx, const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber);

#ifdef PLATFORM_WINDOWS

const char* FormWin32ExceptionString(DWORD dwCode);

#define CATCH_ALL_EXCEPTIONS(expression) \
if(!_parent._bCatchExpts) expression else \
__try{ expression }\
__except(EXCEPTION_EXECUTE_HANDLER)\
{\
	LogWrite(_parent._uiInstIdx, FormWin32ExceptionString(GetExceptionCode()), LT_FATAL, __FILE__, __LINE__);\
}
#else
#define CATCH_ALL_EXCEPTIONS(expression) \
if(!_parent._bCatchExpts) expression else \
try{ expression }\
catch(const std::exception &exc)\
{\
	LogWrite(_parent._uiInstIdx, (std::string("We are very sorry, but program crashed! Unhandled std exception occured with message \"") + std::string(exc.what()) + "\".").c_str(), LT_FATAL, __FILE__, __LINE__);\
}\
catch(...)\
{\
	LogWrite(_parent._uiInstIdx, "We are very sorry, but program crashed! Unhandled cpp exception occured.", LT_FATAL, __FILE__, __LINE__);\
}
#endif

namespace DGLE2
{

typedef void	(CALLBACK *TPProc)(void *pParametr),
				(CALLBACK *TPEventProc)(void *pParametr, IBaseEvent *pEvent),
				(CALLBACK *TPMsgProc)(void *pParametr, const TWinMessage &stMsg);

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
		void *pParametr;
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

	void Add(T1 pFunc, void *pParametr)
	{
		_TFunc tmp;
		tmp.pParametr = pParametr;
		tmp.pFunc = pFunc;
		_funcList.push_back(tmp);
	}

	void Remove(T1 pFunc, void *pParametr)
	{
		for (std::size_t i = 0; i < _funcList.size(); i++)
			if (_funcList[i].pParametr == pParametr && _funcList[i].pFunc == pFunc)
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
			CATCH_ALL_EXCEPTIONS(
			for (std::size_t i = 0; i < _parent._funcList.size(); i++)
				(*_parent._funcList[i].pFunc)(_parent._funcList[i].pParametr);
			)
	}
};

template<>
class CFunctorImpl<void (const TWinMessage &)>: CFunctorBase<TPMsgProc, void (const TWinMessage &)>
{
protected:
	CFunctorImpl(TCFuncDelegate<TPMsgProc, void (const TWinMessage &)> &parent): CFunctorBase(parent) {}
public:
	void operator ()(const TWinMessage &stMsg)
	{
		if (_parent._bAllowInvoke)
			CATCH_ALL_EXCEPTIONS(
			for (std::size_t i = 0; i < _parent._funcList.size(); i++)
				(*_parent._funcList[i].pFunc)(_parent._funcList[i].pParametr, stMsg);
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
			CATCH_ALL_EXCEPTIONS(
			for (std::size_t i = 0; i < _parent._funcList.size(); i++)
				(*_parent._funcList[i].pFunc)(_parent._funcList[i].pParametr, pEvent);
			)
	}
};

#undef CATCH_ALL_EXCEPTIONS

typedef TCFuncDelegate<TPProc, void ()> TProcDelegate;
typedef TCFuncDelegate<TPMsgProc, void (const TWinMessage &)> TMsgProcDelegate;
typedef TCFuncDelegate<TPEventProc, void (IBaseEvent *)> TEventProcDelegate;

}

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif //_FUNCDEL_H