#include "../PrimitiveTypes.h"
#include "./SyntaxKind.h"

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
constexpr int32 kLongestKeywordLength = 9;

constexpr int32 kShortestKeywordLength = 2;

bool TryMatchKeyword_Generated(char * buffer, int32 length, SyntaxKind * keywordType) {

    if(length < kShortestKeywordLength || length > kLongestKeywordLength) {
        return false;
    }

    // we generated a trie like structure in an offline step. this encodes the first 2 chars into a uint
    // and then matches on the rest of the string after switching over the resulting string length
    char * truncatedBuffer = buffer + 2;
    switch(*(uint16*)buffer) {
        case 24930:
                    if(Matches2("se", truncatedBuffer)) { // BaseKeyword
                        *keywordType = SyntaxKind::BaseKeyword;
                        return true;
                    }
                    return false;
        case 24931:
            switch(length) {
                case 4: {
                    if(Matches2("se", truncatedBuffer)) { // CaseKeyword
                        *keywordType = SyntaxKind::CaseKeyword;
                        return true;
                    }
                    return false;
                }
                case 5: {
                    if(Matches3("tch", truncatedBuffer)) { // CatchKeyword
                        *keywordType = SyntaxKind::CatchKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 24934:
                    if(Matches3("lse", truncatedBuffer)) { // FalseKeyword
                        *keywordType = SyntaxKind::FalseKeyword;
                        return true;
                    }
                    return false;
        case 24942:
            switch(length) {
                case 6: {
                    if(Matches4("meof", truncatedBuffer)) { // NameofKeyword
                        *keywordType = SyntaxKind::NameofKeyword;
                        return true;
                    }
                    return false;
                }
                case 9: {
                    if(Matches7("mespace", truncatedBuffer)) { // NamespaceKeyword
                        *keywordType = SyntaxKind::NamespaceKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 24944:
            switch(length) {
                case 6: {
                    if(Matches4("rams", truncatedBuffer)) { // ParamsKeyword
                        *keywordType = SyntaxKind::ParamsKeyword;
                        return true;
                    }
                    return false;
                }
                case 7: {
                    if(Matches5("rtial", truncatedBuffer)) { // PartialKeyword
                        *keywordType = SyntaxKind::PartialKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 25185:
                    if(Matches6("stract", truncatedBuffer)) { // AbstractKeyword
                        *keywordType = SyntaxKind::AbstractKeyword;
                        return true;
                    }
                    return false;
        case 25199:
                    if(Matches4("ject", truncatedBuffer)) { // ObjectKeyword
                        *keywordType = SyntaxKind::ObjectKeyword;
                        return true;
                    }
                    return false;
        case 25203:
                    if(Matches3("yte", truncatedBuffer)) { // SByteKeyword
                        *keywordType = SyntaxKind::SByteKeyword;
                        return true;
                    }
                    return false;
        case 25956:
            switch(length) {
                case 7: {
                    if(Matches5("fault", truncatedBuffer)) { // DefaultKeyword
                        *keywordType = SyntaxKind::DefaultKeyword;
                        return true;
                    }
                    return false;
                }
                case 8: {
                    if(Matches6("legate", truncatedBuffer)) { // DelegateKeyword
                        *keywordType = SyntaxKind::DelegateKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 25959:
                    if(truncatedBuffer[0] == 't') { // GetKeyword
                        *keywordType = SyntaxKind::GetKeyword;
                        return true;
                    }
                    return false;
        case 25966:
                    if(truncatedBuffer[0] == 'w') { // NewKeyword
                        *keywordType = SyntaxKind::NewKeyword;
                        return true;
                    }
                    return false;
        case 25970:
            switch(length) {
                case 3: {
                    if(truncatedBuffer[0] == 'f') { // RefKeyword
                        *keywordType = SyntaxKind::RefKeyword;
                        return true;
                    }
                    return false;
                }
                case 6: {
                    if(Matches4("turn", truncatedBuffer)) { // ReturnKeyword
                        *keywordType = SyntaxKind::ReturnKeyword;
                        return true;
                    }
                    return false;
                }
                case 8: {
                    if(Matches6("adonly", truncatedBuffer)) { // ReadOnlyKeyword
                        *keywordType = SyntaxKind::ReadOnlyKeyword;
                        return true;
                    }
                    if(Matches6("quired", truncatedBuffer)) { // RequiredKeyword
                        *keywordType = SyntaxKind::RequiredKeyword;
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
                        *keywordType = SyntaxKind::SetKeyword;
                        return true;
                    }
                    return false;
                }
                case 6: {
                    if(Matches4("aled", truncatedBuffer)) { // SealedKeyword
                        *keywordType = SyntaxKind::SealedKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 26217:
                    *keywordType = SyntaxKind::IfKeyword;
                    return true;                    return false;
        case 26723:
                    if(Matches2("ar", truncatedBuffer)) { // CharKeyword
                        *keywordType = SyntaxKind::CharKeyword;
                        return true;
                    }
                    return false;
        case 26739:
                    if(Matches3("ort", truncatedBuffer)) { // ShortKeyword
                        *keywordType = SyntaxKind::ShortKeyword;
                        return true;
                    }
                    return false;
        case 26740:
            switch(length) {
                case 4: {
                    if(Matches2("is", truncatedBuffer)) { // ThisKeyword
                        *keywordType = SyntaxKind::ThisKeyword;
                        return true;
                    }
                    return false;
                }
                case 5: {
                    if(Matches3("row", truncatedBuffer)) { // ThrowKeyword
                        *keywordType = SyntaxKind::ThrowKeyword;
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
                        *keywordType = SyntaxKind::WhenKeyword;
                        return true;
                    }
                    return false;
                }
                case 5: {
                    if(Matches3("ile", truncatedBuffer)) { // WhileKeyword
                        *keywordType = SyntaxKind::WhileKeyword;
                        return true;
                    }
                    if(Matches3("ere", truncatedBuffer)) { // WhereKeyword
                        *keywordType = SyntaxKind::WhereKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 26982:
                    if(Matches5("nally", truncatedBuffer)) { // FinallyKeyword
                        *keywordType = SyntaxKind::FinallyKeyword;
                        return true;
                    }
                    return false;
        case 26995:
                    if(Matches4("zeof", truncatedBuffer)) { // SizeOfKeyword
                        *keywordType = SyntaxKind::SizeOfKeyword;
                        return true;
                    }
                    return false;
        case 26997:
                    if(Matches2("nt", truncatedBuffer)) { // UIntKeyword
                        *keywordType = SyntaxKind::UIntKeyword;
                        return true;
                    }
                    return false;
        case 26998:
                    if(Matches5("rtual", truncatedBuffer)) { // VirtualKeyword
                        *keywordType = SyntaxKind::VirtualKeyword;
                        return true;
                    }
                    return false;
        case 26999:
                    if(Matches2("th", truncatedBuffer)) { // WithKeyword
                        *keywordType = SyntaxKind::WithKeyword;
                        return true;
                    }
                    return false;
        case 27745:
                    if(Matches3("ias", truncatedBuffer)) { // AliasKeyword
                        *keywordType = SyntaxKind::AliasKeyword;
                        return true;
                    }
                    return false;
        case 27747:
                    if(Matches3("ass", truncatedBuffer)) { // ClassKeyword
                        *keywordType = SyntaxKind::ClassKeyword;
                        return true;
                    }
                    return false;
        case 27749:
                    if(Matches2("se", truncatedBuffer)) { // ElseKeyword
                        *keywordType = SyntaxKind::ElseKeyword;
                        return true;
                    }
                    return false;
        case 27750:
                    if(Matches3("oat", truncatedBuffer)) { // FloatKeyword
                        *keywordType = SyntaxKind::FloatKeyword;
                        return true;
                    }
                    return false;
        case 27751:
                    if(Matches4("obal", truncatedBuffer)) { // GlobalKeyword
                        *keywordType = SyntaxKind::GlobalKeyword;
                        return true;
                    }
                    return false;
        case 27765:
                    if(Matches3("ong", truncatedBuffer)) { // ULongKeyword
                        *keywordType = SyntaxKind::ULongKeyword;
                        return true;
                    }
                    return false;
        case 28009:
                    if(Matches6("plicit", truncatedBuffer)) { // ImplicitKeyword
                        *keywordType = SyntaxKind::ImplicitKeyword;
                        return true;
                    }
                    return false;
        case 28257:
                    if(truncatedBuffer[0] == 'd') { // AndKeyword
                        *keywordType = SyntaxKind::AndKeyword;
                        return true;
                    }
                    return false;
        case 28261:
                    if(Matches2("um", truncatedBuffer)) { // EnumKeyword
                        *keywordType = SyntaxKind::EnumKeyword;
                        return true;
                    }
                    return false;
        case 28265:
            switch(length) {
                case 2: {
                    *keywordType = SyntaxKind::InKeyword;
                    return true;                    return false;
                }
                case 3: {
                    if(truncatedBuffer[0] == 't') { // IntKeyword
                        *keywordType = SyntaxKind::IntKeyword;
                        return true;
                    }
                    return false;
                }
                case 4: {
                    if(Matches2("it", truncatedBuffer)) { // InitKeyword
                        *keywordType = SyntaxKind::InitKeyword;
                        return true;
                    }
                    return false;
                }
                case 8: {
                    if(Matches6("ternal", truncatedBuffer)) { // InternalKeyword
                        *keywordType = SyntaxKind::InternalKeyword;
                        return true;
                    }
                    return false;
                }
                case 9: {
                    if(Matches7("terface", truncatedBuffer)) { // InterfaceKeyword
                        *keywordType = SyntaxKind::InterfaceKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 28514:
                    if(Matches2("ol", truncatedBuffer)) { // BoolKeyword
                        *keywordType = SyntaxKind::BoolKeyword;
                        return true;
                    }
                    return false;
        case 28515:
            switch(length) {
                case 5: {
                    if(Matches3("nst", truncatedBuffer)) { // ConstKeyword
                        *keywordType = SyntaxKind::ConstKeyword;
                        return true;
                    }
                    return false;
                }
                case 8: {
                    if(Matches6("ntinue", truncatedBuffer)) { // ContinueKeyword
                        *keywordType = SyntaxKind::ContinueKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 28516:
            switch(length) {
                case 2: {
                    *keywordType = SyntaxKind::DoKeyword;
                    return true;                    return false;
                }
                case 6: {
                    if(Matches4("uble", truncatedBuffer)) { // DoubleKeyword
                        *keywordType = SyntaxKind::DoubleKeyword;
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
                        *keywordType = SyntaxKind::ForKeyword;
                        return true;
                    }
                    return false;
                }
                case 7: {
                    if(Matches5("reach", truncatedBuffer)) { // ForEachKeyword
                        *keywordType = SyntaxKind::ForEachKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 28519:
                    if(Matches2("to", truncatedBuffer)) { // GotoKeyword
                        return true;
                    }
                    return false;
        case 28524:
                    if(Matches2("ng", truncatedBuffer)) { // LongKeyword
                        *keywordType = SyntaxKind::LongKeyword;
                        return true;
                    }
                    return false;
        case 28526:
                    if(truncatedBuffer[0] == 't') { // NotKeyword
                        *keywordType = SyntaxKind::NotKeyword;
                        return true;
                    }
                    return false;
        case 28534:
                    if(Matches2("id", truncatedBuffer)) { // VoidKeyword
                        *keywordType = SyntaxKind::VoidKeyword;
                        return true;
                    }
                    return false;
        case 28783:
                    if(Matches6("erator", truncatedBuffer)) { // OperatorKeyword
                        *keywordType = SyntaxKind::OperatorKeyword;
                        return true;
                    }
                    return false;
        case 29282:
                    if(Matches3("eak", truncatedBuffer)) { // BreakKeyword
                        *keywordType = SyntaxKind::BreakKeyword;
                        return true;
                    }
                    return false;
        case 29286:
                    if(Matches2("om", truncatedBuffer)) { // FromKeyword
                        *keywordType = SyntaxKind::FromKeyword;
                        return true;
                    }
                    return false;
        case 29295:
                    *keywordType = SyntaxKind::OrKeyword;
                    return true;                    return false;
        case 29296:
            switch(length) {
                case 7: {
                    if(Matches5("ivate", truncatedBuffer)) { // PrivateKeyword
                        *keywordType = SyntaxKind::PrivateKeyword;
                        return true;
                    }
                    return false;
                }
                case 9: {
                    if(Matches7("otected", truncatedBuffer)) { // ProtectedKeyword
                        *keywordType = SyntaxKind::ProtectedKeyword;
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
                        *keywordType = SyntaxKind::TryKeyword;
                        return true;
                    }
                    return false;
                }
                case 4: {
                    if(Matches2("ue", truncatedBuffer)) { // TrueKeyword
                        *keywordType = SyntaxKind::TrueKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 29537:
                    *keywordType = SyntaxKind::AsKeyword;
                    return true;                    return false;
        case 29545:
                    *keywordType = SyntaxKind::IsKeyword;
                    return true;                    return false;
        case 29557:
            switch(length) {
                case 5: {
                    if(Matches3("ing", truncatedBuffer)) { // UsingKeyword
                        *keywordType = SyntaxKind::UsingKeyword;
                        return true;
                    }
                    return false;
                }
                case 6: {
                    if(Matches4("hort", truncatedBuffer)) { // UShortKeyword
                        *keywordType = SyntaxKind::UShortKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 29811:
                    if(Matches4("ring", truncatedBuffer)) { // StringKeyword
                        *keywordType = SyntaxKind::StringKeyword;
                        return true;
                    }
                    if(Matches4("atic", truncatedBuffer)) { // StaticKeyword
                        *keywordType = SyntaxKind::StaticKeyword;
                        return true;
                    }
                    if(Matches4("ruct", truncatedBuffer)) { // StructKeyword
                        *keywordType = SyntaxKind::StructKeyword;
                        return true;
                    }
                    return false;
        case 30062:
                    if(Matches2("ll", truncatedBuffer)) { // NullKeyword
                        *keywordType = SyntaxKind::NullKeyword;
                        return true;
                    }
                    return false;
        case 30063:
                    if(truncatedBuffer[0] == 't') { // OutKeyword
                        *keywordType = SyntaxKind::OutKeyword;
                        return true;
                    }
                    return false;
        case 30064:
                    if(Matches4("blic", truncatedBuffer)) { // PublicKeyword
                        *keywordType = SyntaxKind::PublicKeyword;
                        return true;
                    }
                    return false;
        case 30319:
                    if(Matches6("erride", truncatedBuffer)) { // OverrideKeyword
                        *keywordType = SyntaxKind::OverrideKeyword;
                        return true;
                    }
                    return false;
        case 30579:
                    if(Matches4("itch", truncatedBuffer)) { // SwitchKeyword
                        *keywordType = SyntaxKind::SwitchKeyword;
                        return true;
                    }
                    return false;
        case 30821:
            switch(length) {
                case 6: {
                    if(Matches4("tern", truncatedBuffer)) { // ExternKeyword
                        *keywordType = SyntaxKind::ExternKeyword;
                        return true;
                    }
                    return false;
                }
                case 8: {
                    if(Matches6("plicit", truncatedBuffer)) { // ExplicitKeyword
                        *keywordType = SyntaxKind::ExplicitKeyword;
                        return true;
                    }
                    return false;
                }
                default: return false;
            }
        case 31074:
                    if(Matches2("te", truncatedBuffer)) { // ByteKeyword
                        *keywordType = SyntaxKind::ByteKeyword;
                        return true;
                    }
                    return false;
        case 31092:
                    if(Matches4("peof", truncatedBuffer)) { // TypeOfKeyword
                        *keywordType = SyntaxKind::TypeOfKeyword;
                        return true;
                    }
                    return false;
    }
    return false;
}

}