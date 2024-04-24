cmake_minimum_required(VERSION 3.28)
project(EvolveCompiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(EvolveCompiler
    main.cpp
    Src/Unicode/Unicode.cpp
    Src/Allocation/PodAllocation.cpp
    Src/Parsing/AlchemyParsing.cpp
    Src/Collections/LongBoolMap.cpp
    Src/Allocation/LinearAllocator.cpp
        Src/Util/Writer.cpp
        Src/JobSystem/Job.cpp
)
