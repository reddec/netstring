#include "http.h"

std::ostream& ns::http::operator<<(std::ostream& o,
                                   const ns::http::RequestHeader& req) {
  o << req.method << ' ' << req.path << ' ' << req.version << "\r\n";
  for (auto& pair : req.headers)
    o << pair.first << ": " << pair.second << "\r\n";
  o << "\r\n";
  return o;
}

std::istream& ns::http::operator>>(std::istream& in,
                                   ns::http::ResponseHeader& res) {
  in >> res.version >> res.status >> res.description;
  in.ignore(2);  // Skip \r\n
  parse_headers(in, res.headers);
  return in;
}

std::ostream& ns::http::operator<<(std::ostream& o,
                                   const ns::http::ResponseHeader& res) {
  o << res.version << ' ' << res.status << ' ' << res.description << "\r\n";
  for (auto& pair : res.headers)
    o << pair.first << ": " << pair.second << "\r\n";
  o << "\r\n";
  return o;
}

std::istream& ns::http::operator>>(std::istream& in,
                                   ns::http::RequestHeader& req) {
  in >> req.method >> req.path >> req.version;
  in.ignore(2);  // Skip \r\n
  parse_headers(in, req.headers);

  return in;
}

void ns::http::parse_headers(std::istream& in,
                             std::map<std::string, std::string>& headers) {
  std::string key, value;
  key = header_parser.scan(in);
  while (!key.empty() && key[0] != '\r') {
    std::ws(in);
    std::getline(in, value, '\r');
    headers[key] = value;
    in.ignore(1);  // Skip \n
    key = header_parser.scan(in);
  }
}

ns::http::RequestHeader::RequestHeader(const ns::URI& uri)
    : path((uri.path.empty() ? "/" : uri.path) + "?" + uri.query) {
  if (path.empty()) path = "/";
  headers["Server"] =
      uri.domain + (uri.port != 80 ? ":" + std::to_string(uri.port) : "");
}

ns::DescriptorStream ns::http::request(const ns::URI& req,
                                       ResponseHeader& response,
                                       const std::string& method) {
  RequestHeader reqHeader(req);
  reqHeader.method = method;
  ns::DescriptorStream ds = connect(req.domain, req.port);
  if (ds.descriptor().is_valid()) {
    std::ostream out(&ds);
    std::istream inp(&ds);
    out << reqHeader;
    out.flush();
    inp >> response;
  }
  return ds;
}

ns::DescriptorStream ns::http::request(const ns::URI& req,
                                       const std::string& method) {
  ResponseHeader resp;
  return request(req, resp, method);
}
