//
// Created by cris0 on 8/1/2025.
//

#ifndef TEST_H
#define TEST_H
#include <vector>
#include <optional>
#endif //TEST_H

namespace myns {

    //–– Deepest nesting: { "deeper": { "value": "Deep nesting" } }
    struct Deeper {
        std::string value;
    };

    struct DeepObject {
        Deeper deeper;
    };

    //–– The inner “object” field
    struct InnerObject {
        std::string      nestedString;
        std::vector<JsonValue> nestedArray;   // [false,5.5]
        DeepObject       deepObject;
    };

    //–– Top‐level structure
    struct MyData {
        std::string               string_;
        int                       numberInteger;
        double                    numberFloat;
        bool                      booleanTrue;
        bool                      booleanFalse;
        std::optional<JsonValue>  nullValue;    // maps JSON null → std::nullopt
        std::vector<JsonValue>    array;        // mixed‐type array
        InnerObject               object;
        std::vector<JsonValue>    emptyArray;
        std::optional<JsonValue>  emptyObject;  // maps `{}` → JSON object, or nullopt if truly absent
    };

    //–– Bind each JSON key to the corresponding member
    template<typename Binder>
    void deserialize(MyData &d, Binder binder) {
        binder("string",        d.string_);
        binder("numberInteger", d.numberInteger);
        binder("numberFloat",   d.numberFloat);
        binder("booleanTrue",   d.booleanTrue);
        binder("booleanFalse",  d.booleanFalse);
        binder("nullValue",     d.nullValue);
        binder("array",         d.array);
        binder("object",        d.object);
        binder("emptyArray",    d.emptyArray);
        binder("emptyObject",   d.emptyObject);
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
    }

    template<typename Binder>
    void deserialize(Deeper &d, Binder binder) {
        binder("value", d.value);
    }

} // namespace myns