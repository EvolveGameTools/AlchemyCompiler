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
        explicit JobSystem(int32 workerCount);

        static void WorkerLoop(Worker * worker);

        template<class T>
        void Execute(ParallelParams parallelParams, const T & job) {

            static_assert(std::is_base_of<IJob, T>::value);

            Worker * mainThreadWorker = workers[workers.size - 1];

            JobHandle handle = mainThreadWorker->Schedule(parallelParams, job);

            for (int32 i = 0; i < workers.size; i++) {
                workers[i]->workInSystem = true;
            }

            mainThreadWorker->Await(handle);

            for(int32 i = 0; i < workers.size; i++) {
                workers[i]->Reset();
                workers[i]->workInSystem = false;
            }

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

        void Shutdown();

    };

}