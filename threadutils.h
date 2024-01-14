#pragma once
#include <atomic>
#include <shared_mutex>
#include "utils.h"

// class Semaphore : private UHandle<HANDLE, &CloseHandle>
// {
// public:
// 	Semaphore(long max, long initialCount = 0)
// 		: UHandle<HANDLE, &CloseHandle>(
// 			CreateSemaphoreW(
// 				nullptr,
// 				initialCount,
// 				max,
// 				nullptr
// 			)
// 		  )
// 	{
// 		if (!*this)
// 			throw std::runtime_error("failed to create semaphore");
// 	}

// 	~Semaphore()
// 	{
// 		while (ReleaseSemaphore(*this, 1, nullptr));
// 	}

// 	void acquire()
// 	{
// 		if (WaitForSingleObject(*this, INFINITE) != WAIT_OBJECT_0)
// 			throw std::runtime_error("failed to acquire semaphore");
// 	}

// 	bool tryAcquire()
// 	{
// 		const auto r = WaitForSingleObject(*this, 0);
// 		switch (r)
// 		{
// 			case WAIT_OBJECT_0: return true;
// 			case WAIT_TIMEOUT: return false;
// 		}
// 		throw std::runtime_error("failed to acquire semaphore");
// 	}

// 	void release(long count = 1)
// 	{
// 		if (!ReleaseSemaphore(*this, count, nullptr))
// 			throw std::runtime_error("failed to release semaphore");
// 	}

// 	void tryRelease() noexcept
// 	{
// 		ReleaseSemaphore(*this, 1, nullptr);
// 	}
// };

class Waiter
{
private:
	std::atomic_uintptr_t a;
public:
	Waiter() noexcept : a(0) {}

	void wait() noexcept
	{
		while (!a.exchange(0, std::memory_order_acquire))
		{
			a.wait(0, std::memory_order_relaxed);
		}
	}

	void requestWakeup() noexcept
	{
		a.store(1, std::memory_order_release);
		a.notify_one();
	}
};

template <bool relaxed = false>
class AtomicFlagLock : public std::atomic_flag
{
public:
	class AlreadyLocked : public WRE {};

	AtomicFlagLock()
		: std::atomic_flag{} {}

	void lock()
	{
		if constexpr (relaxed)
		{
			if (test_and_set(std::memory_order_relaxed))
				throw AlreadyLocked(L"Lock is already locked");
		}
		else
		{
			if (test_and_set())
				throw AlreadyLocked(L"Lock is already locked");
		}
	}

	bool try_lock() noexcept
	{
		if constexpr (relaxed)
			return !test_and_set(std::memory_order_relaxed);
		else
			return !test_and_set();
	}

	void unlock() noexcept
	{
		if constexpr (relaxed)
			clear(std::memory_order_relaxed);
		else
			clear();
	}
};

class SpecialReaderLock
{
private:
	std::atomic_uint64_t nextQueueNumber;
	std::atomic_uint64_t nowServing;

	std::atomic_uint64_t sl;
	std::atomic_flag sr;
	std::atomic_flag el;

	void waitInLine() noexcept
	{
		const std::uint64_t num = nextQueueNumber++;
		std::uint64_t serving;
		while ((serving = nowServing.load()) != num)
		{
			nowServing.wait(serving);
		}
	}

	void serveNext() noexcept
	{
		nowServing++;
		nowServing.notify_all();
	}

	template <typename T>
	static T clearIfTrue(T val, std::atomic_flag& af) noexcept
	{
		if (val)
		{
			af.clear();
			af.notify_all();
		}
		return val;
	};

	template <typename T>
	static T subIfTrue(T val, std::atomic_uint64_t& a) noexcept
	{
		if (val)
		{
			a--;
			a.notify_all();
		}
		return val;
	};
public:
	SpecialReaderLock() noexcept :
		nextQueueNumber(0ull),
		nowServing(0ull),
		sl(0ull),
		sr{},
		el{} {}
	
	void lock() noexcept
	{
		waitInLine();
		do
		{
			do
			{
				do
				{
					while (const std::uint64_t old = sl.load())
					{
						sl.wait(old);
					}
					sr.wait(true);
					el.wait(true);
				} while (el.test_and_set());
				// el.notify_all();
			} while (clearIfTrue(sl.load(), el));
		} while (clearIfTrue(sr.test(), el));
		serveNext();
	}

	/* Väntar på trådar med shared eller exclusive access, returnerar
	   falskt om den specialla läsaren har låst. */
	bool try_lock() noexcept
	{
		waitInLine();
		do
		{
			do
			{
				if (sr.test())
				{
					serveNext();
					return false;
				}
				while (const std::uint64_t old = sl.load())
				{
					sl.wait(old);
				}
				el.wait(true);
			} while (el.test_and_set());
			// el.notify_all();
			if (sr.test())
			{
				el.clear();
				el.notify_all();
				serveNext();
				return false;
			}
		} while (clearIfTrue(sl.load(), el));
		serveNext();
		return true;
	}

	void unlock() noexcept
	{
		el.clear();
		el.notify_all();
	}

	void lock_shared() noexcept
	{
		waitInLine();
		do
		{
			el.wait(true);
			sl++;
			sl.notify_all();
		} while (subIfTrue(el.test(), sl));
		serveNext();
	}

	void unlock_shared() noexcept
	{
		sl--;
		sl.notify_all();
	}

	void lock_special() noexcept
	{
		waitInLine();
		do
		{
			do
			{
				el.wait(true);
				sr.wait(true);
			} while (sr.test_and_set());
			// sr.notify_all();
		} while (clearIfTrue(el.test(), sr));
		serveNext();
	}

	void unlock_special() noexcept
	{
		sr.clear();
		sr.notify_all();
	}

	// Uppgradera från speciell till speciell och exklusiv.
	void upgrade() noexcept
	{
		waitInLine();
		do
		{
			while (const std::uint64_t old = sl.load())
			{
				sl.wait(old);
			}
			do
			{
				el.wait(true);
			} while (el.test_and_set());
			// el.notify_all();
		} while (clearIfTrue(sl.load(), el));
		serveNext();
	}
};

class ExclusiveLockGuard
{
private:
	std::shared_lock<SpecialReaderLock>& sl;
	std::unique_lock<SpecialReaderLock> ul;
public:
	ExclusiveLockGuard(std::shared_lock<SpecialReaderLock>& sl) :
		sl(sl),
		ul(*sl.mutex(), std::defer_lock) {}
	~ExclusiveLockGuard()
	{
		if (ul.owns_lock())
		{
			ul.unlock();
			sl.lock();
		}
	}

	ExclusiveLockGuard(const ExclusiveLockGuard&) = delete;
	ExclusiveLockGuard& operator=(const ExclusiveLockGuard&) = delete;

	void lock()
	{
		sl.unlock();
		try
		{
			ul.lock();
		}
		catch (...)
		{
			sl.lock();
			throw;
		}
	}

	bool try_lock()
	{
		sl.unlock();
		bool locked = false;
		try
		{
			locked = ul.try_lock();
		}
		catch (...)
		{
			sl.lock();
			throw;
		}
		if (locked)
		{
			return true;
		}
		else
		{
			sl.lock();
			return false;
		}
	}

	void unlock()
	{
		ul.unlock();
		sl.lock();
	}
};

class SpecialLockGuard
{
private:
	SpecialReaderLock& srl;
public:
	SpecialLockGuard(SpecialReaderLock& srl) noexcept :
		srl(srl)
	{
		srl.lock_special();
	}
	~SpecialLockGuard()
	{
		srl.unlock_special();
	}
};

class UpgradedLockGuard
{
private:
	SpecialReaderLock& srl;
public:
	UpgradedLockGuard(SpecialReaderLock& srl) noexcept :
		srl(srl)
	{
		srl.upgrade();
	}
	~UpgradedLockGuard()
	{
		srl.unlock();
	}
};