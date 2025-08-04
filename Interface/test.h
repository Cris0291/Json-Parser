//
// Created by cris0 on 8/1/2025.
//

#ifndef TEST_H
#define TEST_H
#include <vector>
#include <optional>
#include <string>
#endif //TEST_H

namespace example {

  // ── meta.author ──────────────────────────────────────────────────────────────
  struct Author {
    std::string                   name;
    std::string                   email;
    std::vector<std::string>      roles;
  };
  template<typename B>
  void deserialize(Author &a, B bind) {
    bind("name",  a.name);
    bind("email", a.email);
    bind("roles", a.roles);
  }

  // ── meta.flags.features ─────────────────────────────────────────────────────
  struct Features {
    bool                      async;
    bool                      streaming;
    std::string               compression;
    std::vector<std::string>  locales;
  };
  template<typename B>
  void deserialize(Features &f, B bind) {
    bind("async",       f.async);
    bind("streaming",   f.streaming);
    bind("compression", f.compression);
    bind("locales",     f.locales);
  }

  // ── meta.flags ───────────────────────────────────────────────────────────────
  struct Flags {
    bool                        beta;
    std::optional<bool>         deprecated;
    Features                    features;
  };
  template<typename B>
  void deserialize(Flags &f, B bind) {
    bind("beta",       f.beta);
    bind("deprecated", f.deprecated);
    bind("features",   f.features);
  }

  // ── meta ────────────────────────────────────────────────────────────────────
  struct Meta {
    std::string   generated;
    std::string   version;
    Author        author;
    Flags         flags;
  };
  template<typename B>
  void deserialize(Meta &m, B bind) {
    bind("generated", m.generated);
    bind("version",   m.version);
    bind("author",    m.author);
    bind("flags",     m.flags);
  }

  // ── data.users.profile.preferences ───────────────────────────────────────────
  struct Notifications {
    std::optional<bool>       email;
    std::optional<bool>       sms;
    std::vector<std::string>  push;
  };
  template<typename B>
  void deserialize(Notifications &n, B bind) {
    bind("email", n.email);
    bind("sms",   n.sms);
    bind("push",  n.push);
  }

  struct Theme {
    std::string              mode;
    std::optional<std::string> accentColor;
  };
  template<typename B>
  void deserialize(Theme &t, B bind) {
    bind("mode",        t.mode);
    bind("accentColor", t.accentColor);
  }

  struct Preferences {
    Notifications  notifications;
    Theme          theme;
  };
  template<typename B>
  void deserialize(Preferences &p, B bind) {
    bind("notifications", p.notifications);
    bind("theme",         p.theme);
  }

  struct Profile {
    std::string        fullName;
    std::string        birthdate;
    Preferences        preferences;
  };
  template<typename B>
  void deserialize(Profile &p, B bind) {
    bind("fullName",    p.fullName);
    bind("birthdate",   p.birthdate);
    bind("preferences", p.preferences);
  }

  struct Activity {
    std::string             type;
    std::optional<std::string> page;
    std::string             timestamp;
  };
  template<typename B>
  void deserialize(Activity &a, B bind) {
    bind("type",      a.type);
    bind("page",      a.page);
    bind("timestamp", a.timestamp);
  }

  struct Session {
    std::string             sessionId;
    std::string             start;
    std::optional<std::string> end;
    std::vector<Activity>     activities;
  };
  template<typename B>
  void deserialize(Session &s, B bind) {
    bind("sessionId", s.sessionId);
    bind("start",     s.start);
    bind("end",       s.end);
    bind("activities",s.activities);
  }

  struct User {
    int                     id;
    std::string             username;
    Profile                 profile;
    std::vector<Session>      sessions;
  };
  template<typename B>
  void deserialize(User &u, B bind) {
    bind("id",       u.id);
    bind("username", u.username);
    bind("profile",  u.profile);
    bind("sessions", u.sessions);
  }

  // ── data.products ────────────────────────────────────────────────────────────
  struct WarehouseLocation {
    std::string  region;
    int          quantity;
  };
  template<typename B>
  void deserialize(WarehouseLocation &w, B bind) {
    bind("region",   w.region);
    bind("quantity", w.quantity);
  }

  struct Dimensions {
    double  width;
    double  height;
    double  depth;
    std::string unit;
  };
  template<typename B>
  void deserialize(Dimensions &d, B bind) {
    bind("width",  d.width);
    bind("height", d.height);
    bind("depth",  d.depth);
    bind("unit",   d.unit);
  }

  struct Product {
    std::string                     sku;
    std::string                     name;
    double                          price;
    std::vector<std::string>        tags;
    Dimensions                      dimensions;
    std::vector<WarehouseLocation>  warehouseLocations;
  };
  template<typename B>
  void deserialize(Product &p, B bind) {
    bind("sku",                p.sku);
    bind("name",               p.name);
    bind("price",              p.price);
    bind("tags",               p.tags);
    bind("dimensions",         p.dimensions);
    bind("warehouseLocations", p.warehouseLocations);
  }

  // ── logs ─────────────────────────────────────────────────────────────────────
  struct Context {
    std::string  path;
    int          attempt;
  };
  template<typename B>
  void deserialize(Context &c, B bind) {
    bind("path",    c.path);
    bind("attempt", c.attempt);
  }

  struct Log {
    std::string                  level;
    std::vector<std::string>     messages;
    std::string                  timestamp;
    std::optional<Context>       context;
  };
  template<typename B>
  void deserialize(Log &l, B bind) {
    bind("level",    l.level);
    bind("messages", l.messages);
    bind("timestamp",l.timestamp);
    bind("context",  l.context);
  }

  // ── data ─────────────────────────────────────────────────────────────────────
  struct Data {
    std::vector<User>     users;
    std::vector<Product>  products;
  };
  template<typename B>
  void deserialize(Data &d, B bind) {
    bind("users",    d.users);
    bind("products", d.products);
  }

  // ── misc ─────────────────────────────────────────────────────────────────────
  struct Empty_obj {
    int emp;
  };
  template<typename B>
  void deserialize(Empty_obj &d, B bind) {

  }

  struct Misc {
    std::optional<Empty_obj>      emptyObject;
    std::vector<int>        emptyArray;
    std::vector<std::vector<std::vector<int>>>        nestedMixed;
    std::string                   unicodeTest;
    //std::vector<double>           scientific;
  };
  template<typename B>
  void deserialize(Misc &m, B bind) {
    bind("emptyObject", m.emptyObject);
    bind("emptyArray",  m.emptyArray);
    bind("nestedMixed", m.nestedMixed);
    bind("unicodeTest", m.unicodeTest);
    //bind("scientific",  m.scientific);
  }
  // ── root ─────────────────────────────────────────────────────────────────────
  struct Root {
    Meta                    meta;
    Data                    data;
    std::vector<Log>        logs;
    Misc                    misc;
  };
  template<typename B>
  void deserialize(Root &r, B bind) {
    bind("meta", r.meta);
    bind("data", r.data);
    bind("logs", r.logs);
    bind("misc", r.misc);
  }

} // namespace myns