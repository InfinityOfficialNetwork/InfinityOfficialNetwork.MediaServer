#pragma once

#include "global.h"

#include <mutex>
#include <condition_variable>

#include <boost/asio.hpp>
#include <boost/thread.hpp>


class BlockingThreadPool
{
	struct TypeErasedFunctionInterface
	{
		virtual void operator()() = 0;
		virtual ~TypeErasedFunctionInterface () {};
	};

	template <typename TFunc>
	struct TypeErasedFunctionImplementation : TypeErasedFunctionInterface
	{
		TFunc func;
		virtual void operator()() override
		{
			func ();
		}

		virtual ~TypeErasedFunctionImplementation () {}

		TypeErasedFunctionImplementation (TFunc func) : func (std::forward<TFunc> (func))
		{}
	};

	class BlockingThreadPoolImpl
	{
		static constexpr int64_t timeout_ms = 10000;

		enum class ThreadWakeupReason : char
		{
			Spurrious,
			TaskReady,
			ThreadShutdownRequested,
		};

		struct ThreadData
		{
			std::thread Thread;
			boost::mutex Mutex;
			boost::condition_variable ConditionVariable;
			TypeErasedFunctionInterface* CurrentTask = nullptr;
			ThreadWakeupReason WakeupReason;
		};

		boost::lockfree::stack<ThreadData*> waiting_threads;
		//std::atomic<int64_t> waiting_threads_count = 0;

		std::shared_mutex threads_mutex;
		std::set<ThreadData*> threads;

	public:
		explicit BlockingThreadPoolImpl () : waiting_threads (std::max (std::thread::hardware_concurrency (), 16u))
		{}

		template <typename TFunc>
		void Submit (TFunc&& func)
		{
			TypeErasedFunctionInterface* task = new TypeErasedFunctionImplementation<TFunc> (std::forward<TFunc> (func));

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

	private:
		void Worker (std::future<ThreadData*> thisThreadPromise) noexcept
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
	};

	inline static BlockingThreadPoolImpl instance = BlockingThreadPoolImpl ();

	template <typename TIoContextPtr, typename TFunc>
	static boost::asio::awaitable<std::invoke_result_t<TFunc>> InvokeBlockingOperationAsyncHelper (TIoContextPtr ioContext, TFunc func)
	{
		boost::asio::use_awaitable_t<> token {};
		return boost::asio::async_initiate<
			boost::asio::use_awaitable_t<>, void (boost::system::error_code, std::invoke_result_t < TFunc>)> (
				[ioc = std::forward<TIoContextPtr> (ioContext), func = std::forward<TFunc> (func)](auto completion_handler)
				{
					auto lambda = [ioc, handler = std::move (completion_handler), func = std::move (func)]() mutable
						{
							std::invoke_result_t<decltype(func)> result = func ();

							boost::asio::post (*ioc, [handler = std::move (handler), result = std::move (result)]() mutable
											   {
												   handler (boost::system::error_code {}, result);
											   });
						};

					instance.Submit (std::move (lambda));
				}, token);
	}

	template <typename TIoContextPtr, typename TFunc>
		requires std::is_void_v<std::invoke_result_t<TFunc>>
	static boost::asio::awaitable<void> InvokeBlockingOperationAsyncHelper (TIoContextPtr ioContext, TFunc func)
	{
		boost::asio::use_awaitable_t<> token {};
		return boost::asio::async_initiate<
			boost::asio::use_awaitable_t<>, void (boost::system::error_code)> (
				[ioc = std::forward<TIoContextPtr> (ioContext), func = std::forward<TFunc> (func)](auto completion_handler)
				{
					auto lambda = [ioc, handler = std::move (completion_handler), func = std::move (func)]() mutable
						{
							func ();

							boost::asio::post (*ioc, [handler = std::move (handler)]() mutable
											   {
												   handler (boost::system::error_code {});
											   });
						};

					instance.Submit (std::move (lambda));
				}, token);
	}

public:
	template <typename TFunc, typename...TArgs>
		requires std::is_invocable_v<TFunc, TArgs...>&& std::is_move_constructible_v<std::invoke_result_t<TFunc, TArgs...>>
	static boost::asio::awaitable<std::invoke_result_t<TFunc, TArgs...>> InvokeBlockingOperationAsync (std::shared_ptr<boost::asio::io_context> ioc, TFunc&& func, TArgs&&... args)
	{
		auto boundFunc = std::bind (std::forward<TFunc> (func), std::forward<TArgs> (args)...);
		return InvokeBlockingOperationAsyncHelper (std::move (ioc), std::move (boundFunc));
	}

	template <auto Func, typename...TArgs>
		requires std::is_invocable_v<decltype(Func), TArgs...>&& std::is_move_constructible_v<std::invoke_result_t<decltype(Func), TArgs...>>
	static boost::asio::awaitable<std::invoke_result_t<decltype(Func), TArgs...>> InvokeBlockingOperationAsync (std::shared_ptr<boost::asio::io_context> ioc, TArgs&&... args)
	{
		auto boundFunc = std::bind (Func, std::forward<TArgs> (args)...);
		return InvokeBlockingOperationAsyncHelper (std::move (ioc), std::move (boundFunc));
	}

	template <auto Func, typename...TArgs>
		requires std::is_invocable_v<decltype(Func), TArgs...>&& std::is_void_v<std::invoke_result_t<decltype(Func), TArgs...>>
	static boost::asio::awaitable<std::invoke_result_t<decltype(Func), TArgs...>> InvokeBlockingOperationAsync (std::shared_ptr<boost::asio::io_context> ioc, TArgs&&... args)
	{
		auto boundFunc = std::bind (Func, std::forward<TArgs> (args)...);
		return InvokeBlockingOperationAsyncHelper (std::move (ioc), std::move (boundFunc));
	}
};