#include "../Src/Compiler2/LoadBuiltIns.h"

namespace Alchemy::Compilation {

void LoadBuiltIns(PodList<SourceFileInfo*> * files, PoolAllocator<SourceFileInfo>* fileAllocator) {
files->Add(MakeBuiltInFile(fileAllocator, "bool", R"xyz(
namespace BuiltIn;

public struct Bool {

    private bool value;

    public override int GetHashCode() {
        return value;
    }

    public override string ToString() {
        return "";
    }

})xyz"));
files->Add(MakeBuiltInFile(fileAllocator, "byte", R"xyz(
namespace BuiltIn;

public struct UInt8 {

    private byte value;

    public override int GetHashCode() {
        return value;
    }

    public override string ToString() {
        return "";
    }

})xyz"));
files->Add(MakeBuiltInFile(fileAllocator, "char", R"xyz(
namespace BuiltIn;

public struct Char {

    private char value;

    public override int GetHashCode() {
        return value;
    }

    public override string ToString() {
        return "";
    }

})xyz"));
files->Add(MakeBuiltInFile(fileAllocator, "double", R"xyz(
namespace BuiltIn;

public struct Double {

    private double value;

    public override int GetHashCode() {
        return value;
    }

    public override string ToString() {
        return "";
    }

})xyz"));
files->Add(MakeBuiltInFile(fileAllocator, "float", R"xyz(
namespace BuiltIn;

public struct Float {

    private float value;

    string ToString() {
        return "0";
    }

})xyz"));
files->Add(MakeBuiltInFile(fileAllocator, "int", R"xyz(
namespace BuiltIn;

public struct Int32 {

    private int value;

    public override int GetHashCode() {
        return value;
    }

    public override string ToString() {
        return "";
    }

})xyz"));
files->Add(MakeBuiltInFile(fileAllocator, "long", R"xyz(
namespace BuiltIn;

public struct Int64 {

    private long value;

    public override int GetHashCode() {
        return value;
    }

    public override string ToString() {
        return "";
    }

})xyz"));
files->Add(MakeBuiltInFile(fileAllocator, "object", R"xyz(
namespace BuiltIn;

public class Object {

    private object value;

    public override int GetHashCode() {
        return value;
    }

    public override string ToString() {
        return "";
    }

})xyz"));
files->Add(MakeBuiltInFile(fileAllocator, "sbyte", R"xyz(
namespace BuiltIn;

public struct Int8 {

    private sbyte value;

    public override int GetHashCode() {
        return value;
    }

    public override string ToString() {
        return "";
    }

})xyz"));
files->Add(MakeBuiltInFile(fileAllocator, "short", R"xyz(
namespace BuiltIn;

public struct Int16 {

    private short value;

    public override int GetHashCode() {
        return value;
    }

    public override string ToString() {
        return "";
    }

})xyz"));
files->Add(MakeBuiltInFile(fileAllocator, "string", R"xyz(
namespace BuiltIn;

public struct String {

    private string value;

    string ToString() {
        return "0";
    }

}
)xyz"));
files->Add(MakeBuiltInFile(fileAllocator, "uint", R"xyz(
namespace BuiltIn;

public struct UInt32 {

    private uint value;

    public override int GetHashCode() {
        return value;
    }

    public override string ToString() {
        return "";
    }

})xyz"));
files->Add(MakeBuiltInFile(fileAllocator, "ulong", R"xyz(
namespace BuiltIn;

public struct UInt64 {

    private ulong value;

    public override int GetHashCode() {
        return value;
    }

    public override string ToString() {
        return "";
    }

})xyz"));
files->Add(MakeBuiltInFile(fileAllocator, "ushort", R"xyz(
namespace BuiltIn;

public struct UInt16 {

    private uint16 value;

    public override int GetHashCode() {
        return value;
    }

    public override string ToString() {
        return "";
    }

})xyz"));
 
}
}