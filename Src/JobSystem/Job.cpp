#include "./Job.h"
#include "./Worker.h"

namespace Alchemy::Jobs {

    ParallelParams Alchemy::Jobs::Parallel::Foreach(int32 size, int32 batchSize) {
        return ParallelParams(JobType::Foreach, size, batchSize);
    }

    ParallelParams Parallel::Single() {
        return ParallelParams(JobType::Single, 1, 1);
    }

    ParallelParams Parallel::Batch(int32 size, int32 batchSize) {
        return ParallelParams(JobType::ForeachBatched, size, batchSize);
    }

    // --------- IJobBase -----------
    void IJobBase::Execute() {
        assert(false && "shouldn't call base Execute()");
    }

    void IJobBase::Execute(int32 index) {
        assert(false && "shouldn't call base Execute(int32 index)");
    }

    void IJobBase::Execute(int32 startIndex, int32 endIndex) {
        for (int32 i = startIndex; i < endIndex; i++) {
            Execute(i);
        }
    }

    bool IJobBase::Active() {
        return state == State::Running || state == State::Scheduled;
    }

    void Worker::Reset() {
        assert(scheduledJobs.size == 0);
        jobAllocator.Clear();
        allocator.Clear();
    }

    bool Worker::IsPrimary() {
        return workerId == workerList.size - 1;
    }

    bool Worker::TryGetJob(IJobBase** retn) {

        {
            if (jobQueueMtx.try_lock()) {
                if (jobQueue.TryDequeue(retn)) {
                    jobQueueMtx.unlock();
                    return true;
                }
                jobQueueMtx.unlock();
            }

        }

        // try to steal some work, iterate worker list from workerId + 1 so each worker tries to steal from its neighbor
        for (int32 workerIndex = workerId + 1; workerIndex < workerList.size; workerIndex++) {
            if (workerList[workerIndex]->TryStealJob(retn)) {
                return true;
            }
        }

        for (int32 workerIndex = 0; workerIndex < workerId; workerIndex++) {
            if (workerList[workerIndex]->TryStealJob(retn)) {
                return true;
            }
        }

        return false;
    }

    bool Worker::TryStealJob(IJobBase** job) {
        if (jobQueueMtx.try_lock()) {
            bool result = jobQueue.TryDequeue(job);
            jobQueueMtx.unlock();
            return result;
        }

        return false;

    }

    void Worker::JobLoop() {

        for (int32 i = 0; i < 10; i++) {

            IJobBase* job;

            if (TryGetJob(&job)) {

                assert(job != nullptr);
                assert(job->state == IJobBase::State::Scheduled);
                int32 scheduleThreshold = scheduledJobs.size;

                job->worker = this;
                job->state = IJobBase::State::Running;

                TempAllocator::Marker m = allocator.Mark();

                switch (job->jobType) {

                    case JobType::Single: {
                        job->Execute();
                        break;
                    }

                    case JobType::Foreach: {

                        for (int32 x = job->start; x < job->end; x++) {
                            job->Execute(x);
                        }

                        break;
                    }

                    case JobType::ForeachBatched: {
                        job->Execute(job->start, job->end);
                        break;
                    }

                }

                int32 scheduleEnd = scheduledJobs.size;
                allocator.RollbackTo(m);

                while (true) {

                    // A job isn't done until all jobs it spawned are done
                    bool hasPendingJobs = false;
                    for (int32 s = scheduleThreshold; s < scheduleEnd; s++) {
                        if (scheduledJobs[s]->state != IJobBase::State::Completed) {
                            hasPendingJobs = true;
                            break;
                        }
                    }

                    if (hasPendingJobs) {
                        JobLoop();
                    }
                    else {
                        break;
                    }

                }

                job->state = IJobBase::State::Completed;

                scheduledJobs.size = scheduleThreshold;

                return;

            }

            std::this_thread::yield();
        }

        if (!IsPrimary()) {
            // make 10 attempts to get a job, then sleep if we still don't have one, only workers should do this
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void Worker::Await(JobHandle handle) {

        while (handle.job->Active()) {
            JobLoop();
        }

    }

    void Worker::Await(JobHandle job1, JobHandle job2) {

        while (job1.job->Active() && job2.job->Active()) {
            JobLoop();
        }

    }

    void Worker::AwaitAll(int32 cnt, JobHandle* jobs) {
        while (true) {
            bool hasActive = false;

            for (int32 i = 0; i < cnt; i++) {
                if (jobs[i].job->Active()) {
                    hasActive = true;
                    break;
                }
            }
            if (hasActive) {
                JobLoop();
            }
            else {
                break;
            }
        }
    }

    void Worker::Await(JobHandle job1, JobHandle job2, JobHandle job3) {

        while (job1.job->Active() && job2.job->Active() && job3.job->Active()) {
            JobLoop();
        }

    }

    void Worker::WorkerLoop() {

        while (!shuttingDown) {
            // if no active work, block
            if (!workInSystem) {
                std::unique_lock lock(waitForWorkMtx);
                waitForWorkCV.wait(lock, [&] { return shuttingDown || workInSystem; });

                if (shuttingDown) {
                    return;
                }

            }

            JobLoop();
        }
    }

    int32 Worker::CalculateBatches(int32 count, int32 batchSize) {
        int32 batches = count / batchSize;
        if (count % batchSize != 0) {
            batches++;
        }
        return batches;
    }

    void Worker::JobContainer::Execute() {

        int32 size = jobs.size;
        JobHandle* array = jobs.array;

        while (size > 0) {

            for (int32 i = 0; i < size; i++) {

                if (!array[i].job->Active()) {
                    array[i] = array[size - 1];
                    size--;
                    i--;
                }

            }

            jobs = CheckedArray<JobHandle>(array, size);

            if (size == 0) {
                break;
            }

            worker->JobLoop();

        }

    }

    int32 IJob::GetWorkerCount() const {
        return worker->workerList.size;
    }

    int32 IJob::GetWorkerId() const {
        return worker->workerId;
    }

    TempAllocator* IJob::GetAllocator() {
        return &worker->allocator;
    }
}

