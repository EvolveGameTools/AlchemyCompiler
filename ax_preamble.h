
typedef unsigned char			byte;
typedef char16_t                char16;
typedef char32_t                char32;

#if defined(_MSC_VER)

typedef signed char				int8;
typedef short					int16;
typedef int						int32;
typedef __int64					int64;
typedef unsigned char			uint8;
typedef unsigned char			byte;
typedef unsigned short			uint16;
typedef unsigned int			uint32;
typedef unsigned __int64		uint64;

#elif defined(__ORBIS__) || defined(__PROSPERO__)

typedef signed char				int8;
typedef short					int16;
typedef int						int32;
typedef long					int64;
typedef unsigned char			uint8;
typedef unsigned short			uint16;
typedef unsigned int			uint32;
typedef unsigned long			uint64;

#elif defined(__GNUC__)

typedef signed char int8;
typedef short int16;
typedef int int32;
typedef long long int64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

#endif

struct float2 {
    float x;
    float y;
};

struct float3 {
    float x;
    float y;
    float z;
};

struct float4 {
    float x;
    float y;
    float z;
    float w;
};