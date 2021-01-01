#pragma once
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "utils.h"

class Semaphore : private UHandle<HANDLE, &CloseHandle>
{
public:
	Semaphore(long max, long initialCount = 0)
		: UHandle<HANDLE, &CloseHandle>(
			CreateSemaphoreW(
				nullptr,
				initialCount,
				max,
				nullptr
			)
		  )
	{
		if (!*this)
			throw std::runtime_error("failed to create semaphore");
	}

	~Semaphore()
	{
		while (ReleaseSemaphore(*this, 1, nullptr));
	}

	void acquire()
	{
		if (WaitForSingleObject(*this, INFINITE) != WAIT_OBJECT_0)
			throw std::runtime_error("failed to acquire semaphore");
	}

	bool tryAcquire()
	{
		const auto r = WaitForSingleObject(*this, 0);
		switch (r)
		{
			case WAIT_OBJECT_0: return true;
			case WAIT_TIMEOUT: return false;
		}
		throw std::runtime_error("failed to acquire semaphore");
	}

	void release()
	{
		if (!ReleaseSemaphore(*this, 1, nullptr))
			throw std::runtime_error("failed to release semaphore");
	}
};