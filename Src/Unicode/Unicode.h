#pragma once
#include "../PrimitiveTypes.h"

namespace Alchemy::Unicode {

    enum class UnicodeCategory {
        UppercaseLetter = 0,
        LowercaseLetter = 1,
        TitlecaseLetter = 2,
        ModifierLetter = 3,
        OtherLetter = 4,
        NonSpacingMark = 5,
        SpacingCombiningMark = 6,
        EnclosingMark = 7,
        DecimalDigitNumber = 8,
        LetterNumber = 9,
        OtherNumber = 10,
        SpaceSeparator = 11,
        LineSeparator = 12,
        ParagraphSeparator = 13,
        Control = 14,
        Format = 15,
        Surrogate = 16,
        PrivateUse = 17,
        ConnectorPunctuation = 18,
        DashPunctuation = 19,
        OpenPunctuation = 20,
        ClosePunctuation = 21,
        InitialQuotePunctuation = 22,
        FinalQuotePunctuation = 23,
        OtherPunctuation = 24,
        MathSymbol = 25,
        CurrencySymbol = 26,
        ModifierSymbol = 27,
        OtherSymbol = 28,
        OtherNotAssigned = 29,
    };

    extern uint8 CategoryCasingLevel1Index[2176];
    extern uint8 CategoryCasingLevel2Index[6528];
    extern uint8 CategoryCasingLevel3Index[11952];
    extern uint8 CategoriesValues[234];
    extern uint8 Latin1CharInfo[256];

    UnicodeCategory GetUnicodeCategory(uint32 codePoint);

    bool IsLetter(uint32 c);

    bool IsWhiteSpace(uint32 c);

    bool IsPunctuation(uint32 c);

    bool IsLetterOrDigit(uint32 c);

    bool IsNumber(uint32 c);

    bool IsDigit(uint32 c);

    bool IsNewLine(uint32 c);

    bool IsHexDigit(uint32 c);

    bool IsLetterChar(UnicodeCategory cat);

}