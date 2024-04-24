#pragma once
#include "../PrimitiveTypes.h"

namespace Alchemy::Jobs {

    enum class JobType : uint8 {

        Single,
        Foreach,
        ForeachBatched

    };

    struct ParallelParams {

        JobType type {};
        int32 batchSize {};
        int32 itemCount {};

        inline ParallelParams()
                : type(JobType::Single)
                  , batchSize(1)
                  , itemCount(1) {}

        inline ParallelParams(JobType type, int32 itemCount, int32 batchCount)
                : type(type)
                  , itemCount(itemCount)
                  , batchSize(batchCount) {}

    };

    struct Parallel {

        static ParallelParams Foreach(int32 size, int32 batchSize);

        static ParallelParams Single();

        static ParallelParams Batch(int32 size, int32 batchSize);

    };

    struct Worker;

    struct IJobBase {

        enum class State : uint8 {
            Invalid,
            Scheduled,
            Running,
            Completed
        };

        virtual void Execute();

        virtual void Execute(int32 index);

        virtual void Execute(int32 startIndex, int32 endIndex);

        bool Active();

        friend class Worker;
        friend class IJob;

    private:
        Worker* worker {};
        int32 start {};
        int32 end {};
        JobType jobType {};
        State state {State::Invalid}; // maybe pad this out for false sharing

    };

    class JobHandle {

        IJobBase* job;

        explicit JobHandle(IJobBase* job) : job(job) {}

        friend class Worker;

    };

}