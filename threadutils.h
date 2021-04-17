#pragma once
#include <atomic>
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
	EXCEPT(AlreadyLocked)

	AtomicFlagLock()
		: std::atomic_flag{} {}

	void lock()
	{
		if constexpr (relaxed)
		{
			if (test_and_set(std::memory_order_relaxed))
				throw AlreadyLocked("Lock is already locked");
		}
		else
		{
			if (test_and_set())
				throw AlreadyLocked("Lock is already locked");
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