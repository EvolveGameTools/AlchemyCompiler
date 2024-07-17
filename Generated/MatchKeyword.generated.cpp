#include "../Src/PrimitiveTypes.h"
#include "../Src/Parsing3/TokenKind.h"

namespace Alchemy::Compilation {

bool Matches2(const char* keyword, char* test) {
    return *(uint16 * )
    keyword == *(uint16 * )
    test;
}

bool Matches3(const char* keyword, char* test) {
    return *(uint16 * )
    keyword == *(uint16 * )
    test && keyword[2] == test[2];
}

bool Matches4(const char* keyword, char* test) {
    return *(uint32*) keyword == *(uint32*) test;
}

bool Matches5(const char* keyword, char* test) {
    return Matches4(keyword, test) && keyword[4] == test[4];
}

bool Matches6(const char* keyword, char* test) {
    return Matches4(keyword, test) && Matches2(keyword + 4, test + 4);
}

bool Matches7(const char* keyword, char* test) {
    return Matches4(keyword, test) && Matches3(keyword + 4, test + 4);
}

bool Matches8(const char* keyword, char* test) {
    return *(uint64*) keyword == *(uint64*) test;
}

bool Matches9(const char* keyword, char* test) {
    return Matches8(keyword, test) && keyword[8] == test[8];
}

bool Matches10(const char* keyword, char* test) {
    return Matches8(keyword, test) && Matches2(keyword + 8, test + 8);
}

bool Matches11(const char* keyword, char* test) {
    return Matches8(keyword, test) && Matches3(keyword + 8, test + 8);
}

bool Matches12(const char* keyword, char* test) {
    return Matches8(keyword, test) && Matches4(keyword + 8, test + 8);
}

bool Matches13(const char* keyword, char* test) {
    return Matches8(keyword, test) && Matches4(keyword + 8, test + 8) && keyword[12] == test[12];
}

bool Matches14(const char* keyword, char* test) {
    return Matches8(keyword, test) && Matches4(keyword + 8, test + 8) && Matches2(keyword + 12, test + 12);
}

bool Matches15(const char* keyword, char* test) {
    return Matches8(keyword, test) && Matches4(keyword + 8, test + 8) && Matches3(keyword + 12, test + 12);
}

bool Matches16(const char* keyword, char* test) {
    return Matches8(keyword, test) && Matches8(keyword + 8, test + 8);
}

bool MatchesGT16(const char* keyword, char* test, int32 length) {

    if (!Matches16(keyword, test)) {
        return false;
    }

    keyword += 16;
    test += 16;
    length -= 16;

    for (int32 i = 0; i < length; i++) {
        if (keyword[i] != test[i]) {
            return false;
        }
    }

    return true;

}
constexpr int32 kLongestKeywordLength = 11;

constexpr int32 kShortestKeywordLength = 2;

bool TryMatchKeyword_Generated(char * buffer, int32 length, TokenKind * keywordType) {

    if(length < kShortestKeywordLength || length > kLongestKeywordLength) {
        return false;
    }

    // we generated a trie like structure in an offline step. this encodes the first 2 chars into a uint
    // and then matches on the rest of the string after switching over the resulting string length
    char * truncatedBuffer = buffer + 2;
    switch(*(uint16*)buffer) {
        case 24930:
                    if(Matches2("se", truncatedBuffer)) { // BaseKeyword
                        *keywordType = TokenKind::BaseKeyword;
                        return true;
                    }
                    return false;
        case 24931:
            switch(length) {
                case 4: {
                    if(Matches2("se", truncatedBuffer)) { // CaseKeyword
                        *keywordType = TokenKind::CaseKeyword;
                        return true;
                    }
                    return false;
                }
                case 5: {
                    if(Matches3("tch", truncatedBuffer)) { // CatchKeyword
                        *keywordType = TokenKind::CatchKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 24934:
                    if(Matches3("lse", truncatedBuffer)) { // FalseKeyword
                        *keywordType = TokenKind::FalseKeyword;
                        return true;
                    }
                    return false;
        case 24942:
                    if(Matches7("mespace", truncatedBuffer)) { // NamespaceKeyword
                        *keywordType = TokenKind::NamespaceKeyword;
                        return true;
                    }
                    return false;
        case 24944:
                    if(Matches4("rams", truncatedBuffer)) { // ParamsKeyword
                        *keywordType = TokenKind::ParamsKeyword;
                        return true;
                    }
                    return false;
        case 24950:
                    if(truncatedBuffer[0] == 'r') { // VarKeyword
                        *keywordType = TokenKind::VarKeyword;
                        return true;
                    }
                    return false;
        case 25185:
                    if(Matches6("stract", truncatedBuffer)) { // AbstractKeyword
                        *keywordType = TokenKind::AbstractKeyword;
                        return true;
                    }
                    return false;
        case 25199:
                    if(Matches4("ject", truncatedBuffer)) { // ObjectKeyword
                        *keywordType = TokenKind::ObjectKeyword;
                        return true;
                    }
                    return false;
        case 25203:
                    if(Matches3("yte", truncatedBuffer)) { // SByteKeyword
                        *keywordType = TokenKind::SByteKeyword;
                        return true;
                    }
                    return false;
        case 25956:
            switch(length) {
                case 7: {
                    if(Matches5("fault", truncatedBuffer)) { // DefaultKeyword
                        *keywordType = TokenKind::DefaultKeyword;
                        return true;
                    }
                    return false;
                }
                case 8: {
                    if(Matches6("legate", truncatedBuffer)) { // DelegateKeyword
                        *keywordType = TokenKind::DelegateKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 25959:
                    if(truncatedBuffer[0] == 't') { // GetKeyword
                        *keywordType = TokenKind::GetKeyword;
                        return true;
                    }
                    return false;
        case 25966:
                    if(truncatedBuffer[0] == 'w') { // NewKeyword
                        *keywordType = TokenKind::NewKeyword;
                        return true;
                    }
                    return false;
        case 25970:
            switch(length) {
                case 3: {
                    if(truncatedBuffer[0] == 'f') { // RefKeyword
                        *keywordType = TokenKind::RefKeyword;
                        return true;
                    }
                    return false;
                }
                case 6: {
                    if(Matches4("turn", truncatedBuffer)) { // ReturnKeyword
                        *keywordType = TokenKind::ReturnKeyword;
                        return true;
                    }
                    return false;
                }
                case 8: {
                    if(Matches6("adonly", truncatedBuffer)) { // ReadOnlyKeyword
                        *keywordType = TokenKind::ReadOnlyKeyword;
                        return true;
                    }
                    if(Matches6("quired", truncatedBuffer)) { // RequiredKeyword
                        *keywordType = TokenKind::RequiredKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 25971:
            switch(length) {
                case 3: {
                    if(truncatedBuffer[0] == 't') { // SetKeyword
                        *keywordType = TokenKind::SetKeyword;
                        return true;
                    }
                    return false;
                }
                case 6: {
                    if(Matches4("aled", truncatedBuffer)) { // SealedKeyword
                        *keywordType = TokenKind::SealedKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 26217:
                    *keywordType = TokenKind::IfKeyword;
                    return true;                    return false;
        case 26723:
                    if(Matches2("ar", truncatedBuffer)) { // CharKeyword
                        *keywordType = TokenKind::CharKeyword;
                        return true;
                    }
                    return false;
        case 26739:
                    if(Matches3("ort", truncatedBuffer)) { // ShortKeyword
                        *keywordType = TokenKind::ShortKeyword;
                        return true;
                    }
                    return false;
        case 26740:
            switch(length) {
                case 4: {
                    if(Matches2("is", truncatedBuffer)) { // ThisKeyword
                        *keywordType = TokenKind::ThisKeyword;
                        return true;
                    }
                    return false;
                }
                case 5: {
                    if(Matches3("row", truncatedBuffer)) { // ThrowKeyword
                        *keywordType = TokenKind::ThrowKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 26743:
            switch(length) {
                case 4: {
                    if(Matches2("en", truncatedBuffer)) { // WhenKeyword
                        *keywordType = TokenKind::WhenKeyword;
                        return true;
                    }
                    return false;
                }
                case 5: {
                    if(Matches3("ile", truncatedBuffer)) { // WhileKeyword
                        *keywordType = TokenKind::WhileKeyword;
                        return true;
                    }
                    if(Matches3("ere", truncatedBuffer)) { // WhereKeyword
                        *keywordType = TokenKind::WhereKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 26982:
                    if(Matches5("nally", truncatedBuffer)) { // FinallyKeyword
                        *keywordType = TokenKind::FinallyKeyword;
                        return true;
                    }
                    return false;
        case 26997:
                    if(Matches2("nt", truncatedBuffer)) { // UIntKeyword
                        *keywordType = TokenKind::UIntKeyword;
                        return true;
                    }
                    return false;
        case 26998:
                    if(Matches5("rtual", truncatedBuffer)) { // VirtualKeyword
                        *keywordType = TokenKind::VirtualKeyword;
                        return true;
                    }
                    return false;
        case 26999:
                    if(Matches2("th", truncatedBuffer)) { // WithKeyword
                        *keywordType = TokenKind::WithKeyword;
                        return true;
                    }
                    return false;
        case 27001:
                    if(Matches3("eld", truncatedBuffer)) { // YieldKeyword
                        *keywordType = TokenKind::YieldKeyword;
                        return true;
                    }
                    return false;
        case 27747:
                    if(Matches3("ass", truncatedBuffer)) { // ClassKeyword
                        *keywordType = TokenKind::ClassKeyword;
                        return true;
                    }
                    return false;
        case 27749:
                    if(Matches2("se", truncatedBuffer)) { // ElseKeyword
                        *keywordType = TokenKind::ElseKeyword;
                        return true;
                    }
                    if(Matches2("if", truncatedBuffer)) { // ElifKeyword
                        *keywordType = TokenKind::ElifKeyword;
                        return true;
                    }
                    return false;
        case 27750:
                    if(Matches3("oat", truncatedBuffer)) { // FloatKeyword
                        *keywordType = TokenKind::FloatKeyword;
                        return true;
                    }
                    return false;
        case 27765:
                    if(Matches3("ong", truncatedBuffer)) { // ULongKeyword
                        *keywordType = TokenKind::ULongKeyword;
                        return true;
                    }
                    return false;
        case 28009:
                    if(Matches6("plicit", truncatedBuffer)) { // ImplicitKeyword
                        *keywordType = TokenKind::ImplicitKeyword;
                        return true;
                    }
                    return false;
        case 28257:
                    if(truncatedBuffer[0] == 'd') { // AndKeyword
                        *keywordType = TokenKind::AndKeyword;
                        return true;
                    }
                    return false;
        case 28261:
            switch(length) {
                case 4: {
                    if(Matches2("um", truncatedBuffer)) { // EnumKeyword
                        *keywordType = TokenKind::EnumKeyword;
                        return true;
                    }
                    return false;
                }
                case 5: {
                    if(Matches3("dif", truncatedBuffer)) { // EndIfKeyword
                        *keywordType = TokenKind::EndIfKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 28265:
            switch(length) {
                case 2: {
                    *keywordType = TokenKind::InKeyword;
                    return true;                    return false;
                }
                case 3: {
                    if(truncatedBuffer[0] == 't') { // IntKeyword
                        *keywordType = TokenKind::IntKeyword;
                        return true;
                    }
                    return false;
                }
                case 4: {
                    if(Matches2("it", truncatedBuffer)) { // InitKeyword
                        *keywordType = TokenKind::InitKeyword;
                        return true;
                    }
                    return false;
                }
                case 8: {
                    if(Matches6("ternal", truncatedBuffer)) { // InternalKeyword
                        *keywordType = TokenKind::InternalKeyword;
                        return true;
                    }
                    return false;
                }
                case 9: {
                    if(Matches7("terface", truncatedBuffer)) { // InterfaceKeyword
                        *keywordType = TokenKind::InterfaceKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 28514:
                    if(Matches2("ol", truncatedBuffer)) { // BoolKeyword
                        *keywordType = TokenKind::BoolKeyword;
                        return true;
                    }
                    return false;
        case 28515:
            switch(length) {
                case 5: {
                    if(Matches3("nst", truncatedBuffer)) { // ConstKeyword
                        *keywordType = TokenKind::ConstKeyword;
                        return true;
                    }
                    return false;
                }
                case 8: {
                    if(Matches6("ntinue", truncatedBuffer)) { // ContinueKeyword
                        *keywordType = TokenKind::ContinueKeyword;
                        return true;
                    }
                    return false;
                }
                case 11: {
                    if(Matches9("nstructor", truncatedBuffer)) { // ConstructorKeyword
                        *keywordType = TokenKind::ConstructorKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 28516:
            switch(length) {
                case 2: {
                    *keywordType = TokenKind::DoKeyword;
                    return true;                    return false;
                }
                case 6: {
                    if(Matches4("uble", truncatedBuffer)) { // DoubleKeyword
                        *keywordType = TokenKind::DoubleKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 28518:
            switch(length) {
                case 3: {
                    if(truncatedBuffer[0] == 'r') { // ForKeyword
                        *keywordType = TokenKind::ForKeyword;
                        return true;
                    }
                    return false;
                }
                case 7: {
                    if(Matches5("reach", truncatedBuffer)) { // ForEachKeyword
                        *keywordType = TokenKind::ForEachKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 28519:
                    if(Matches2("to", truncatedBuffer)) { // GotoKeyword
                        *keywordType = TokenKind::GotoKeyword;
                        return true;
                    }
                    return false;
        case 28524:
                    if(Matches2("ng", truncatedBuffer)) { // LongKeyword
                        *keywordType = TokenKind::LongKeyword;
                        return true;
                    }
                    if(Matches2("ck", truncatedBuffer)) { // LockKeyword
                        *keywordType = TokenKind::LockKeyword;
                        return true;
                    }
                    return false;
        case 28526:
                    if(truncatedBuffer[0] == 't') { // NotKeyword
                        *keywordType = TokenKind::NotKeyword;
                        return true;
                    }
                    return false;
        case 28534:
                    if(Matches2("id", truncatedBuffer)) { // VoidKeyword
                        *keywordType = TokenKind::VoidKeyword;
                        return true;
                    }
                    return false;
        case 28783:
                    if(Matches6("erator", truncatedBuffer)) { // OperatorKeyword
                        *keywordType = TokenKind::OperatorKeyword;
                        return true;
                    }
                    return false;
        case 29282:
                    if(Matches3("eak", truncatedBuffer)) { // BreakKeyword
                        *keywordType = TokenKind::BreakKeyword;
                        return true;
                    }
                    return false;
        case 29286:
                    if(Matches2("om", truncatedBuffer)) { // FromKeyword
                        *keywordType = TokenKind::FromKeyword;
                        return true;
                    }
                    return false;
        case 29295:
                    *keywordType = TokenKind::OrKeyword;
                    return true;                    return false;
        case 29296:
            switch(length) {
                case 7: {
                    if(Matches5("ivate", truncatedBuffer)) { // PrivateKeyword
                        *keywordType = TokenKind::PrivateKeyword;
                        return true;
                    }
                    return false;
                }
                case 9: {
                    if(Matches7("otected", truncatedBuffer)) { // ProtectedKeyword
                        *keywordType = TokenKind::ProtectedKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 29300:
            switch(length) {
                case 3: {
                    if(truncatedBuffer[0] == 'y') { // TryKeyword
                        *keywordType = TokenKind::TryKeyword;
                        return true;
                    }
                    return false;
                }
                case 4: {
                    if(Matches2("ue", truncatedBuffer)) { // TrueKeyword
                        *keywordType = TokenKind::TrueKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 29537:
                    *keywordType = TokenKind::AsKeyword;
                    return true;                    return false;
        case 29545:
                    *keywordType = TokenKind::IsKeyword;
                    return true;                    return false;
        case 29557:
            switch(length) {
                case 5: {
                    if(Matches3("ing", truncatedBuffer)) { // UsingKeyword
                        *keywordType = TokenKind::UsingKeyword;
                        return true;
                    }
                    return false;
                }
                case 6: {
                    if(Matches4("hort", truncatedBuffer)) { // UShortKeyword
                        *keywordType = TokenKind::UShortKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 29811:
                    if(Matches4("ring", truncatedBuffer)) { // StringKeyword
                        *keywordType = TokenKind::StringKeyword;
                        return true;
                    }
                    if(Matches4("atic", truncatedBuffer)) { // StaticKeyword
                        *keywordType = TokenKind::StaticKeyword;
                        return true;
                    }
                    if(Matches4("ruct", truncatedBuffer)) { // StructKeyword
                        *keywordType = TokenKind::StructKeyword;
                        return true;
                    }
                    return false;
        case 30062:
                    if(Matches2("ll", truncatedBuffer)) { // NullKeyword
                        *keywordType = TokenKind::NullKeyword;
                        return true;
                    }
                    return false;
        case 30063:
                    if(truncatedBuffer[0] == 't') { // OutKeyword
                        *keywordType = TokenKind::OutKeyword;
                        return true;
                    }
                    return false;
        case 30064:
                    if(Matches4("blic", truncatedBuffer)) { // PublicKeyword
                        *keywordType = TokenKind::PublicKeyword;
                        return true;
                    }
                    return false;
        case 30068:
                    if(Matches3("ple", truncatedBuffer)) { // TupleKeyword
                        *keywordType = TokenKind::TupleKeyword;
                        return true;
                    }
                    return false;
        case 30319:
                    if(Matches6("erride", truncatedBuffer)) { // OverrideKeyword
                        *keywordType = TokenKind::OverrideKeyword;
                        return true;
                    }
                    return false;
        case 30579:
                    if(Matches4("itch", truncatedBuffer)) { // SwitchKeyword
                        *keywordType = TokenKind::SwitchKeyword;
                        return true;
                    }
                    return false;
        case 30821:
                    if(Matches4("port", truncatedBuffer)) { // ExportKeyword
                        *keywordType = TokenKind::ExportKeyword;
                        return true;
                    }
                    if(Matches4("tern", truncatedBuffer)) { // ExternKeyword
                        *keywordType = TokenKind::ExternKeyword;
                        return true;
                    }
                    return false;
        case 31074:
                    if(Matches2("te", truncatedBuffer)) { // ByteKeyword
                        *keywordType = TokenKind::ByteKeyword;
                        return true;
                    }
                    return false;
        case 31092:
                    if(Matches4("peof", truncatedBuffer)) { // TypeofKeyword
                        *keywordType = TokenKind::TypeofKeyword;
                        return true;
                    }
                    return false;
    }
    return false;
}

}