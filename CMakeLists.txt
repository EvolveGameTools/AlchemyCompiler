cmake_minimum_required(VERSION 3.26)
project(AlchemyCompiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(AlchemyCompiler
#        main.cpp
        Src/Unicode/Unicode.cpp
        Src/Allocation/PodAllocation.cpp
        Src/Collections/LongBoolMap.cpp
        Src/Allocation/LinearAllocator.cpp
#        Src/Util/Writer.cpp
#        Src/JobSystem/Job.cpp
    Src/Parsing/impl/AlchemyParsing.cpp
    Src/Parsing/impl/TokenType.cpp
    Src/Parsing/impl/Keyword.cpp
    Src/Parsing/impl/ParseResult.cpp
    Src/Parsing/impl/BuiltInTypeName.cpp
    Src/Parsing/impl/NodeType.cpp
    Src/Parsing/impl/Nodes.cpp
)

#-gsplit-dwarf (gcc/clang) /DEBUG:FASTLINK (MSVC)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Enable Link Time Optimization for GCC or Clang
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message("Enabling LTO")
        target_compile_options(AlchemyCompiler PUBLIC -flto)
        set_target_properties(AlchemyCompiler PROPERTIES LINK_FLAGS "-flto")
    # Enable Link Time Optimization for MSVC
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        message("Enabling LTO")
        target_compile_options(AlchemyCompiler PUBLIC /GL)
        set_target_properties(AlchemyCompiler PROPERTIES LINK_FLAGS "/LTCG")
    else()
        message("LTO is not enabled")
    endif()

endif()