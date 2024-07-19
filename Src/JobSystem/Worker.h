#pragma once

#include <mutex>
#include <thread>

#include "./Job.h"
#include "../Allocation/PagedAllocator.h"
#include "../Collections/PodQueue.h"
#include "../Allocation/LinearAllocator.h"

namespace Alchemy::Jobs {

    using namespace Alchemy;

    struct Worker {

        PodQueue<IJobBase*> jobQueue;
        Alchemy::PagedAllocator<uint8> jobAllocator;
        int32 workerId;
        bool shuttingDown;
        bool workInSystem;

        CheckedArray<Worker*> workerList;
        PodList<IJobBase*> scheduledJobs;

        std::condition_variable& waitForWorkCV;
        std::mutex& waitForWorkMtx;
        std::mutex jobQueueMtx;

        TempAllocator allocator;

        Worker(int32 workerId, CheckedArray<Worker*> workerList, std::mutex& workMutex, std::condition_variable& waitForWorkCV)
            : workerId(workerId)
            , workerList(workerList)
            , waitForWorkMtx(workMutex)
            , waitForWorkCV(waitForWorkCV)
            , jobAllocator(64 * 1024)
            , shuttingDown(false)
            , workInSystem(false)
            , scheduledJobs(128)
            , allocator(1024ll * 1024ll * 1024ll * 8ll, 32 * 1024) {}

        void Reset();

        bool IsPrimary();

        bool TryGetJob(IJobBase** retn);

        bool TryStealJob(IJobBase** job);

        void JobLoop();

        void Await(JobHandle handle);

        void Await(JobHandle job1, JobHandle job2);

        void AwaitAll(int32 cnt, JobHandle* jobs);

        void Await(JobHandle job1, JobHandle job2, JobHandle job3);

        void WorkerLoop();

        template<class T>
        IJobBase* AllocateJob(T* inst) {
            static_assert(std::is_base_of<IJobBase, T>::value);
            T* instance = (T*) jobAllocator.AllocateUncleared(sizeof(T));
            memcpy((void*) instance, (void*) inst, sizeof(T));
            return (IJobBase*) instance;

        }

        struct EmptyJob : IJobBase {};

        struct JobContainer : IJobBase {

            CheckedArray<JobHandle> jobs;

            explicit JobContainer(CheckedArray<JobHandle> jobs) : jobs(jobs) {}

            void Execute() override;

        };

        static int32 CalculateBatches(int32 count, int32 batchSize);

        template<class T>
        JobHandle Schedule(T& jobBase) {
            return Schedule(Parallel::Single(), jobBase);
        }

        template<class T>
        void Await(ParallelParams parallel, T& jobInstance) {
            static_assert(std::is_base_of<IJobBase, T>::value);
            IJobBase* pBase = (IJobBase*) &jobInstance;
            assert(pBase->state == IJobBase::State::Invalid);

            Await(Schedule(parallel, jobInstance));

        }

        template<class T>
        void Await(T& jobInstance) {
            static_assert(std::is_base_of<IJobBase, T>::value);
            IJobBase* pBase = reinterpret_cast<IJobBase*>(jobInstance);
            assert(pBase->state == IJobBase::State::Invalid);

            Await(Schedule(Parallel::Single(), jobInstance));

        }

        template<class T>
        JobHandle Schedule(ParallelParams parallel, T& jobBase) {
            static_assert(std::is_base_of<IJobBase, T>::value);

            switch (parallel.type) {
                case JobType::Single: {
                    IJobBase* job = AllocateJob(&jobBase);

                    job->worker = nullptr;
                    job->state = IJobBase::State::Scheduled;
                    job->jobType = JobType::Single;
                    job->start = 0;
                    job->end = 1;

                    std::lock_guard lock(jobQueueMtx);
                    jobQueue.Enqueue(job);
                    return JobHandle(job);
                }
                case JobType::ForeachBatched:
                case JobType::Foreach: {

                    int32 itemCount = parallel.itemCount;
                    int32 batchSize = parallel.batchSize;

                    if (batchSize < 0) batchSize = 1;
                    if (batchSize > itemCount) batchSize = itemCount;

                    if(batchSize == 0 || itemCount == 0) {
                        EmptyJob jobContainer;
                        IJobBase* container = AllocateJob(&jobContainer);
                        container->worker = nullptr;
                        container->state = IJobBase::State::Completed;
                        container->jobType = JobType::Single;
                        container->start = 0;
                        container->end = 0;
                        return JobHandle(container);
                    }

                    int32 batchCount = CalculateBatches(itemCount, batchSize);

                    JobHandle* jobPtrArray = (JobHandle*) jobAllocator.AllocateUncleared((int32) sizeof(JobHandle) * batchCount);

                    CheckedArray<JobHandle> jobArray(jobPtrArray, batchCount);

                    for (int32 batchIndex = 0; batchIndex < batchCount; batchIndex++) {

                        IJobBase* job = AllocateJob(&jobBase);
                        job->worker = nullptr;
                        job->state = IJobBase::State::Scheduled;
                        job->jobType = parallel.type;
                        job->start = batchIndex * batchSize;
                        job->end = job->start + batchSize;
                        if (job->end > itemCount) {
                            job->end = itemCount;
                        }

                        jobArray[batchIndex] = JobHandle(job);

                    }

                    JobContainer jobContainer(jobArray);
                    IJobBase* container = AllocateJob(&jobContainer);
                    container->worker = nullptr;
                    container->state = IJobBase::State::Scheduled;
                    container->jobType = JobType::Single;
                    container->start = 0;
                    container->end = 0;

                    {

                        std::lock_guard lock(jobQueueMtx);
                        for (int32 i = 0; i < jobArray.size; i++) {
                            jobQueue.Enqueue(jobArray[i].job);
                        }
                        jobQueue.Enqueue(container);

                    }

                    return JobHandle(container);

                }

            }

        }

        friend class IJob;
    };


    struct IJob : IJobBase {

        void Await(JobHandle job) {
            worker->Await(job);
        }

        void Await(JobHandle job1, JobHandle job2) {
            worker->Await(job1, job2);
        }

        void Await(JobHandle job1, JobHandle job2, JobHandle job3) {
            worker->Await(job1, job2, job3);
        }

        template<class T>
        void Await(ParallelParams params, const T& jobBase) {
            worker->Await(params, jobBase);
        }

        template<class T>
        void Await(const T& jobBase) {
            worker->Await(Parallel::Single(), jobBase);
        }

        void AwaitAll(int32 cnt, JobHandle * jobs) {
            worker->AwaitAll(cnt, jobs);
        }

        template<class T>
        JobHandle Schedule(ParallelParams params, const T& jobBase) {
            return worker->Schedule(params, jobBase);
        }

        template<class T>
        JobHandle Schedule(const T& jobBase) {
            return worker->Schedule(Parallel::Single(), jobBase);
        }

        template<typename T>
        T* TempAllocate(int32 count) {
            return worker->allocator.Allocate<T>(count);
        }

        template<typename T>
        T* TempAllocateUncleared(int32 count) {
            return worker->allocator.AllocateUncleared<T>(count);
        }

        TempAllocator::Marker TempAllocatorScope() {
            return worker->allocator.Mark();
        }

        void TempAllocatorRollbackTo(TempAllocator::Marker m) {
            worker->allocator.RollbackTo(m);
        }

        TempAllocator* GetAllocator();

        int32 GetWorkerId() const;

        int32 GetWorkerCount() const;

    };

}