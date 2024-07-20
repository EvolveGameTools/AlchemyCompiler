#include "./MemberInfo.h"
#include "../Util/StringUtil.h"

namespace Alchemy::Compilation {

    const char* MemberVisibilityToString(MemberVisibility visibility) {
        switch (visibility) {
            case MemberVisibility::Public:
                return "public";
            case MemberVisibility::Private:
                return "private";
            case MemberVisibility::Internal:
                return "internal";
            case MemberVisibility::Protected:
                return "protected";
            case MemberVisibility::Export:
                return "export";
        }
        UNREACHABLE("MemberVisibilityToString");
        return nullptr;
    }

    size_t MethodModifiersToString(MethodModifiers flags, char * buffer) {
        PrintFlagVars(buffer);

        // PrintFlagZero(MethodModifiers, None)
        PrintFlagLower(MethodModifiers, Sealed)
        PrintFlagLower(MethodModifiers, Abstract)
        PrintFlagLower(MethodModifiers, Virtual)
        PrintFlagLower(MethodModifiers, Override)
        PrintFlagLower(MethodModifiers, Static)

        return PrintFlagLength;
    }

    size_t ParameterModifiersToString(ParameterModifiers flags, char * buffer) {

        PrintFlagVars(buffer);

        // PrintFlagZero(ParameterModifiers, None);

        PrintFlagLower(ParameterModifiers, Out)
        PrintFlagLower(ParameterModifiers, Readonly)
        PrintFlagLower(ParameterModifiers, Ref)
        PrintFlagLower(ParameterModifiers, Temp)

        return PrintFlagLength;
    }

    size_t FieldModifiersToString(FieldModifiers flags, char* buffer) {
        PrintFlagVars(buffer);

        PrintFlagZero(FieldModifiers, None);
        PrintFlagLower(FieldModifiers, Readonly)
        PrintFlagLower(FieldModifiers, Static)
        PrintFlagLower(FieldModifiers, Const)

        return PrintFlagLength;
    }
}