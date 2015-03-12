#include "helper.h"
#include <unistd.h>
#include <sstream>
ns::vectorstreambuf::vectorstreambuf(uint64_t bufsize)
    : std::streambuf(), bufsize_(bufsize) {
  buffer_.resize(bufsize_);
}

ns::vectorstreambuf::vectorstreambuf(vectorstreambuf &&that) {
  buffer_ = std::move(that.buffer_);
  out_buffer_ = std::move(that.out_buffer_);
  bufsize_ = that.bufsize_;
}

ns::vectorstreambuf &ns::vectorstreambuf::operator=(
    ns::vectorstreambuf &&that) {
  std::swap(buffer_, that.buffer_);
  std::swap(out_buffer_, that.out_buffer_);
  std::swap(bufsize_, that.bufsize_);
  return *this;
}

std::streambuf::int_type ns::vectorstreambuf::underflow() {
  if (gptr() < egptr()) return traits_type::to_int_type(*gptr());
  ssize_t size = read_some(buffer_.data(), buffer_.size());
  if (size <= 0) return traits_type::eof();
  char *start = &buffer_[0];
  char *end = &buffer_[size];
  setg(&buffer_[0], start, end);
}

std::streambuf::int_type ns::vectorstreambuf::overflow(int_type __c) {
  if (__c == traits_type::eof()) return traits_type::eof();
  out_buffer_.push_back(__c);
  if (out_buffer_.size() >= bufsize_) {
    ssize_t s = write_some(out_buffer_.data(), out_buffer_.size());
    if (s <= 0) return traits_type::eof();
    out_buffer_.resize(out_buffer_.size() - s);
  }
  return out_buffer_.size();
}

std::streambuf::int_type ns::vectorstreambuf::sync() {
  ssize_t s, total = 0;
  while (total < out_buffer_.size()) {
    s = write_some(&out_buffer_[total], out_buffer_.size() - total);
    if (s <= 0) return traits_type::eof();
    total += s;
  }
  out_buffer_.clear();
  return 0;
}

ns::Descriptor::Descriptor(ns::Descriptor &&that) {
  handle_ = that.handle_;
  that.handle_ = -1;
}

ns::Descriptor &ns::Descriptor::operator=(ns::Descriptor &&that) {
  std::swap(handle_, that.handle_);
  return *this;
}

void ns::Descriptor::close() {
  if (handle_ >= 0) ::close(handle_);
  handle_ = -1;
}

ns::Descriptor::~Descriptor() { close(); }

ns::DescriptorStream::DescriptorStream() {}

ns::DescriptorStream::DescriptorStream(ns::Descriptor &fd_, uint64_t bufsize)
    : ns::vectorstreambuf(bufsize), fd(std::move(fd_)) {}

ns::DescriptorStream::DescriptorStream(ns::DescriptorStream &&that)
    : ns::vectorstreambuf(std::move(that)) {
  fd = std::move(that.fd);
}

ns::DescriptorStream &ns::DescriptorStream::operator=(
    ns::DescriptorStream &&that) {
  std::swap(fd, that.fd);
  return *this;
}

ssize_t ns::DescriptorStream::read_some(char *buffer, size_t max) {
  if (!fd.is_valid()) return -1;
  ssize_t s;
  s = ::read(*fd, buffer, max);
  if (s <= 0) fd.close();
  return s;
}

ssize_t ns::DescriptorStream::write_some(char *buffer, size_t max) {
  if (!fd.is_valid()) return -1;
  ssize_t s;
  s = ::write(*fd, buffer, max);
  if (s <= 0) fd.close();
  return s;
}

std::string ns::Separator::scan(std::istream &in, size_t len) const {
  std::vector<char> d;
  int t;
  while (!in.eof() && d.size() < len) {
    t = in.get();
    if (t >= 0)
      d.push_back(t);
    else
      break;
    for (auto &sep : sep_) {
      if (d.size() >= sep.size() &&
          sep.compare(0, sep.size(), &d[d.size() - sep.size()], sep.size()) ==
              0) {
        return std::string(begin(d), end(d) - sep.size());
      }
    }
  }
  return std::string(begin(d), end(d));
}
