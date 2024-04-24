#pragma once

#include "../PrimitiveTypes.h"
#include "./Worker.h"
#include "../Collections/PodList.h"

namespace Alchemy::Jobs {

    class JobSystem {

        Alchemy::PodList<std::thread*> threads;
        Alchemy::PodList<Worker*> workers;
        std::mutex workMutex;
        std::condition_variable workCV;

    public:
        explicit JobSystem(int32 workerCount) {

            workerCount++;

            workers.EnsureCapacity(workerCount);
            workers.size = workerCount;

            for (int32 i = 0; i < workerCount; i++) {
                workers[i] = new Worker(i, workers.ToCheckedArray(), workMutex, workCV);
            }

            for (int32 i = 0; i < workerCount - 1; i++) {
                threads.Add(new std::thread(&WorkerLoop, workers[i]));
            }

        }

        static void WorkerLoop(Worker * worker) {
            worker->WorkerLoop();
        }

        template<class T>
        void Execute(const T & job) {

            static_assert(std::is_base_of<IJob, T>::value);

            Worker * mainThreadWorker = workers[workers.size - 1];

            JobHandle handle = mainThreadWorker->Schedule(job);

            for (int32 i = 0; i < workers.size; i++) {
                workers[i]->workInSystem = true;
            }

            mainThreadWorker->Await(handle);

            for(int32 i = 0; i < workers.size; i++) {
                workers[i]->Reset();
                workers[i]->workInSystem = false;
            }

        }

        void Shutdown() {
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

    };

}