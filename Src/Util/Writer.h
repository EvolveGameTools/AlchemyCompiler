#include "../Collections/PodList.h"
#include "../Compiler/TypeInfo.h"
#include "../Compiler/ResolvedType.h"
#include "FixedCharSpan.h"

namespace Alchemy::Compilation {

    struct MethodInfo;

    struct Writer {

        int32 indent;
        PodList<char>* outputBuffer;

        void WriteType(Alchemy::Compilation::ResolvedType resolvedType, bool emitStruct);
        void WriteMethodName(MethodInfo * methodInfo);
        void Write(FixedCharSpan span);
        void Write(const char* str);
        void WriteInt(int32 value);
        void WriteFloat(float value);

    };

}


