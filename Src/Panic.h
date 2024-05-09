#pragma once

namespace Alchemy {

    enum class PanicType {
        MemoryCorruption,
        InvalidFree,
        PartialFree,
        IndexOutOfBounds,
        NotImplemented,
        NotSupported,
        Unreachable
    };

    void Panic(PanicType panicType, void * payload);

}