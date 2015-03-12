#ifndef HTTP
#define HTTP
#include "helper.h"
#include "net.h"
namespace ns {

namespace http {

const static ns::Separator header_parser = Separator({":", "\r\n"});
struct RequestHeader {
  std::string method = "GET", version = "HTTP/1.0", path = "/";
  std::map<std::string, std::string> headers;
  RequestHeader() {}
  RequestHeader(const ns::URI& uri);
};

struct ResponseHeader {
  std::string description, version = "HTTP/1.0";
  int status = 0;
  std::map<std::string, std::string> headers;
};

DescriptorStream request(const ns::URI& req, ResponseHeader& response,
                         const std::string& method = "GET");
DescriptorStream request(const ns::URI& req, const std::string& method = "GET");

void parse_headers(std::istream& in,
                   std::map<std::string, std::string>& headers);

std::ostream& operator<<(std::ostream& o, const RequestHeader& req);
std::istream& operator>>(std::istream& in, RequestHeader& req);

std::istream& operator>>(std::istream& in, ResponseHeader& res);
std::ostream& operator<<(std::ostream& o, const ResponseHeader& res);
}
}
#endif  // HTTP
