//
// Created by cris0 on 8/2/2025.
//

#ifndef TEST2_H
#define TEST2_H
#include <optional>
#include <vector>
#include <array>
#endif //TEST2_H

namespace ljson {

    struct empty_obj {
        int value;
    };

    struct Deeper {
        std::string value;
    };

    struct DeepObject {
        Deeper deeper;
        int deep;
    };

    struct InnerObject {
        std::string      nestedString;
        std::vector<double> nestedArray;   // [6.5,5.5]
        DeepObject       deepObject;
    };

    struct obj {
        int numberInteger;
        std::string sval;
        double numberFloat;
        bool booleanTrue;
        bool booleanFalse;
        std::optional<int>  nullValue;
        std::array<empty_obj, 7>    array;
        InnerObject               object;
        int**    emptyArray;
        std::optional<empty_obj>  emptyObject;
    };

    template<typename Binder>
    void deserialize(obj &o, Binder binder) {
        binder("numberInteger", o.numberInteger);
        binder("sval",          o.sval);
        binder("numberFloat", o.numberFloat);
        binder("booleanTrue", o.booleanTrue);
        binder("booleanFalse", o.booleanFalse);
        binder("nullValue",     o.nullValue);
        binder("array",         o.array);
        binder("object",        o.object);
        binder("emptyArray",    o.emptyArray);
        binder("emptyObject",   o.emptyObject);
    }

    template<typename Binder>
    void deserialize(InnerObject &o, Binder binder) {
        binder("nestedString", o.nestedString);
        binder("nestedArray",  o.nestedArray);
        binder("deepObject",   o.deepObject);
    }

    template<typename Binder>
    void deserialize(DeepObject &d, Binder binder) {
        binder("deeper", d.deeper);
        binder("deep", d.deep);
    }

    template<typename Binder>
    void deserialize(Deeper &d, Binder binder) {
        binder("value", d.value);
    }

    template<typename Binder>
    void deserialize(empty_obj &d, Binder binder) {
        binder("value", d.value);
    }
}