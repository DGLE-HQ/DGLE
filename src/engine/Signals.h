/**
\author		Alexey Shaydurov aka ASH
\date		02.01.2017 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/

#pragma once

#include <cassert>
#include <algorithm>
#include <iterator>
#include <functional>
#include <list>
#include <memory>
#include <utility>

namespace Signals
{
	template<typename ...Args>
	class Signal;

	template<typename ...Args>
	class Connection;

	template<typename ...Args>
	std::shared_ptr<Signal<Args...>> MakeSignal();

	template<typename ...Args>
	class Signal final : public std::enable_shared_from_this<Signal<Args...>>
	{
		friend class Connection<Args...>;
		friend std::shared_ptr<Signal<Args...>> MakeSignal<Args...>();

	private:
		template<class T = Signal<Args...>>
		class Allocator;

		/*
			VS 2015 STL uses allocator's construct() to construct combined object (main object + shared ptr data)
			but constructs containing object directly via placement new which does not have access to private members.
			GCC at the same time compiles it fine.
		*/
#if defined _MSC_VER && _MSC_VER <= 1900
	public:
#else
	private:
#endif
		Signal() = default;
		~Signal() = default;
		Signal(Signal &) = delete;
		void operator =(Signal &) = delete;

	private:
		mutable std::list<std::function<void(Args...)>> slots;
		mutable unsigned int recursionDepth = 0;
		mutable bool dirty = false;

	public:
		using std::enable_shared_from_this<Signal<Args...>>::shared_from_this;

	public:
		Connection<Args...> Connect(std::function<void(Args...)> slot);
		bool Empty() const noexcept;
		void operator ()(Args ...args) const;
	};

	template<typename ...Args>
	class Connection
	{
		friend class Signal<Args...>;

	private:
		std::weak_ptr<Signal<Args...>> signal;
		typename std::enable_if_t<true, decltype(Signal<Args...>::slots)>::iterator slot;	// enable_if_t - workaround for VS 2015

	public:
		Connection(Connection &&) = default;
		Connection &operator =(Connection &&) = default;

	private:
		Connection(decltype(signal) &&signal, decltype(slot) slot) noexcept : signal(std::move(signal)), slot(slot) {}

	public:
		void Disconnect() noexcept;
	};

	template<typename ...Args>
	struct ScopedConnection : Connection<Args...>
	{
		typedef Connection<Args...> Connection;
		ScopedConnection(ScopedConnection &&) = default;
		ScopedConnection(Connection &&src) noexcept : Connection(std::move(src)) {}
		ScopedConnection &operator =(Connection &&src) noexcept { return Connection::operator =(std::move(src)), *this; }
		ScopedConnection &operator =(ScopedConnection &&src) = default;
		~ScopedConnection() noexcept { Connection::Disconnect(); }
	};
}

#pragma region implementation
// custom allocator needed because standard one does not have access to private ctor/dtor
template<typename ...Args>
template<class T>
class Signals::Signal<Args...>::Allocator : public std::allocator<T>
{
public:
	template<class Other>
	struct rebind
	{
		typedef Allocator<Other> other;
	};

public:
	using std::allocator<T>::allocator;

public:
	template<class Class, typename ...Params>
	void construct(Class *p, Params &&...params)
	{
		new(p) Class(std::forward<Params>(params)...);
	}

	template<class Class>
	void destroy(Class *p)
	{
		p->~Class();
	}
};

template<typename ...Args>
auto Signals::Signal<Args...>::Connect(std::function<void(Args...)> slot) -> Connection<Args...>
{
	slots.push_back(std::move(slot));
	return{ shared_from_this(), std::prev(slots.end()) };
}

template<typename ...Args>
bool Signals::Signal<Args...>::Empty() const noexcept
{
	return dirty ? std::find_if(slots.cbegin(), slots.cend(), [](typename decltype(slots)::const_reference slot) { return slot; }) == slots.cend() : slots.empty();
}

template<typename ...Args>
void Signals::Signal<Args...>::operator ()(Args ...args) const
{
	const class ScopedCleanup
	{
		const std::shared_ptr<const Signal> signal;	// also holds reference to keep Signal alive when Signals destroyed

	public:
		ScopedCleanup(ScopedCleanup &) = delete;
		void operator = (ScopedCleanup &) = delete;
		ScopedCleanup(std::shared_ptr<const Signal> &&signal) noexcept : signal(std::move(signal))
		{
			this->signal->recursionDepth++;
			assert(this->signal->recursionDepth);	// check for overflow
		}
		~ScopedCleanup() noexcept
		{
			if (!--signal->recursionDepth && signal->dirty)
			{
				signal->slots.remove_if([](typename decltype(slots)::const_reference slot) -> bool { return !slot; });
				signal->dirty = false;
			}
		}
	} scopedCleanup(shared_from_this());

	for (const auto &slot : slots)
		if (slot)
			slot(std::forward<Args>(args)...);
}

template<typename ...Args>
void Signals::Connection<Args...>::Disconnect() noexcept
{
	if (!signal.expired())
	{
		const auto lockedSig = signal.lock();
		if (lockedSig->recursionDepth)
		{
			*slot = nullptr;
			lockedSig->dirty = true;
		}
		else
			lockedSig->slots.erase(slot);
		signal.reset();
	}
}

template<typename ...Args>
auto Signals::MakeSignal() -> std::shared_ptr<Signal<Args...>>
{
	return std::allocate_shared<Signal<Args...>>(typename Signal<Args...>::Allocator<>());
}
#pragma endregion