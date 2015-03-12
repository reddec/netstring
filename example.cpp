#include "netstring.hpp"
#include <iostream>
#include <string>

int main() {
  ns::URI uri("http://rbc.ru:80");
  std::cout << "******************" << std::endl;
  std::cout << "Schema: " << uri.scheme << std::endl;
  std::cout << "Domain: " << uri.domain << std::endl;
  std::cout << "  Port: " << uri.port << std::endl;
  std::cout << "  Path: " << uri.path << std::endl;
  std::cout << " Query: " << uri.query << std::endl;
  std::cout << "******************" << std::endl;

  ns::DescriptorStream sock = ns::http::request(uri);
  std::istream inp(&sock);
  if (!sock.descriptor().is_valid())
    std::cerr << "Failed request to " << uri << std::endl;
  std::string fline;
  do {
    std::getline(inp, fline);
    std::cout << fline << std::endl;
  } while (!inp.eof());
  return 0;
}
