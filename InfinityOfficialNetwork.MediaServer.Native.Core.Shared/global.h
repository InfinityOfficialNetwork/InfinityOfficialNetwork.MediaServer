#pragma once

#pragma warning(disable: 4251)

#ifdef __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_SHARED_EXPORTS
#define __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_SHARED_API __declspec(dllexport)
#else
#define __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_SHARED_API __declspec(dllimport)
#endif

#pragma warning(disable: 28252)

#pragma warning( push, 0 )
#include <jemalloc/jemalloc.h>
#pragma warning( pop )

void* operator new(size_t size) {
    if (void* ptr = je_malloc(size))
        return ptr;
    else
        throw std::bad_alloc();
}

void operator delete(void* ptr) noexcept {
    je_free(ptr);
}

void* operator new[](size_t size) {
    if (void* ptr = je_malloc(size))
        return ptr;
    else
        throw std::bad_alloc();
}

void operator delete[](void* ptr) noexcept {
    je_free(ptr);
}

#pragma warning(default: 28252)
