#include "pch.h"
#include "BlockingThreadPool.h"

InfinityOfficialNetwork::MediaServer::Native::Core::Shared::BlockingThreadPool::BlockingThreadPoolImpl::BlockingThreadPoolImpl () : waiting_threads (std::max (std::thread::hardware_concurrency (), 16u))
{}

void InfinityOfficialNetwork::MediaServer::Native::Core::Shared::BlockingThreadPool::BlockingThreadPoolImpl::Submit (TypeErasedFunctionInterface* task)
{
	ThreadData* waiting_thread;
	if (!waiting_threads.pop (waiting_thread))
	{
		std::promise<ThreadData*> threadDataPromise;
		waiting_thread = new ThreadData { .Thread = std::thread (&BlockingThreadPoolImpl::Worker, this, threadDataPromise.get_future ()) };
		threadDataPromise.set_value (waiting_thread);
		{
			std::unique_lock lock (threads_mutex);
			threads.insert (waiting_thread);
		}
	}

	std::unique_lock lock (waiting_thread->Mutex);
	waiting_thread->CurrentTask = task;
	waiting_thread->WakeupReason = ThreadWakeupReason::TaskReady;
	waiting_thread->ConditionVariable.notify_one ();
}

void InfinityOfficialNetwork::MediaServer::Native::Core::Shared::BlockingThreadPool::BlockingThreadPoolImpl::Worker (std::future<ThreadData*> thisThreadPromise) noexcept
{
	thisThreadPromise.wait ();
	ThreadData* thisThread = thisThreadPromise.get ();

	while (true)
	{
		boost::unique_lock lock (thisThread->Mutex);
		if (thisThread->ConditionVariable.wait_for (lock, boost::chrono::milliseconds (timeout_ms),
													[thisThread]() { return thisThread->WakeupReason != ThreadWakeupReason::Spurrious; }))
		{//wakeup signaled
			if (thisThread->WakeupReason == ThreadWakeupReason::TaskReady)
			{
				(*thisThread->CurrentTask)();
				delete thisThread->CurrentTask;
				thisThread->CurrentTask = nullptr;


				boost::unique_lock lock (thisThread->Mutex);
				thisThread->WakeupReason = ThreadWakeupReason::Spurrious;
				waiting_threads.push (thisThread);
			}
			else if (thisThread->WakeupReason == ThreadWakeupReason::ThreadShutdownRequested)
				break;
		}
		else
		{//timed out, pop another thread from wait stack and signal shutdown
			ThreadData* threadToStop = nullptr;
			if (waiting_threads.pop (threadToStop))
			{
				if (threadToStop == thisThread)
					break;

				{
					std::unique_lock lock (threadToStop->Mutex);
					threadToStop->WakeupReason = ThreadWakeupReason::ThreadShutdownRequested;
					threadToStop->ConditionVariable.notify_one ();
				}

				std::unique_lock lock (thisThread->Mutex);
				thisThread->WakeupReason = ThreadWakeupReason::Spurrious;
				waiting_threads.push (thisThread);
			}
		}
	}

	{
		std::unique_lock lock (threads_mutex);
		threads.erase (thisThread);

		thisThread->Thread.detach ();
		delete thisThread;
	}
}
