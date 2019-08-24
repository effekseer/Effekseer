/*  ipc.h - v0.2 - public domain cross platform inter process communication
                   no warranty implied; use at your own risk

    by Jari Komppa, http://iki.fi/sol/

INCLUDE

    Do this:
       #define IPC_IMPLEMENTATION
    before you include this file in *one* C or C++ file to create the implementation.

    // i.e. it should look like this:
    #include ...
    #include ...
    #include ...
    #define IPC_IMPLEMENTATION
    #include "ipc.h"

    You can #define IPC_MALLOC, and IPC_FREE to avoid using malloc,free

LICENSE

    This is free and unencumbered software released into the public domain.

    Anyone is free to copy, modify, publish, use, compile, sell, or
    distribute this software, either in source code form or as a compiled
    binary, for any purpose, commercial or non-commercial, and by any
    means.

    In jurisdictions that recognize copyright laws, the author or authors
    of this software dedicate any and all copyright interest in the
    software to the public domain. We make this dedication for the benefit
    of the public at large and to the detriment of our heirs and
    successors. We intend this dedication to be an overt act of
    relinquishment in perpetuity of all present and future rights to this
    software under copyright law.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

    For more information, please refer to <http://unlicense.org/>

USAGE

    Shared memory API:

    void ipc_mem_init(ipc_sharedmemory *mem, unsigned char *name, size_t size);
    - Initialize ipc_sharedmemory structure for use. Call this first.

    int ipc_mem_open_existing(ipc_sharedmemory *mem);
    - Try to open existing shared memory. Returns 0 for success.

    int ipc_mem_create(ipc_sharedmemory *mem);
    - Try to create shared memory. Returns 0 for success.

    void ipc_mem_close(ipc_sharedmemory *mem);
    - Close shared memory and free allocated stuff. Shared memory will only get
      destroyed when nobody is accessing it. Call this last.

    unsigned char *ipc_mem_access(ipc_sharedmemory *mem);
    - Access the shared memory. Same as poking mem->data directly, but some people
      like accessors.



    Shared semaphore API:

    void ipc_sem_init(ipc_sharedsemaphore *sem, unsigned char *name);
    - Initialize ipc_sharedsemaphore for use. Call this first.

    int ipc_sem_create(ipc_sharedsemaphore *sem, int initialvalue);
    - Try to create semaphore. Returns 0 for success.

    void ipc_sem_close(ipc_sharedsemaphore *sem);
    - Close the semaphore and deallocate. Shared semaphore will only go away after
      nobody is using it. Call this last.

    void ipc_sem_increment(ipc_sharedsemaphore *sem);
    - Increment the semaphore.

    void ipc_sem_decrement(ipc_sharedsemaphore *sem);
    - Decrement the semaphore, waiting forever if need be.

    int ipc_sem_try_decrement(ipc_sharedsemaphore *sem);
    - Try to decrement the semaphore, returns 1 for success, 0 for failure.

TROUBLESHOOTING

    - Thread programming issues apply; don't mess with the memory someone else might
      be reading, make sure you release semaphore after use not to hang someone else,
      etc.
    - If process crashes while holding a semaphore, the others may end up waiting
      forever for them to release.
    - On Linux, named items will remain after your application closes unless you
      close them. This is particularly fun if your application crashes. Having a
      commandline mode that just tries to create and then close all your shared
      resources may be convenient. Saves on rebooting, at least.. On windows, if
      nobody is around to use the resource, they disappear.
*/

#ifndef IPC_H_INCLUDE_GUARD
#define IPC_H_INCLUDE_GUARD

#if defined(_WIN32)
typedef void *HANDLE;
#else
#include <semaphore.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ipc_sharedmemory_
{
    char *name;
    unsigned char *data;
    size_t        size;
#if defined(_WIN32)
    HANDLE        handle;
#else
    int           fd;
#endif
} ipc_sharedmemory;

extern void ipc_mem_init(ipc_sharedmemory *mem, char *name, size_t size);
extern int ipc_mem_open_existing(ipc_sharedmemory *mem);
extern int ipc_mem_create(ipc_sharedmemory *mem);
extern void ipc_mem_close(ipc_sharedmemory *mem);
extern unsigned char *ipc_mem_access(ipc_sharedmemory *mem);

typedef struct ipc_sharedsemaphore_
{
    char *name;
#if defined(_WIN32)
    HANDLE        handle;
#else
    sem_t         *semaphore;
#endif
} ipc_sharedsemaphore;

extern void ipc_sem_init(ipc_sharedsemaphore *sem, char *name);
extern int ipc_sem_create(ipc_sharedsemaphore *sem, int initialvalue);
extern void ipc_sem_close(ipc_sharedsemaphore *sem);
extern void ipc_sem_increment(ipc_sharedsemaphore *sem);
extern void ipc_sem_decrement(ipc_sharedsemaphore *sem);
extern int ipc_sem_try_decrement(ipc_sharedsemaphore *sem);

#ifdef __cplusplus
}
#endif

//////// end of header ////////

#ifdef IPC_IMPLEMENTATION

#if defined(_WIN32)
#include <windows.h>
#else // !_WIN32
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#endif // !_WIN32

#ifndef IPC_ASSERT
#define IPC_ASSERT(x) assert(x)
#endif

#ifndef IPC_MALLOC
#define IPC_MALLOC(x) malloc(x)
#endif

#ifndef IPC_FREE
#define IPC_FREE(x) free(x)
#endif

static char * ipc_strdup(char *src)
{
    int i, len;
    char *dst = NULL;
    len = 0;
    while (src[len]) len++;
#if !defined(_WIN32)
    len++;
#endif
    dst = (char *)IPC_MALLOC(len+1);
    if (!dst) return NULL;
    dst[len] = 0;

#if defined(_WIN32)
    for (i = 0; i < len; i++)
        dst[i] = src[i];
#else
    dst[0] = '/';
    for (i = 0; i < len-1; i++)
        dst[i+1] = src[i];
#endif
    return dst;
}

void ipc_mem_init(ipc_sharedmemory *mem, char *name, size_t size)
{
    mem->name = ipc_strdup(name);

    mem->size = size;
    mem->data = NULL;
#if defined(_WIN32)
    mem->handle = 0;
#else
    mem->fd = -1;
#endif
}

unsigned char *ipc_mem_access(ipc_sharedmemory *mem)
{
    return mem->data;
}

void ipc_sem_init(ipc_sharedsemaphore *sem, char *name)
{
    sem->name = ipc_strdup(name);
#if defined(_WIN32)
    sem->handle = 0;
#else
    sem->semaphore = NULL;
#endif

}


#if defined(_WIN32)

int ipc_mem_open_existing(ipc_sharedmemory *mem)
{
    mem->handle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE,  mem->name);

    if (!mem->handle)
        return -1;

    mem->data = (unsigned char*)MapViewOfFile(mem->handle, FILE_MAP_ALL_ACCESS, 0, 0, mem->size);

    if (!mem->data)
        return -1;
    return 0;
}

int ipc_mem_create(ipc_sharedmemory *mem)
{
    mem->handle = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, mem->size, mem->name);

    if (!mem->handle)
        return -1;

    mem->data = (unsigned char*)MapViewOfFile(mem->handle, FILE_MAP_ALL_ACCESS, 0, 0, mem->size);

    if (!mem->data)
        return -1;

    return 0;

}

void ipc_mem_close(ipc_sharedmemory *mem)
{
    if (mem->data != NULL)
    {
        UnmapViewOfFile(mem->data);
        mem->data = NULL;
    }
    IPC_FREE(mem->name);
    mem->name = NULL;
    mem->size = 0;
}

int ipc_sem_create(ipc_sharedsemaphore *sem, int initialvalue)
{
    sem->handle = CreateSemaphoreA(NULL, initialvalue, 0x7fffffff, sem->name);
    if (!sem->handle)
        return -1;
    return 0;
}

void ipc_sem_close(ipc_sharedsemaphore *sem)
{
    CloseHandle(sem->handle);
    IPC_FREE(sem->name);
    sem->handle = 0;
}

void ipc_sem_increment(ipc_sharedsemaphore *sem)
{
    ReleaseSemaphore(sem->handle, 1, NULL);
}

void ipc_sem_decrement(ipc_sharedsemaphore *sem)
{
    WaitForSingleObject(sem->handle, INFINITE);
}

int ipc_sem_try_decrement(ipc_sharedsemaphore *sem)
{
    DWORD ret = WaitForSingleObject(sem->handle, 0);
    if (ret == WAIT_OBJECT_0)
        return 1;
    return 0;
}

#else // !defined(_WIN32)

int ipc_mem_open_existing(ipc_sharedmemory *mem)
{
    mem->fd = shm_open(mem->name, O_RDWR, 0755);
    if (mem->fd < 0)
        return -1;

    mem->data = (unsigned char *)mmap(NULL, mem->size, PROT_READ | PROT_WRITE, MAP_SHARED, mem->fd, 0);
    if (!mem->data)
        return -1;

    return 0;
}

int ipc_mem_create(ipc_sharedmemory *mem)
{
    int ret;
    ret = shm_unlink(mem->name);
    if (ret < 0 && errno != ENOENT)
        return -1;

    mem->fd = shm_open(mem->name, O_CREAT | O_RDWR, 0755);
    if (mem->fd < 0)
        return -1;

    ftruncate(mem->fd, mem->size);

    mem->data = (unsigned char *)mmap(NULL, mem->size, PROT_READ | PROT_WRITE, MAP_SHARED, mem->fd, 0);
    if (!mem->data)
        return -1;

    return 0;
}

void ipc_mem_close(ipc_sharedmemory *mem)
{
    if (mem->data != NULL)
    {
        munmap(mem->data, mem->size);
        close(mem->fd);
        shm_unlink(mem->name);
    }
    IPC_FREE(mem->name);
    mem->name = NULL;
    mem->size = 0;
}

int ipc_sem_create(ipc_sharedsemaphore *sem, int initialvalue)
{
    sem->semaphore = sem_open(sem->name, O_CREAT, 0700, initialvalue);
    if (sem->semaphore == SEM_FAILED)
        return -1;
    return 0;
}

void ipc_sem_close(ipc_sharedsemaphore *sem)
{
    sem_close(sem->semaphore);
    sem_unlink(sem->name);
    IPC_FREE(sem->name);
}

void ipc_sem_increment(ipc_sharedsemaphore *sem)
{
    sem_post(sem->semaphore);
}

void ipc_sem_decrement(ipc_sharedsemaphore *sem)
{
    sem_wait(sem->semaphore);
}

int ipc_sem_try_decrement(ipc_sharedsemaphore *sem)
{
    int res = sem_trywait(sem->semaphore);
    if (res == 0)
        return 1;
    return 0;
}

#endif // !_WIN32

#endif // IPC_IMPLEMENTATION

#endif // IPC_H_INCLUDE_GUARD