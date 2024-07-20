#include "./JobSystem.h"
#include "../Util/StringUtil.h"

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <processthreadsapi.h>

#elif defined(__linux__)
#include <pthread.h>
#elif defined(__APPLE__) && defined(__MACH__)
#include <pthread.h>
#endif

void SetThreadName(std::thread* thread, const char* name) {
#if defined(_WIN32) || defined(_WIN64)
    SetThreadDescription(thread->native_handle(), std::wstring(name, name + strlen(name)).c_str());
#elif defined(__linux__) || defined(__APPLE__) && defined(__MACH__)
    pthread_setname_np(thread->native_handle(), name);
#endif
}

Alchemy::Jobs::JobSystem::JobSystem(int32 workerCount) {

    uint32 threadMax = std::thread::hardware_concurrency();

    workerCount++;

    if (workerCount >= threadMax) {
        workerCount = (int32) (threadMax) - 1;
    }

    if (workerCount > 32) {
        workerCount = 32;
    }

    workers.EnsureCapacity(workerCount);
    workers.size = workerCount;

    for (int32 i = 0; i < workerCount; i++) {
        workers[i] = new Worker(i, workers.ToCheckedArray(), workMutex, workCV);
    }

    char buffer[32];
    char* c = buffer;
    memcpy(c, "Worker[", 7);
    c += 7;
    for (int32 i = 0; i < workerCount - 1; i++) {
        threads.Add(new std::thread(&WorkerLoop, workers[i]));
        char* p = c;
        p += IntToAscii(i, p);
        p[0] = ']';
        p++;
        p[0] = '\0';
        SetThreadName(threads[i], p);
    }

}

void Alchemy::Jobs::JobSystem::Shutdown() {
    for (int32 i = 0; i < workers.size; i++) {
        workers[i]->shuttingDown = true;
    }

    for (int i = 0; i < threads.size; i++) {
        threads[i]->join();
        delete threads[i];
    }

    for (int32 i = 0; i < workers.size; i++) {
        delete workers[i];
    }

}

void Alchemy::Jobs::JobSystem::WorkerLoop(Alchemy::Jobs::Worker* worker) {
    worker->WorkerLoop();
}
