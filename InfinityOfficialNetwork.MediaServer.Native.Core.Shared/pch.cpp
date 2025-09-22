// pch.cpp: source file corresponding to the pre-compiled header

#include "pch.h"

#pragma warning( push, 0 )
#include <jemalloc/jemalloc.h>
#pragma warning( pop )

// When you are using pre-compiled headers, this source file is necessary for compilation to succeed.

void* operator new  (std::size_t count)
{
    if (void* ptr = je_malloc (count))
        return ptr;
    else
        throw std::bad_alloc ();
}
void* operator new[] (std::size_t count)
{
    if (void* ptr = je_malloc (count))
        return ptr;
    else
        throw std::bad_alloc ();
}
void* operator new  (std::size_t count, std::align_val_t al)
{
    if (void* ptr = je_aligned_alloc ((size_t) al, count))
        return ptr;
    else
        throw std::bad_alloc ();
}
void* operator new[] (std::size_t count, std::align_val_t al)
{
    if (void* ptr = je_aligned_alloc ((size_t) al, count))
        return ptr;
    else
        throw std::bad_alloc ();
}

//Replaceable non - throwing allocation functions
void* operator new  (std::size_t count, const std::nothrow_t& tag) noexcept
{
    return je_malloc (count);
}
void* operator new[] (std::size_t count, const std::nothrow_t& tag) noexcept
{
    return je_malloc (count);
}
void* operator new  (std::size_t count, std::align_val_t al,
                     const std::nothrow_t& tag) noexcept
{
    return je_aligned_alloc ((size_t) al, count);
}
void* operator new[] (std::size_t count, std::align_val_t al,
                      const std::nothrow_t& tag) noexcept
{
    return je_aligned_alloc ((size_t) al, count);
}

void operator delete  (void* ptr) noexcept
{
    je_free (ptr);
}
void operator delete[] (void* ptr) noexcept
{
    je_free (ptr);
}
void operator delete  (void* ptr, std::align_val_t al) noexcept
{
    je_free (ptr);
}
void operator delete[] (void* ptr, std::align_val_t al) noexcept
{
    je_free (ptr);
}
void operator delete  (void* ptr, std::size_t sz) noexcept
{
    je_free (ptr);
}
void operator delete[] (void* ptr, std::size_t sz) noexcept
{
    je_free (ptr);
}
void operator delete  (void* ptr, std::size_t sz,
                       std::align_val_t al) noexcept
{
    je_free (ptr);
}
void operator delete[] (void* ptr, std::size_t sz,
                        std::align_val_t al) noexcept
{
    je_free (ptr);
}