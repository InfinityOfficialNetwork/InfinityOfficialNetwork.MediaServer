#pragma once

#include "global.h"
#include <new>

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