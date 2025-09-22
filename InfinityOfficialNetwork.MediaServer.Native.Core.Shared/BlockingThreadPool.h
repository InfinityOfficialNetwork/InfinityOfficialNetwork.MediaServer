#pragma once

#include "global.h"

#include <mutex>
#include <condition_variable>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <thread>
#include <algorithm>
#include <cstdint>
#include <future>
#include <memory>
#include <set>
#include <shared_mutex>
#include <type_traits>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/chrono/duration.hpp>
#include <boost/lockfree/stack.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>


namespace InfinityOfficialNetwork::MediaServer::Native::Core::Shared
{
	class __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_SHARED_API BlockingThreadPool
	{
		struct __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_SHARED_API TypeErasedFunctionInterface
		{
			virtual void operator()() = 0;
			virtual ~TypeErasedFunctionInterface () {};
		};

		template <typename TFunc>
		struct __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_SHARED_API TypeErasedFunctionImplementation : TypeErasedFunctionInterface
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

		class __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_SHARED_API BlockingThreadPoolImpl
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
			explicit BlockingThreadPoolImpl ();

			void Submit (TypeErasedFunctionInterface* task);

		private:
			void Worker (std::future<ThreadData*> thisThreadPromise) noexcept;
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

						TypeErasedFunctionInterface* task = new TypeErasedFunctionImplementation<TFunc> (std::move (lambda));
						instance.Submit (task);
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

						TypeErasedFunctionInterface* task = new TypeErasedFunctionImplementation<TFunc> (std::move (lambda));
						instance.Submit (task);
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
}
