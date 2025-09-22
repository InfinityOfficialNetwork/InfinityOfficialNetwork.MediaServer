// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#pragma warning( push, 0 )

// add headers that you want to pre-compile here

#include <algorithm>
#include <any>
#include <array>
#include <atomic>
#include <barrier>
#include <bit>
#include <bitset>
#include <cassert>
#include <ccomplex>
#include <cctype>
#include <cerrno>
#include <cfenv>
#include <cfloat>
#include <charconv>
#include <chrono>
#include <cinttypes>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <codecvt>
#include <compare>
#include <complex>
#include <concepts>
#include <condition_variable>
#include <coroutine>
#include <csetjmp>
#include <csignal>
#include <cstdalign>
#include <cstdarg>
#include <cstdbool>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctgmath>
#include <ctime>
#include <cuchar>
#include <cwchar>
#include <cwctype>
#include <deque>
#include <exception>
#include <execution>
//#include <expected>
#include <filesystem>
#include <format>
#include <forward_list>
#include <fstream>
#include <functional>
#include <future>
//#include <generator>
//#include <hash_map>
//#include <hash_set>
#include <initializer_list>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <latch>
#include <limits>
#include <list>
#include <locale>
#include <map>
//#include <mdspan>
#include <memory>
#include <memory_resource>
#include <mutex>
#include <new>
#include <numbers>
#include <numeric>
#include <optional>
#include <ostream>
//#include <print>
#include <queue>
#include <random>
#include <ranges>
#include <ratio>
#include <regex>
#include <scoped_allocator>
#include <semaphore>
#include <set>
#include <shared_mutex>
#include <source_location>
#include <span>
//#include <spanstream>
#include <sstream>
#include <stack>
//#include <stacktrace>
#include <stdexcept>
//#include <stdfloat>
#include <stop_token>
#include <streambuf>
#include <string>
#include <string_view>
#include <strstream>
#include <syncstream>
#include <system_error>
#include <thread>
#include <tuple>
#include <typeindex>
#include <typeinfo>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <variant>
#include <vector>
#include <version>


#include <boost/asio.hpp>

#include "Windows.h"

#undef min
#undef max

#include <boost/json.hpp>
#include <boost/filesystem.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/regex.hpp>
#include <boost/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/lockfree/stack.hpp>

#pragma warning(disable: 28252)

//Replaceable allocation functions
void* operator new  (std::size_t count);
void* operator new[] (std::size_t count);
void* operator new  (std::size_t count, std::align_val_t al);
void* operator new[] (std::size_t count, std::align_val_t al);

//Replaceable non - throwing allocation functions
void* operator new  (std::size_t count, const std::nothrow_t& tag) noexcept;
void* operator new[] (std::size_t count, const std::nothrow_t& tag) noexcept;
void* operator new  (std::size_t count, std::align_val_t al, const std::nothrow_t& tag) noexcept;
void* operator new[] (std::size_t count, std::align_val_t al, const std::nothrow_t& tag) noexcept;

void operator delete  (void* ptr) noexcept;
void operator delete[] (void* ptr) noexcept;
void operator delete  (void* ptr, std::align_val_t al) noexcept;
void operator delete[] (void* ptr, std::align_val_t al) noexcept;
void operator delete  (void* ptr, std::size_t sz) noexcept;
void operator delete[] (void* ptr, std::size_t sz) noexcept;
void operator delete  (void* ptr, std::size_t sz, std::align_val_t al) noexcept;
void operator delete[] (void* ptr, std::size_t sz, std::align_val_t al) noexcept;

#pragma warning(default: 28252)

#pragma warning( pop )

#endif //PCH_H
