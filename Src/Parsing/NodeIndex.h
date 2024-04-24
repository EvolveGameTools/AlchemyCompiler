#pragma once

namespace Alchemy {

    struct TokenRange {

        uint16 start;
        uint16 end;

        TokenRange() : start(0), end(0) {}

        TokenRange(uint16 start, uint16 end)
            : start(start)
            , end(end) {}

        bool IsValid() const {
            return start != end;
        }

    };

    struct UntypedNodeIndex {

        uint16 index {};

        UntypedNodeIndex() : index(0) {}

        explicit UntypedNodeIndex(uint16 index) : index(index) {}

        inline bool IsValid() {
            return index != 0;
        }

    };

    template<typename T>
    struct NodeIndex {

        uint16 index {};

        explicit NodeIndex() : index(0) {}

        explicit NodeIndex(uint16 index) : index(index) {}

        inline bool IsValid() {
            return index != 0;
        }

        operator UntypedNodeIndex() const {
            return UntypedNodeIndex(index);
        }

    };


}