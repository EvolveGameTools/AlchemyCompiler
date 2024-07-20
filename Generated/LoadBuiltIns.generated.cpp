#include "../Src/Compiler2/LoadBuiltIns.h"

namespace Alchemy::Compilation {

void LoadBuiltIns(PodList<SourceFileInfo*> * files, PoolAllocator<SourceFileInfo>* fileAllocator) {
files->Add(MakeBuiltInFile(fileAllocator, "builtin:array.wyx", R"xyz(
namespace BuiltIn;

public sealed class Array<T> {

    // public int size {
    //     get |v| => return v;
    //     private set |v, newValue| => v = newValue;
    // }

}
)xyz"));
files->Add(MakeBuiltInFile(fileAllocator, "builtin:bool.wyx", R"xyz(
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
files->Add(MakeBuiltInFile(fileAllocator, "builtin:byte.wyx", R"xyz(
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
files->Add(MakeBuiltInFile(fileAllocator, "builtin:char.wyx", R"xyz(
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
files->Add(MakeBuiltInFile(fileAllocator, "builtin:double.wyx", R"xyz(
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
files->Add(MakeBuiltInFile(fileAllocator, "builtin:extern.wyx", R"xyz(
namespace BuiltIn;

// public extern void Print(string value);
// public delegate void Action();
// public delegate void Action<T0>(T0 value);
// public delegate void Action<T0, T1>(T0 value, T1 value);)xyz"));
files->Add(MakeBuiltInFile(fileAllocator, "builtin:float.wyx", R"xyz(
namespace BuiltIn;

public struct Float {

    private float value;

    string ToString() {
        return "0";
    }

})xyz"));
files->Add(MakeBuiltInFile(fileAllocator, "builtin:int.wyx", R"xyz(
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
files->Add(MakeBuiltInFile(fileAllocator, "builtin:long.wyx", R"xyz(
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
files->Add(MakeBuiltInFile(fileAllocator, "builtin:object.wyx", R"xyz(
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
files->Add(MakeBuiltInFile(fileAllocator, "builtin:sbyte.wyx", R"xyz(
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
files->Add(MakeBuiltInFile(fileAllocator, "builtin:short.wyx", R"xyz(
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
files->Add(MakeBuiltInFile(fileAllocator, "builtin:string.wyx", R"xyz(
namespace BuiltIn;

public struct String {

    private string value;

    string ToString() {
        return "0";
    }

}
)xyz"));
files->Add(MakeBuiltInFile(fileAllocator, "builtin:uint.wyx", R"xyz(
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
files->Add(MakeBuiltInFile(fileAllocator, "builtin:ulong.wyx", R"xyz(
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
files->Add(MakeBuiltInFile(fileAllocator, "builtin:ushort.wyx", R"xyz(
namespace BuiltIn;

public struct UInt16 {

    private ushort value;

    public override int GetHashCode() {
        return value;
    }

    public override string ToString() {
        return "";
    }

})xyz"));
files->Add(MakeBuiltInFile(fileAllocator, "builtin:void.wyx", R"xyz(
namespace BuiltIn;

public struct Void {})xyz"));
 
}
}