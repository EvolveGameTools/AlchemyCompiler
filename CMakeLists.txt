cmake_minimum_required(VERSION 3.26)
project(AlchemyCompiler)

set(CMAKE_CXX_STANDARD 17)

function(include_builtin_sources input_dir output_file)

    # Get the list of files in the directory
    file(GLOB files "${input_dir}/*")

    # Initialize an empty string to accumulate the contents
    set(all_contents "#include \"../Src/Compiler2/LoadBuiltIns.h\"\n\nnamespace Alchemy::Compilation {\n\nvoid LoadBuiltIns(PodList<SourceFileInfo*> * files, PoolAllocator<SourceFileInfo>* fileAllocator) {\n")

    # Loop through each file in the directory
    foreach(file ${files})

        # Get the file name without the directory
        get_filename_component(filename ${file} NAME)
        # Get the file name without the extension
        get_filename_component(name ${filename} NAME_WE)
        # Read the file content
        file(READ ${file} content)
        # Set the delimiter
        set(delim "xyz")

        # Create the C string
        set(file_content "files->Add(MakeBuiltInFile(fileAllocator, \"${name}\", R\"${delim}(\n${content})${delim}\"));\n")
        # Append to the accumulated content
        set(all_contents "${all_contents}${file_content}")

    endforeach()

    # Write the accumulated content to the output file
    file(WRITE ${output_file} "${all_contents} \n}\n}")
endfunction(include_builtin_sources)

include_builtin_sources(System/ Generated/LoadBuiltIns.generated.cpp)

list(APPEND Sources
        Src/Unicode/Unicode.cpp
        Src/Collections/LongBoolMap.cpp
        Src/Allocation/PodAllocation.cpp
        Src/Allocation/LinearAllocator.cpp
        Src/Allocation/BytePoolAllocator.cpp
        Src/Allocation/ThreadLocalTemp.cpp

        Src/JobSystem/Job.cpp

        Src/Panic.cpp

        Src/Parsing3/TextWindow.cpp
        Src/Parsing3/Scanning.cpp
        Src/Parsing3/Diagnostics.cpp
        Src/Parsing3/Tokenizer.cpp
        Src/Parsing3/SyntaxFacts.cpp
        Src/Parsing3/FindSkippedTokens.cpp
        Src/Parsing3/Parser.cpp
        Src/Parsing3/Parsing.cpp

        Src/Compiler2/Compiler.cpp
        Src/Compiler2/TypeInfo.cpp
        Src/Compiler2/TypeResolver.cpp
        Src/Compiler2/BuiltInTypeName.cpp
        Src/Compiler2/TypeResolutionMap.cpp
        Src/Compiler2/FullyQualifiedName.cpp
        Src/Compiler2/SourceFileInfo.cpp
        Src/Compiler2/LoadBuiltIns.cpp

        Src/Compiler2/Jobs/ParseFilesJob.cpp
        Src/Compiler2/Jobs/GatherTypeInfo.cpp

        Src/FileSystem/VirtualFileSystem.cpp
        Src/Util/StringTable.cpp
        Src/Util/StringUtil.cpp
        Src/Util/File.cpp

        Generated/FindSkippedTokens.generated.cpp
        Generated/GetFirstToken.generated.cpp
        Generated/MatchKeyword.generated.cpp
        Generated/NodePrinter.generated.cpp
        Generated/SyntaxKind.generated.cpp
        Generated/TokenKind.generated.cpp
        Generated/LoadBuiltIns.generated.cpp
)

add_executable(AlchemyCompiler
#        main.cpp
        ${Sources}
)


include(FetchContent)
FetchContent_Declare(
    cpptrace
    GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
    GIT_TAG        v0.5.2 # <HASH or TAG>
)
FetchContent_MakeAvailable(cpptrace)
target_link_libraries(AlchemyCompiler cpptrace::cpptrace)
# On windows copy cpptrace.dll to the same directory as the executable for your_target
if(WIN32)
    add_custom_command(
        TARGET AlchemyCompiler POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:cpptrace::cpptrace>
        $<TARGET_FILE_DIR:AlchemyCompiler>
    )

endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(AlchemyCompiler PUBLIC ALCHEMY_DEBUG=1 ALCHEMY_MALLOC_DEBUG=1 USE_STACKTRACE=1)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(AlchemyCompiler PRIVATE "-gsplit-dwarf")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(AlchemyCompiler PRIVATE "/DEBUG:FASTLINK")
endif()

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

Include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

add_executable(tests
    Tests/Test2.cpp
    ${Sources}
)

add_custom_command(
    TARGET tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:cpptrace::cpptrace>
    $<TARGET_FILE_DIR:tests>
)

target_compile_definitions(tests PRIVATE ALCHEMY_DEBUG=1 USE_STACKTRACE)

target_link_libraries(tests PRIVATE cpptrace::cpptrace Catch2::Catch2WithMain)