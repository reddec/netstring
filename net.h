#ifndef NET
#define NET
#include "helper.h"
#include <netdb.h>
#include <string>
#include <stdint.h>
#include <iostream>
#include <map>
namespace ns {
class AddrInfo {
 public:
  AddrInfo(const std::string &host, uint16_t port);
  inline const addrinfo *address() const { return info; }
  inline bool is_valid() const { return valid; }
  ~AddrInfo();

 private:
  addrinfo *info = nullptr;
  bool valid = false;
};

struct NetString {
  std::string data;
  explicit NetString(const std::string &ns) : data(ns) {}
  NetString() {}
};

std::istream &operator>>(std::istream &s, NetString &str);
std::ostream &operator<<(std::ostream &o, const NetString &str);

struct URI {
  std::string scheme;
  std::string path;
  std::string domain;
  uint16_t port = 0;
  std::string query;
  URI() {}
  explicit URI(const std::string &uri);

 private:
  const static Separator scheme_parser, domain_port_parser, path_parser;
};
std::istream &operator>>(std::istream &s, URI &str);
std::ostream &operator<<(std::ostream &o, const URI &str);

DescriptorStream connect(const std::string &host, uint16_t port);
DescriptorStream connect(const URI &uri);
}

#endif  // NET
