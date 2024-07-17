#pragma once

namespace Alchemy::Parsing {

    struct TokenRange {

        uint16 start;
        uint16 end;

        inline TokenRange() : start(0), end(0) {}

        inline TokenRange(uint16 start, uint16 end)
            : start(start)
            , end(end) {}

        inline bool IsValid() const {
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

        inline operator UntypedNodeIndex() const {
            return UntypedNodeIndex(index);
        }

    };


}