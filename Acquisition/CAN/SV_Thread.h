/************************************************************
 *
 *  file: SV_Thread.h
 *
 *  purpose: base pthread class
 *
 *  author: Original by
 *      Jeremy Friesner 09: http://stackoverflow.com/questions/1151582/pthread-function-from-a-class
 *      Adapted for Beaglebone solar vehicle code Jan. 2015, J. C. Wiley
 *
 ************************************************************/
#ifndef _SV_THREAD_H
#define _SV_THREAD_H
#include "SV_stdlibs.h"

class SV_Thread {
public:
	SV_Thread() {
		 /* empty */
	}
	virtual ~SV_Thread() {
		/* empty */
	}

	/** Returns true if the thread was successfully started,
	 *  false if there was an error starting the thread
	 *  */
	bool StartInternalThread() {
		return (pthread_create(&_thread, NULL, InternalThreadEntryFunc, this) == 0);
	}

	/** Will not return until the internal thread has exited. */
	void WaitForInternalThreadToExit() {
		(void) pthread_join(_thread, NULL);
	}

protected:
	/** Implement this method in your subclass with
	 *  the code you want your thread to run. */
	virtual void InternalThreadEntry() = 0;

private:
	static void* InternalThreadEntryFunc(void * This) {
		((SV_Thread *) This)->InternalThreadEntry();
		return NULL;
	}

	pthread_t _thread;
};

#endif
