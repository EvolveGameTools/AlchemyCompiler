#pragma once
#include "Keyword.h"
#include "KeywordMatch.h"

constexpr int32 kLongestKeywordLength = 11;

constexpr int32 kShortestKeywordLength = 2;

bool TryMatchKeyword_Generated(char * buffer, int32 length, Keyword * keywordType) {

    if(length < kShortestKeywordLength || length > kLongestKeywordLength) {
        return false;
    }

    // we generated a trie like structure in an offline step. this encodes the first 2 chars into a uint
    // and then matches on the rest of the string after switching over the resulting string length
    char * truncatedBuffer = buffer + 2;
    switch(*(uint16*)buffer) {
        case 24930:
            switch(length) {
                case 4: {
                    if(Matches2("se", truncatedBuffer)) { // Base
                        *keywordType = Keyword::Base;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 24931:
            switch(length) {
                case 4: {
                    if(Matches2("se", truncatedBuffer)) { // Case
                        *keywordType = Keyword::Case;
                        return true;
                    }
                    return false;
                }
                case 5: {
                    if(Matches3("tch", truncatedBuffer)) { // Catch
                        *keywordType = Keyword::Catch;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 24934:
            switch(length) {
                case 5: {
                    if(Matches3("lse", truncatedBuffer)) { // False
                        *keywordType = Keyword::False;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 24942:
            switch(length) {
                case 6: {
                    if(Matches4("meof", truncatedBuffer)) { // NameOf
                        *keywordType = Keyword::NameOf;
                        return true;
                    }
                    return false;
                }
                case 9: {
                    if(Matches7("mespace", truncatedBuffer)) { // Namespace
                        *keywordType = Keyword::Namespace;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 24944:
            switch(length) {
                case 6: {
                    if(Matches4("rams", truncatedBuffer)) { // Params
                        *keywordType = Keyword::Params;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 24950:
            switch(length) {
                case 3: {
                    if(truncatedBuffer[0] == 'r') { // Var
                        *keywordType = Keyword::Var;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 25185:
            switch(length) {
                case 8: {
                    if(Matches6("stract", truncatedBuffer)) { // Abstract
                        *keywordType = Keyword::Abstract;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 25199:
            switch(length) {
                case 6: {
                    if(Matches4("ject", truncatedBuffer)) { // Object
                        *keywordType = Keyword::Object;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 25203:
            switch(length) {
                case 5: {
                    if(Matches3("yte", truncatedBuffer)) { // Sbyte
                        *keywordType = Keyword::Sbyte;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 25459:
            switch(length) {
                case 6: {
                    if(Matches4("oped", truncatedBuffer)) { // Scoped
                        *keywordType = Keyword::Scoped;
                        return true;
                    }
                    return false;
                }
                case 10: {
                    if(Matches8("opealloc", truncatedBuffer)) { // Scopealloc
                        *keywordType = Keyword::Scopealloc;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 25956:
            switch(length) {
                case 7: {
                    if(Matches5("fault", truncatedBuffer)) { // Default
                        *keywordType = Keyword::Default;
                        return true;
                    }
                    return false;
                }
                case 8: {
                    if(Matches6("legate", truncatedBuffer)) { // Delegate
                        *keywordType = Keyword::Delegate;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 25959:
            switch(length) {
                case 3: {
                    if(truncatedBuffer[0] == 't') { // Get
                        *keywordType = Keyword::Get;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 25966:
            switch(length) {
                case 3: {
                    if(truncatedBuffer[0] == 'w') { // New
                        *keywordType = Keyword::New;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 25970:
            switch(length) {
                case 3: {
                    if(truncatedBuffer[0] == 'f') { // Ref
                        *keywordType = Keyword::Ref;
                        return true;
                    }
                    return false;
                }
                case 6: {
                    if(Matches4("turn", truncatedBuffer)) { // Return
                        *keywordType = Keyword::Return;
                        return true;
                    }
                    return false;
                }
                case 8: {
                    if(Matches6("member", truncatedBuffer)) { // Remember
                        *keywordType = Keyword::Remember;
                        return true;
                    }
                    if(Matches6("adonly", truncatedBuffer)) { // Readonly
                        *keywordType = Keyword::Readonly;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 25971:
            switch(length) {
                case 3: {
                    if(truncatedBuffer[0] == 't') { // Set
                        *keywordType = Keyword::Set;
                        return true;
                    }
                    return false;
                }
                case 6: {
                    if(Matches4("aled", truncatedBuffer)) { // Sealed
                        *keywordType = Keyword::Sealed;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 25972:
            switch(length) {
                case 4: {
                    if(Matches2("mp", truncatedBuffer)) { // Temp
                        *keywordType = Keyword::Temp;
                        return true;
                    }
                    return false;
                }
                case 9: {
                    if(Matches7("mpalloc", truncatedBuffer)) { // Tempalloc
                        *keywordType = Keyword::Tempalloc;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 26217:
            switch(length) {
                case 2: {
                    *keywordType = Keyword::If;
                    return true;                }
                default: return false;
            }
        case 26723:
            switch(length) {
                case 4: {
                    if(Matches2("ar", truncatedBuffer)) { // Char
                        *keywordType = Keyword::Char;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 26739:
            switch(length) {
                case 5: {
                    if(Matches3("ort", truncatedBuffer)) { // Short
                        *keywordType = Keyword::Short;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 26740:
            switch(length) {
                case 4: {
                    if(Matches2("is", truncatedBuffer)) { // This
                        *keywordType = Keyword::This;
                        return true;
                    }
                    return false;
                }
                case 5: {
                    if(Matches3("row", truncatedBuffer)) { // Throw
                        *keywordType = Keyword::Throw;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 26743:
            switch(length) {
                case 4: {
                    if(Matches2("en", truncatedBuffer)) { // When
                        *keywordType = Keyword::When;
                        return true;
                    }
                    return false;
                }
                case 5: {
                    if(Matches3("ile", truncatedBuffer)) { // While
                        *keywordType = Keyword::While;
                        return true;
                    }
                    if(Matches3("ere", truncatedBuffer)) { // Where
                        *keywordType = Keyword::Where;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 26982:
            switch(length) {
                case 7: {
                    if(Matches5("nally", truncatedBuffer)) { // Finally
                        *keywordType = Keyword::Finally;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 26995:
            switch(length) {
                case 6: {
                    if(Matches4("zeof", truncatedBuffer)) { // Sizeof
                        *keywordType = Keyword::Sizeof;
                        return true;
                    }
                    if(Matches4("ngle", truncatedBuffer)) { // Single
                        *keywordType = Keyword::Single;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 26997:
            switch(length) {
                case 4: {
                    if(Matches2("nt", truncatedBuffer)) { // Uint
                        *keywordType = Keyword::Uint;
                        return true;
                    }
                    return false;
                }
                case 5: {
                    if(Matches3("nt2", truncatedBuffer)) { // Uint2
                        *keywordType = Keyword::Uint2;
                        return true;
                    }
                    if(Matches3("nt3", truncatedBuffer)) { // Uint3
                        *keywordType = Keyword::Uint3;
                        return true;
                    }
                    if(Matches3("nt4", truncatedBuffer)) { // Uint4
                        *keywordType = Keyword::Uint4;
                        return true;
                    }
                    if(Matches3("nt8", truncatedBuffer)) { // Uint8
                        *keywordType = Keyword::Uint8;
                        return true;
                    }
                    return false;
                }
                case 6: {
                    if(Matches4("nt16", truncatedBuffer)) { // Uint16
                        *keywordType = Keyword::Uint16;
                        return true;
                    }
                    if(Matches4("nt32", truncatedBuffer)) { // Uint32
                        *keywordType = Keyword::Uint32;
                        return true;
                    }
                    if(Matches4("nt64", truncatedBuffer)) { // Uint64
                        *keywordType = Keyword::Uint64;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 26998:
            switch(length) {
                case 7: {
                    if(Matches5("rtual", truncatedBuffer)) { // Virtual
                        *keywordType = Keyword::Virtual;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 26999:
            switch(length) {
                case 4: {
                    if(Matches2("th", truncatedBuffer)) { // With
                        *keywordType = Keyword::With;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 27747:
            switch(length) {
                case 5: {
                    if(Matches3("ass", truncatedBuffer)) { // Class
                        *keywordType = Keyword::Class;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 27749:
            switch(length) {
                case 4: {
                    if(Matches2("se", truncatedBuffer)) { // Else
                        *keywordType = Keyword::Else;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 27750:
            switch(length) {
                case 5: {
                    if(Matches3("oat", truncatedBuffer)) { // Float
                        *keywordType = Keyword::Float;
                        return true;
                    }
                    return false;
                }
                case 6: {
                    if(Matches4("oat2", truncatedBuffer)) { // Float2
                        *keywordType = Keyword::Float2;
                        return true;
                    }
                    if(Matches4("oat3", truncatedBuffer)) { // Float3
                        *keywordType = Keyword::Float3;
                        return true;
                    }
                    if(Matches4("oat4", truncatedBuffer)) { // Float4
                        *keywordType = Keyword::Float4;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 27765:
            switch(length) {
                case 5: {
                    if(Matches3("ong", truncatedBuffer)) { // Ulong
                        *keywordType = Keyword::Ulong;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 28261:
            switch(length) {
                case 4: {
                    if(Matches2("um", truncatedBuffer)) { // Enum
                        *keywordType = Keyword::Enum;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 28265:
            switch(length) {
                case 2: {
                    *keywordType = Keyword::In;
                    return true;                }
                case 3: {
                    if(truncatedBuffer[0] == 't') { // Int
                        *keywordType = Keyword::Int;
                        return true;
                    }
                    return false;
                }
                case 4: {
                    if(Matches2("t2", truncatedBuffer)) { // Int2
                        *keywordType = Keyword::Int2;
                        return true;
                    }
                    if(Matches2("t3", truncatedBuffer)) { // Int3
                        *keywordType = Keyword::Int3;
                        return true;
                    }
                    if(Matches2("t4", truncatedBuffer)) { // Int4
                        *keywordType = Keyword::Int4;
                        return true;
                    }
                    if(Matches2("t8", truncatedBuffer)) { // Int8
                        *keywordType = Keyword::Int8;
                        return true;
                    }
                    return false;
                }
                case 5: {
                    if(Matches3("t16", truncatedBuffer)) { // Int16
                        *keywordType = Keyword::Int16;
                        return true;
                    }
                    if(Matches3("t32", truncatedBuffer)) { // Int32
                        *keywordType = Keyword::Int32;
                        return true;
                    }
                    if(Matches3("t64", truncatedBuffer)) { // Int64
                        *keywordType = Keyword::Int64;
                        return true;
                    }
                    return false;
                }
                case 8: {
                    if(Matches6("ternal", truncatedBuffer)) { // Internal
                        *keywordType = Keyword::Internal;
                        return true;
                    }
                    return false;
                }
                case 9: {
                    if(Matches7("terface", truncatedBuffer)) { // Interface
                        *keywordType = Keyword::Interface;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 28514:
            switch(length) {
                case 4: {
                    if(Matches2("ol", truncatedBuffer)) { // Bool
                        *keywordType = Keyword::Bool;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 28515:
            switch(length) {
                case 5: {
                    if(Matches3("nst", truncatedBuffer)) { // Const
                        *keywordType = Keyword::Const;
                        return true;
                    }
                    if(Matches3("lor", truncatedBuffer)) { // Color
                        *keywordType = Keyword::Color;
                        return true;
                    }
                    return false;
                }
                case 7: {
                    if(Matches5("lor32", truncatedBuffer)) { // Color32
                        *keywordType = Keyword::Color32;
                        return true;
                    }
                    if(Matches5("lor64", truncatedBuffer)) { // Color64
                        *keywordType = Keyword::Color64;
                        return true;
                    }
                    return false;
                }
                case 8: {
                    if(Matches6("ntinue", truncatedBuffer)) { // Continue
                        *keywordType = Keyword::Continue;
                        return true;
                    }
                    return false;
                }
                case 11: {
                    if(Matches9("nstructor", truncatedBuffer)) { // Constructor
                        *keywordType = Keyword::Constructor;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 28516:
            switch(length) {
                case 2: {
                    *keywordType = Keyword::Do;
                    return true;                }
                case 6: {
                    if(Matches4("uble", truncatedBuffer)) { // Double
                        *keywordType = Keyword::Double;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 28518:
            switch(length) {
                case 3: {
                    if(truncatedBuffer[0] == 'r') { // For
                        *keywordType = Keyword::For;
                        return true;
                    }
                    return false;
                }
                case 7: {
                    if(Matches5("reach", truncatedBuffer)) { // Foreach
                        *keywordType = Keyword::Foreach;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 28524:
            switch(length) {
                case 4: {
                    if(Matches2("ng", truncatedBuffer)) { // Long
                        *keywordType = Keyword::Long;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 28534:
            switch(length) {
                case 4: {
                    if(Matches2("id", truncatedBuffer)) { // Void
                        *keywordType = Keyword::Void;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 29282:
            switch(length) {
                case 5: {
                    if(Matches3("eak", truncatedBuffer)) { // Break
                        *keywordType = Keyword::Break;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 29296:
            switch(length) {
                case 7: {
                    if(Matches5("ivate", truncatedBuffer)) { // Private
                        *keywordType = Keyword::Private;
                        return true;
                    }
                    return false;
                }
                case 9: {
                    if(Matches7("otected", truncatedBuffer)) { // Protected
                        *keywordType = Keyword::Protected;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 29300:
            switch(length) {
                case 3: {
                    if(truncatedBuffer[0] == 'y') { // Try
                        *keywordType = Keyword::Try;
                        return true;
                    }
                    return false;
                }
                case 4: {
                    if(Matches2("ue", truncatedBuffer)) { // True
                        *keywordType = Keyword::True;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 29537:
            switch(length) {
                case 2: {
                    *keywordType = Keyword::As;
                    return true;                }
                default: return false;
            }
        case 29545:
            switch(length) {
                case 2: {
                    *keywordType = Keyword::Is;
                    return true;                }
                default: return false;
            }
        case 29557:
            switch(length) {
                case 5: {
                    if(Matches3("ing", truncatedBuffer)) { // Using
                        *keywordType = Keyword::Using;
                        return true;
                    }
                    return false;
                }
                case 6: {
                    if(Matches4("hort", truncatedBuffer)) { // Ushort
                        *keywordType = Keyword::Ushort;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 29811:
            switch(length) {
                case 6: {
                    if(Matches4("atic", truncatedBuffer)) { // Static
                        *keywordType = Keyword::Static;
                        return true;
                    }
                    if(Matches4("ruct", truncatedBuffer)) { // Struct
                        *keywordType = Keyword::Struct;
                        return true;
                    }
                    if(Matches4("ring", truncatedBuffer)) { // String
                        *keywordType = Keyword::String;
                        return true;
                    }
                    return false;
                }
                case 10: {
                    if(Matches8("ackalloc", truncatedBuffer)) { // Stackalloc
                        *keywordType = Keyword::Stackalloc;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 30062:
            switch(length) {
                case 4: {
                    if(Matches2("ll", truncatedBuffer)) { // Null
                        *keywordType = Keyword::Null;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 30063:
            switch(length) {
                case 3: {
                    if(truncatedBuffer[0] == 't') { // Out
                        *keywordType = Keyword::Out;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 30064:
            switch(length) {
                case 6: {
                    if(Matches4("blic", truncatedBuffer)) { // Public
                        *keywordType = Keyword::Public;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 30319:
            switch(length) {
                case 8: {
                    if(Matches6("erride", truncatedBuffer)) { // Override
                        *keywordType = Keyword::Override;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 30579:
            switch(length) {
                case 6: {
                    if(Matches4("itch", truncatedBuffer)) { // Switch
                        *keywordType = Keyword::Switch;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 30821:
            switch(length) {
                case 6: {
                    if(Matches4("tern", truncatedBuffer)) { // Extern
                        *keywordType = Keyword::Extern;
                        return true;
                    }
                    if(Matches4("port", truncatedBuffer)) { // Export
                        *keywordType = Keyword::Export;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 31074:
            switch(length) {
                case 4: {
                    if(Matches2("te", truncatedBuffer)) { // Byte
                        *keywordType = Keyword::Byte;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 31076:
            switch(length) {
                case 7: {
                    if(Matches5("namic", truncatedBuffer)) { // Dynamic
                        *keywordType = Keyword::Dynamic;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 31092:
            switch(length) {
                case 6: {
                    if(Matches4("peof", truncatedBuffer)) { // Typeof
                        *keywordType = Keyword::Typeof;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
    }
    return false;
}
