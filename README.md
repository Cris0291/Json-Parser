# LJSON — A tiny, header‑friendly JSON → C++ object mapper

A compact C++23 library that turns JSON files into your own C++ structs/classes without runtime reflection. You describe how your type maps to JSON by providing a single `deserialize(T&, binder)` function in the same namespace as `T` (found via ADL). The library does the rest: tokenize JSON, build an object map, and populate your types.

---

## ✨ Highlights

* **C++23**, **CMake** based
* **Tokenizer**: whitespace‑tolerant lexical analysis (pretty‑printed JSON ok)
* **JsonDescriptor**: builds a `std::unordered_map<std::string, JsonValue>` from tokens
* **JsonValue**: a thin wrapper over `std::variant` (null/int/double/bool/string/object/array)
* **from\_json<T>**: constructs your type `T` using your **ADL** `deserialize` function
* **`std::optional`** support: represent `null` cleanly when desired
* Works on Windows/Mingw, Clang/GCC toolchains

> **No runtime reflection**. You control how fields bind via `deserialize(T&, binder)`. This keeps the library small, predictable, and fast to compile.

---

## 📁 Project structure

```
Json/
├─ CMakeLists.txt              # Top-level
├─ Parse/                      # Static library (core)
│  ├─ CMakeLists.txt
│  ├─ Deserializer/
│  │  ├─ DescriptorState.h
│  │  ├─ JsonDescriptor.h
│  │  ├─ JsonDescriptor.cpp
│  │  ├─ JsonValue.h
│  │  └─ JsonValue.cpp
│  ├─ Include/
│  │  └─ TokenState.h          # Shared token type (no duplication)
│  └─ Tokenizer/
│     ├─ CMakeLists.txt
│     ├─ LookUpTable.h
│     ├─ TokenNode.h
│     ├─ TokenNode.cpp
│     ├─ Tokenizer.h
│     └─ Tokenizer.cpp
└─ Interface/                  # Small example / manual tests
   ├─ CMakeLists.txt
   ├─ main.cpp                 # shows how to use the library
   ├─ test.h                   # example user types (optional)
   └─ test2.h                  # example user types (optional)
```

### What each folder does

* **Tokenizer/** → *Lexical analysis*. Reads JSON text (file path) and produces a `std::vector<State::Token>`.
* **Deserializer/** → *Builds object hierarchy*. Turns tokens into a `JsonDescriptor` and offers `from_json<T>` overloads for leaf/containers/arrays/options.
* **Include/** → *Public shared headers* used by both Tokenizer and Deserializer (e.g., `TokenState.h`).
* **Interface/** → *Example app*. Minimal program showing how to read a JSON file and deserialize into user types. No unit tests yet.

---

## 🛠️ Build

### Requirements

* CMake **≥ 3.30**
* A C++23 compiler (GCC 13+, Clang 16+, MSVC 19.3x)

### Configure & build (out-of-source)

```bash
# From the repo root (Json/)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target Interface -j
```

This builds the static library **Parse** and the example executable **Interface**.

---

## 🚀 Usage (minimal)

1. **Place your JSON file** somewhere (example below uses Windows path):

```json
{
  "meta": {
    "generated": "2025-07-28T15:42:00Z",
    "version": "3.2.1",
    "author": {
      "name": "Jane Doe",
      "email": "jane.doe@example.com",
      "roles": ["developer", "architect", "contributor"]
    },
    "flags": {
      "beta": false,
      "deprecated": null,
      "features": {
        "async": true,
        "streaming": false,
        "compression": "gzip",
        "locales": ["en-US", "es-ES", "zh-Hant-TW"]
      }
    }
  },
  "data": {
    "users": [
      {
        "id": 101,
        "username": "alice_w",
        "profile": {
          "fullName": "Alice Wonderland",
          "birthdate": "1985-12-17",
          "preferences": {
            "notifications": {
              "email": true,
              "sms": false,
              "push": ["news", "updates", "alerts"]
            },
            "theme": {
              "mode": "dark",
              "accentColor": "#ff4081"
            }
          }
        },
        "sessions": [
          {
            "sessionId": "abc123",
            "start": "2025-07-27T22:15:30Z",
            "end": null,
            "activities": [
              { "type": "login","page": null, "timestamp": "2025-07-27T22:15:30Z" },
              { "type": "view", "page": "/dashboard", "timestamp": "2025-07-27T22:16:02Z" },
              { "type": "click", "page": null, "target": "button#refresh", "timestamp": "2025-07-27T22:16:15Z" }
            ]
          }
        ]
      },
      {
        "id": 102,
        "username": "bob_smith",
        "profile": {
          "fullName": "Bob Smith",
          "birthdate": "1990-04-03",
          "preferences": {
            "notifications": {
              "email": null,
              "sms": null,
              "push": []
            },
            "theme": {
              "mode": "light",
              "accentColor": null
            }
          }
        },
        "sessions": []
      }
    ],
    "products": [
      {
        "sku": "XJ-900",
        "name": "Widget Pro™",
        "price": 1299.99,
        "tags": ["new", "sale", "popular"],
        "dimensions": {
          "width": 12.5,
          "height": 7.25,
          "depth": 1.125,
          "unit": "inches"
        },
        "warehouseLocations": [
          { "region": "us-east", "quantity": 124 },
          { "region": "eu-central", "quantity": 98 }
        ]
      },
      {
        "sku": "ZQ-404",
        "name": "Gadget “Ultimate”",
        "price": 0.0,
        "tags": [],
        "dimensions": {
          "width": 0.0,
          "height": 0.0,
          "depth": 0.0,
          "unit": "N/A"
        },
        "warehouseLocations": []
      }
    ]
  },
  "logs": [
    {
      "level": "info",
      "messages": [
        "System initialized",
        "Connected to database",
        "Cache warm-up complete"
      ],
      "timestamp": "2025-07-28T15:40:01Z",
      "context": {
        "path": "/etc/app/config.json",
        "attempt": 1
      }
    },
    {
      "level": "error",
      "messages": [
        "Failed to load configuration: file not found",
        "Using defaults for missing entries"
      ],
      "timestamp": "2025-07-28T15:41:22Z",
      "context": {
        "path": "/etc/app/config.json",
        "attempt": 1
      }
    }
  ],
  "misc": {
    "emptyObject": {},
    "emptyArray": [],
    "nestedMixed": [
      [[1, 2], [3, 4], [5, 6], []],
      [[1, 2], [3, 4], [5, 6], []],
      [[1, 2], [3, 4], [5, 6], []],
      [[1, 2], [3, 4], [5, 6], []]
    ],
    "unicodeTest": "こんにちは世界 🌏\nLine2\tTabbed",
    "scientific": [1e-3, 2.5E+6, -3.14e-2]
  }
}
```

2. **Define your C++ types** and a `deserialize` function (ADL):

```cpp
// example/Model.hpp
#pragma once
#include <string>
#include <vector>
#include <optional>

namespace example {
  struct Author { std::string name, email; std::vector<std::string> roles; };
  struct Features { bool async{}, streaming{}; std::string compression; std::vector<std::string> locales; };
  struct Flags { bool beta{}; std::optional<bool> deprecated; Features features; };
  struct Meta { std::string generated, version; Author author; Flags flags; };

  struct Notifications { std::optional<bool> email, sms; std::vector<std::string> push; };
  struct Theme { std::string mode; std::optional<std::string> accentColor; };
  struct Preferences { Notifications notifications; Theme theme; };
  struct Profile { std::string fullName, birthdate; Preferences preferences; };

  struct Activity { std::string type; std::optional<std::string> page; std::optional<std::string> target; std::string timestamp; };
  struct Session { std::string sessionId, start; std::optional<std::string> end; std::vector<Activity> activities; };
  struct User { int id{}; std::string username; Profile profile; std::vector<Session> sessions; };

  struct Dimensions { double width{}, height{}, depth{}; std::string unit; };
  struct Warehouse { std::string region; int quantity{}; };
  struct Product { std::string sku, name; double price{}; std::vector<std::string> tags; Dimensions dimensions; std::vector<Warehouse> warehouseLocations; };

  struct LogItemCtx { std::string path; int attempt{}; };
  struct LogItem { std::string level; std::vector<std::string> messages; std::string timestamp; std::optional<LogItemCtx> context; };

  // For this example we’ll store nestedMixed as a triple-nested vector of ints
  using NestedMixed = std::vector<std::vector<std::vector<int>>>;

  struct Data { std::vector<User> users; std::vector<Product> products; };
  struct Misc { std::vector<int> emptyArray; NestedMixed nestedMixed; std::string unicodeTest; std::vector<double> scientific; };
  struct Root { Meta meta; Data data; std::vector<LogItem> logs; Misc misc; };
}

// ADL deserialize functions — one binder call per field
namespace example {
  template<class Binder>
  void deserialize(Author& a, Binder b){ b("name", a.name); b("email", a.email); b("roles", a.roles); }
  template<class Binder>
  void deserialize(Features& f, Binder b){ b("async", f.async); b("streaming", f.streaming); b("compression", f.compression); b("locales", f.locales); }
  template<class Binder>
  void deserialize(Flags& f, Binder b){ b("beta", f.beta); b("deprecated", f.deprecated); b("features", f.features); }
  template<class Binder>
  void deserialize(Meta& m, Binder b){ b("generated", m.generated); b("version", m.version); b("author", m.author); b("flags", m.flags); }

  template<class Binder>
  void deserialize(Notifications& n, Binder b){ b("email", n.email); b("sms", n.sms); b("push", n.push); }
  template<class Binder>
  void deserialize(Theme& t, Binder b){ b("mode", t.mode); b("accentColor", t.accentColor); }
  template<class Binder>
  void deserialize(Preferences& p, Binder b){ b("notifications", p.notifications); b("theme", p.theme); }
  template<class Binder>
  void deserialize(Profile& p, Binder b){ b("fullName", p.fullName); b("birthdate", p.birthdate); b("preferences", p.preferences); }

  template<class Binder>
  void deserialize(Activity& a, Binder b){ b("type", a.type); b("page", a.page); b("target", a.target); b("timestamp", a.timestamp); }
  template<class Binder>
  void deserialize(Session& s, Binder b){ b("sessionId", s.sessionId); b("start", s.start); b("end", s.end); b("activities", s.activities); }
  template<class Binder>
  void deserialize(User& u, Binder b){ b("id", u.id); b("username", u.username); b("profile", u.profile); b("sessions", u.sessions); }

  template<class Binder>
  void deserialize(Dimensions& d, Binder b){ b("width", d.width); b("height", d.height); b("depth", d.depth); b("unit", d.unit); }
  template<class Binder>
  void deserialize(Warehouse& w, Binder b){ b("region", w.region); b("quantity", w.quantity); }
  template<class Binder>
  void deserialize(Product& p, Binder b){ b("sku", p.sku); b("name", p.name); b("price", p.price); b("tags", p.tags); b("dimensions", p.dimensions); b("warehouseLocations", p.warehouseLocations); }

  template<class Binder>
  void deserialize(LogItemCtx& c, Binder b){ b("path", c.path); b("attempt", c.attempt); }
  template<class Binder>
  void deserialize(LogItem& l, Binder b){ b("level", l.level); b("messages", l.messages); b("timestamp", l.timestamp); b("context", l.context); }

  template<class Binder>
  void deserialize(Data& d, Binder b){ b("users", d.users); b("products", d.products); }
  template<class Binder>
  void deserialize(Misc& m, Binder b){ b("emptyObject", *static_cast<std::optional<int>*>(nullptr)); /* placeholder if needed */ b("emptyArray", m.emptyArray); b("nestedMixed", m.nestedMixed); b("unicodeTest", m.unicodeTest); b("scientific", m.scientific); }
  template<class Binder>
  void deserialize(Root& r, Binder b){ b("meta", r.meta); b("data", r.data); b("logs", r.logs); b("misc", r.misc); }
}
```

> **Note**: The above demonstrates how ADL `deserialize` looks. Adjust fields/types to your actual needs (e.g., model `emptyObject` as `std::optional<YourType>` or omit it if unused).

3. **Use the library** (as in your example `Interface/main.cpp`):

```cpp
#include <filesystem>
#include "Tokenizer.h"
#include "JsonDescriptor.h"
#include "Model.hpp"           // your types + deserialize

int main(){
  Tokenizer tokenizer{ std::filesystem::path{"C:\\Users\\cris0\\OneDrive\\Escritorio\\test2.json"} };
  auto tokens = tokenizer.Parse();

  JsonDescriptor descriptor{};
  descriptor.createJsonMap(tokens);

  auto root = from_json<example::Root>(descriptor);
  // …print/use `root`
}
```

---

## 🔌 API sketch

* **Tokenizer**

  * `Tokenizer(std::filesystem::path)`
  * `std::vector<State::Token> Parse();`
* **JsonDescriptor**

  * `void createJsonMap(const std::vector<State::Token>& tokens);`
  * `const JsonValue& get(const std::string& key) const;`
* **JsonValue**

  * Holds `std::variant<std::monostate,int,double,bool,std::string,JsonObject,JsonArray>`
  * `template<class T, int idx> T get_value_by_index() const;`
* **from\_json**

  * `T from_json(const JsonDescriptor&)` (root entry for user types)
  * `void from_json(T&, const JsonValue&)` overloads for scalars/containers/arrays/`std::optional`

---

## 📝 Notes & tips

* **Null handling**: If a field may be `null`, model it as `std::optional<...>`. Non‑optional targets will throw on `null`.
* **Containers**: `std::vector`, `std::list`, `std::forward_list`, and `std::array<N>` supported. `std::array<N>` size must match the JSON array length.
* **Strings**: Pretty‑printed JSON (newlines/tabs/spaces) is fine; embedded escapes like `\n` and `\t` are preserved during tokenization.
* **Numbers**: Integers and floating‑point values (including scientific notation like `2.5E+6`).
* **CRLF**: Files with Windows CRLF endings are handled; avoid stray characters *after* the final `}`.

---

## 🧭 Roadmap

* Add unit tests
* Better diagnostics (file/line/column on parse errors)
* More container specializations and nicer error messages for mismatches
* Optional pretty printer / dumper

---

## 📄 License

Choose a license (e.g., MIT) and place it in `LICENSE` at the repo root.

---

## 🙌 Acknowledgements

Thanks for the questions and design guidance that shaped this layout and API.
