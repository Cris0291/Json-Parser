#include <iostream>

#include "test.h"
#include "Tokenizer.h"

void indent(int level) {
  for (int i = 0; i < level; ++i) std::cout << "  ";
}

void print_string_vec(std::vector<std::string> const &v, int indentLevel) {
  std::cout << "[";
  for (size_t i = 0; i < v.size(); ++i) {
    std::cout << '"' << v[i] << '"';
    if (i + 1 < v.size()) std::cout << ", ";
  }
  std::cout << "]";
}
void print_vec1d(const std::vector<int> &v, int indentLevel) {
  std::cout << "[";
  for (size_t i = 0; i < v.size(); ++i) {
    std::cout << v[i];
    if (i + 1 < v.size()) std::cout << ", ";
  }
  std::cout << "]";
}

void print_vec2d(const std::vector<std::vector<int>> &v, int indentLevel) {
  std::cout << "[\n";
  for (auto const &inner : v) {
    indent(indentLevel + 1);
    print_vec1d(inner, indentLevel + 1);
    std::cout << ",\n";
  }
  indent(indentLevel);
  std::cout << "]";
}

void print_vec3d(const std::vector<std::vector<std::vector<int>>> &v, int indentLevel) {
  std::cout << "[\n";
  for (auto const &matrix : v) {
    indent(indentLevel + 1);
    print_vec2d(matrix, indentLevel + 1);
    std::cout << ",\n";
  }
  indent(indentLevel);
  std::cout << "]";
}

int main() {
    std::cout << "Please enter some text in json format" << std::endl;

    Tokenizer tokenizer {std::filesystem::path{"C:\\Users\\cris0\\OneDrive\\Escritorio\\test2.json"}};
    auto tokens = tokenizer.Parse();
    JsonDescriptor descriptor {};
    descriptor.createJsonMap(tokens);

     auto root = from_json<example::Root>(descriptor);

     std::cout << "meta:\n";
    indent(1); std::cout << "generated: "  << root.meta.generated << "\n";
    indent(1); std::cout << "version:   "  << root.meta.version   << "\n";
    indent(1); std::cout << "author:\n";
      indent(2); std::cout << "name:  "  << root.meta.author.name  << "\n";
      indent(2); std::cout << "email: "  << root.meta.author.email << "\n";
      indent(2); std::cout << "roles: ";
        print_string_vec(root.meta.author.roles, 2);
        std::cout << "\n";
    indent(1); std::cout << "flags:\n";
      indent(2); std::cout << "beta:       "
                           << (root.meta.flags.beta ? "true" : "false")
                           << "\n";
      indent(2); std::cout << "deprecated: ";
        if (root.meta.flags.deprecated.has_value())
          std::cout << (*root.meta.flags.deprecated ? "true" : "false");
        else
          std::cout << "null";
        std::cout << "\n";
      indent(2); std::cout << "features:\n";
        indent(3); std::cout << "async:      "
                             << (root.meta.flags.features.async ? "true" : "false")
                             << "\n";
        indent(3); std::cout << "streaming:  "
                             << (root.meta.flags.features.streaming ? "true" : "false")
                             << "\n";
        indent(3); std::cout << "compression: "
                             << root.meta.flags.features.compression
                             << "\n";
        indent(3); std::cout << "locales:    ";
          print_string_vec(root.meta.flags.features.locales, 3);
          std::cout << "\n";

    // 4) Print data.users
    std::cout << "\ndata.users:\n";
    for (auto const &u : root.data.users) {
      indent(1); std::cout << "user:\n";
        indent(2); std::cout << "id:       " << u.id       << "\n";
        indent(2); std::cout << "username: " << u.username << "\n";
        indent(2); std::cout << "profile:\n";
          indent(3); std::cout << "fullName:  " << u.profile.fullName << "\n";
          indent(3); std::cout << "birthdate: " << u.profile.birthdate << "\n";
          indent(3); std::cout << "preferences:\n";
            indent(4); std::cout << "notifications:\n";
              indent(5); std::cout << "email: ";
                if (u.profile.preferences.notifications.email.has_value())
                  std::cout << (*u.profile.preferences.notifications.email? "true":"false");
                else std::cout << "null";
                std::cout << "\n";
              indent(5); std::cout << "sms:   ";
                if (u.profile.preferences.notifications.sms.has_value())
                  std::cout << (*u.profile.preferences.notifications.sms? "true":"false");
                else std::cout << "null";
                std::cout << "\n";
              indent(5); std::cout << "push: ";
                print_string_vec(u.profile.preferences.notifications.push, 5);
                std::cout << "\n";
            indent(4); std::cout << "theme:\n";
              indent(5); std::cout << "mode:        "
                                     << u.profile.preferences.theme.mode
                                     << "\n";
              indent(5); std::cout << "accentColor: ";
                if (u.profile.preferences.theme.accentColor.has_value())
                  std::cout << *u.profile.preferences.theme.accentColor;
                else std::cout << "null";
                std::cout << "\n";
        indent(2); std::cout << "sessions:\n";
        for (auto const &s : u.sessions) {
          indent(3); std::cout << "session:\n";
            indent(4); std::cout << "sessionId: " << s.sessionId << "\n";
            indent(4); std::cout << "start:     " << s.start     << "\n";
            indent(4); std::cout << "end:       ";
              if (s.end.has_value()) std::cout << *s.end; else std::cout << "null";
              std::cout << "\n";
            indent(4); std::cout << "activities:\n";
            for (auto const &a: s.activities) {
              indent(5); std::cout << "type:      " << a.type      << "\n";
              indent(5); std::cout << "page:      ";
                if (a.page.has_value()) std::cout << *a.page; else std::cout << "null";
                std::cout << "\n";
              indent(5); std::cout << "timestamp: " << a.timestamp << "\n";
            }
        }
    }

    // 5) Print data.products
    std::cout << "\ndata.products:\n";
    for (auto const &p : root.data.products) {
      indent(1); std::cout << "product:\n";
        indent(2); std::cout << "sku:   " << p.sku   << "\n";
        indent(2); std::cout << "name:  " << p.name  << "\n";
        indent(2); std::cout << "price: " << p.price << "\n";
        indent(2); std::cout << "tags:  ";
          print_string_vec(p.tags, 2); std::cout << "\n";
        indent(2); std::cout << "dimensions:\n";
          indent(3); std::cout << "w=" << p.dimensions.width
                               << " h=" << p.dimensions.height
                               << " d=" << p.dimensions.depth
                               << " unit=" << p.dimensions.unit << "\n";
        indent(2); std::cout << "warehouseLocations:\n";
        for (auto const &w : p.warehouseLocations) {
          indent(3); std::cout << w.region
                               << " qty=" << w.quantity << "\n";
        }
    }

    // 6) Print logs
    std::cout << "\nlogs:\n";
    for (auto const &l : root.logs) {
      indent(1); std::cout << "level: " << l.level << "\n";
      indent(1); std::cout << "messages:\n";
      for (auto const &m : l.messages) {
        indent(2); std::cout << "- " << m << "\n";
      }
      indent(1); std::cout << "timestamp: " << l.timestamp << "\n";
      indent(1); std::cout << "context:\n";
        indent(2); std::cout << "path:    " << l.context->path    << "\n";
        indent(2); std::cout << "attempt: " << l.context->attempt << "\n";
    }

    // 7) Print misc
    std::cout << "\nmisc:\n";
    indent(1); std::cout << "emptyObject: ";
      if (root.misc.emptyObject.has_value()) std::cout << "{}";
      else std::cout << "null";
      std::cout << "\n";
    indent(1); std::cout << "emptyArray: [ ]\n";
    indent(1); std::cout << "nestedMixed:\n";
    print_vec3d(root.misc.nestedMixed, 1);
    std::cout << "\n";
    indent(1); std::cout << "unicodeTest: \""
                         << root.misc.unicodeTest << "\"\n";
    indent(1); std::cout << "scientific: ";
    {
      std::cout << "[";
      for (size_t i=0; i<root.misc.scientific.size(); ++i) {
        std::cout << root.misc.scientific[i];
        if (i+1<root.misc.scientific.size()) std::cout << ", ";
      }
      std::cout << "]\n";
    }

}


