/**
\author		Korotkov Andrey aka DRON
\date		10.04.2016 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4355) //this used in member init. list
#endif

#include "DGLE.h"
#include "Signals.h"
#include <exception>
#include <string>
#include <functional>
#include <utility>

void LogWrite(uint uiInstIdx, const char *pcTxt, E_LOG_TYPE eType, const char *pcSrcFileName, int iSrcLineNumber);

#ifndef PLATFORM_WINDOWS

const char *FormWin32ExceptionString(DWORD dwCode);

#define CATCH_ALL_EXCEPTIONS(doCatch, instIdx, expression)											\
__try { expression }																				\
__except (doCatch ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)							\
{																									\
	LogWrite(instIdx, FormWin32ExceptionString(GetExceptionCode()), LT_FATAL, __FILE__, __LINE__);	\
}

#else

#define CATCH_ALL_EXCEPTIONS(doCatch, instIdx, expression)																																	\
try { expression }																																											\
catch (const std::exception &exc)																																							\
{																																															\
	if (!doCatch) throw;																																									\
	LogWrite(instIdx, (std::string("We are very sorry, but program crashed! Unhandled std exception occured with message \"") + exc.what() + "\".").c_str(), LT_FATAL, __FILE__, __LINE__);	\
}																																															\
catch (...)																																													\
{																																															\
	if (!doCatch) throw;																																									\
	LogWrite(instIdx, "We are very sorry, but program crashed! Unhandled cpp exception occured.", LT_FATAL, __FILE__, __LINE__);															\
}

#endif

namespace DGLE
{
	template<typename ...Args>
	class TCFuncDelegate final
	{
		const decltype(Signals::MakeSignal<Args...>()) _signal = Signals::MakeSignal<Args...>();
		const uint _uiInstIdx;
		bool	_bCatchExpts = false,
				_bAllowInvoke = true;

	public:

		TCFuncDelegate(uint uiInstIdx) : _uiInstIdx(uiInstIdx) {}

		void CatchExceptions(bool bDoCatch) noexcept
		{
			_bCatchExpts = bDoCatch;
		}

		void AllowInvoke(bool bAllow) noexcept
		{
			_bAllowInvoke = bAllow;
		}

		inline bool IsNull() const noexcept
		{
			return _signal->Empty();
		}

		auto Add(std::function<void (Args...)> callback)
		{
			return _signal->Connect(std::move(callback));
		}

		void operator ()(Args ...args) const
		{
			if (_bAllowInvoke)
				CATCH_ALL_EXCEPTIONS(_bCatchExpts, _uiInstIdx, (*_signal)(std::forward<Args>(args)...);)
		}
	};

	typedef TCFuncDelegate<>						TProcDelegate;
	typedef TCFuncDelegate<const TWindowMessage &>	TMsgProcDelegate;
	typedef TCFuncDelegate<IBaseEvent *>			TEventProcDelegate;
};

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif