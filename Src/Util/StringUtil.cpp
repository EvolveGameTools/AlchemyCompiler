#include "./StringUtil.h"

namespace Alchemy {

    constexpr int32 kBase32PointerLength = 13;

    // output must be at least 13 chars in size
    void EncodePointerBase32(void * ptr, char * output) {
        static const char sBase32Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

        uint64 ptr_val = (uint64)ptr;

        for (int32 i = 0; i < kBase32PointerLength; ++i) { // 64-bit pointer divided into 13 groups of 5 bits for Base32 encoding
            output[i] = sBase32Chars[(ptr_val >> (i * 5)) & 0x1F]; // Extract 5 bits and map to corresponding Base32 character
        }

    }

    int32 IntToAsciiCount(int32 num) {
        int32 i = 0;
        bool isNegative = false;

        // Handle 0 explicitly, otherwise empty string is printed for 0
        if (num == 0) {
            return 1;
        }

        if (num < 0) {
            isNegative = true;
            num = -num;
        }

        // Process individual digits
        while (num != 0) {
            num /= 10;
            i++;
        }

        if (isNegative) {
            i++;
        }

        return i;
    }

    int32 IntToAscii(int32 num, char* str) {
        int32 i = 0;
        bool isNegative = false;

        /* Handle 0 explicitly, otherwise empty string is printed for 0 */
        if (num == 0) {
            str[0] = '0';
            return 1;
        }

        if (num < 0) {
            isNegative = true;
            num = -num;
        }

        // Process individual digits
        while (num != 0) {
            int32 rem = num % 10;
            str[i++] = (char16) ((rem > 9) ? (rem - 10) + 'a' : rem + '0');
            num /= 10;
        }

        // If number is negative, append '-'
        if (isNegative) {
            str[i++] = '-';
        }

        // Reverse the string
        int32 start = 0;
        int32 end = i - 1;
        while (start < end) {
            char* a = (str + start);
            char* b = (str + end);
            char tmp = *a;
            *a = *b;
            *b = tmp;
            start++;
            end--;
        }

        return i;

    }

    size_t CountAndMaybeWriteLower(const char* str, char** buffer, size_t offset) {
        size_t len = strlen(str);
        if (*buffer != nullptr) {
            char * c = *buffer + offset;
            for(int32 i = 0; i < len; i++) {
                if (str[i] >= 'A' && str[i] <= 'Z') {
                    c[i] = str[i] + ('a' - 'A');
                }
                else {
                    c[i] = str[i];
                }
            }
        }
        return len;
    }

    size_t CountAndMaybeWrite(const char* str, char** buffer, size_t offset) {
        size_t len = strlen(str);
        if (*buffer != nullptr) {
            memcpy(*buffer + offset, str, len);
        }
        return len;
    }

}