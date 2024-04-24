
namespace Alchemy::Parsing {

    DEFINE_ENUM_FLAGS(TokenFlags, uint8, {
        None = 0,
        FollowedByWhitespaceOrComment = 1 << 0,
        InvalidMatch = 1 << 1
    });

}