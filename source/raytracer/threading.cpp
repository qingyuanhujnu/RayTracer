#include "threading.hpp"
#include <windows.h>

class LockImp
{
public:
	LockImp ()
	{
	
	}

	virtual ~LockImp ()
	{
	
	}

	virtual void	Enter () = 0;
	virtual void	Leave () = 0;
};

class ThreadImp
{
public:
	ThreadImp ()
	{
	
	}
	
	virtual ~ThreadImp ()
	{
	
	}

	virtual bool Start (const Thread* thread) = 0;
	virtual void Wait () = 0;

protected:
	static int StaticThreadProcess (void* parameter)
	{
		Thread* thread = static_cast<Thread*> (parameter);
		if (thread == nullptr) {
			return 1;
		}
		return thread->Do ();
	}
};

class WinapiLockImp : public LockImp
{
public:
	WinapiLockImp () :
		LockImp ()
	{
		InitializeCriticalSection (&cs);
	}

	virtual ~WinapiLockImp ()
	{
		DeleteCriticalSection (&cs);
	}

	virtual void Enter ()
	{
		EnterCriticalSection (&cs);
	}
	
	virtual void Leave ()
	{
		LeaveCriticalSection (&cs);
	}

private:
	CRITICAL_SECTION	cs;
};

class WinapiThreadImp : public ThreadImp
{
public:
	WinapiThreadImp () :
		ThreadImp (),
		threadHandle (INVALID_HANDLE_VALUE)
	{
		
	}

	virtual ~WinapiThreadImp ()
	{
	
	}

	virtual bool Start (const Thread* thread)
	{
		threadHandle = CreateThread (nullptr, 0, (LPTHREAD_START_ROUTINE) StaticThreadProcess, (LPVOID) thread, 0, nullptr);
		return threadHandle != INVALID_HANDLE_VALUE;
	}

	virtual void Wait ()
	{
		WaitForSingleObject (threadHandle, INFINITE);
	}

private:
	HANDLE threadHandle;
};

Lock::Lock ()
{
	imp.reset (new WinapiLockImp ());
}

Lock::~Lock ()
{

}

void Lock::Enter ()
{
	imp->Enter ();
}

void Lock::Leave ()
{
	imp->Leave ();
}

Thread::Thread ()
{
	imp.reset (new WinapiThreadImp ());
}

Thread::~Thread ()
{

}

bool Thread::Start ()
{
	return imp->Start (this);
}

void Thread::Wait ()
{
	imp->Wait ();
}
