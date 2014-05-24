#ifndef THREADING_HPP
#define THREADING_HPP
#include <memory>

class LockImp;
class ThreadImp;

class Lock
{
public:
	Lock ();
	~Lock ();

	void Enter ();
	void Leave ();

private:
	std::shared_ptr<LockImp>	imp;
};

class Thread
{
public:
	Thread ();
	virtual ~Thread ();

	bool			Start ();
	void			Wait ();

	virtual int		Do () = 0;

private:
	std::shared_ptr<ThreadImp>	imp;
};

#endif
