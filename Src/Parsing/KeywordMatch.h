#pragma once

#include "../PrimitiveTypes.h"

bool Matches2(const char* keyword, char* test) {
    return *(uint16*)keyword == *(uint16*)test;
}

bool Matches3(const char* keyword, char* test) {
    return *(uint16*)keyword == *(uint16*)test && keyword[2] == test[2];
}

bool Matches4(const char* keyword, char* test) {
    return *(uint32*)keyword == *(uint32*)test;
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
    return *(uint64*)keyword == *(uint64*)test;
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
    return Matches8(keyword, test) && Matches8(keyword + 8, test + 8) ;
}

bool MatchesGT16(const char* keyword, char* test, int32 length) {

    if(!Matches16(keyword, test)) {
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
