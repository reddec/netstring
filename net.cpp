#include "net.h"
#include <sstream>
#include <algorithm>
ns::DescriptorStream ns::connect(const std::string& host, uint16_t port) {
  Descriptor sock(::socket(AF_INET, SOCK_STREAM, 0));
  AddrInfo info(host, port);
  if (!info.is_valid() || !sock.is_valid() ||
      ::connect(*sock, info.address()->ai_addr, info.address()->ai_addrlen) <
          0) {
    return DescriptorStream();
  }
  return DescriptorStream(sock);
}

ns::DescriptorStream ns::connect(const ns::URI& uri) {
  return connect(uri.domain, uri.port);
}

ns::AddrInfo::AddrInfo(const std::string& host, uint16_t port) {
  valid = getaddrinfo(host.c_str(), std::to_string(port).c_str(), nullptr,
                      &info) == 0;
}

ns::AddrInfo::~AddrInfo() {
  if (info != nullptr) freeaddrinfo(info);
}

std::istream& ns::operator>>(std::istream& s, ns::NetString& str) {
  long count;
  s >> count;
  if (count < 0)
    str.data = "";
  else {
    s.get();  //:
    std::vector<char> buf(count);
    s.read(&buf[0], count);
    str.data = std::string(buf.data(), count);
    s.get();  //,
  }
  return s;
}

std::ostream& ns::operator<<(std::ostream& o, const ns::NetString& str) {
  o << str.data.size() << ':' << str.data << ',';
  return o;
}

const ns::Separator ns::URI::scheme_parser({"://"});
const ns::Separator ns::URI::domain_port_parser({"/", "?"});
const ns::Separator ns::URI::path_parser({"?"});

ns::URI::URI(const std::string& uri) {
  std::stringstream ss(uri);
  scheme = scheme_parser.scan(ss);
  std::string domain_port = domain_port_parser.scan(ss);

  path = path_parser.scan(ss);
  std::cerr << path << std::endl;
  std::getline(ss, query);
  auto port_i = domain_port.find(':');
  if (port_i != std::string::npos) {
    port = std::atof(domain_port.substr(port_i + 1).c_str());
    domain = domain_port.substr(0, port_i);
  } else {
    domain = domain_port;
  }
}

std::istream& ns::operator>>(std::istream& s, ns::URI& str) {
  std::string q;
  s >> q;
  str = ns::URI(q);
  return s;
}

std::ostream& ns::operator<<(std::ostream& o, const ns::URI& str) {
  o << str.scheme << "://" << str.domain << ":" << str.port << "/" << str.path
    << "?" << str.query;
  return o;
}
