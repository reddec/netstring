#ifndef HELPER
#define HELPER
#include <vector>
#include <ios>
#include <iostream>
#include <memory>
namespace ns {
class vectorstreambuf : public std::streambuf {
  std::vector<char> buffer_;
  std::vector<char> out_buffer_;
  uint64_t bufsize_;

 public:
  vectorstreambuf(uint64_t bufsize = 8192);

  vectorstreambuf(const vectorstreambuf &) = delete;

  vectorstreambuf &operator=(const vectorstreambuf &) = delete;

  vectorstreambuf(vectorstreambuf &&that);

  vectorstreambuf &operator=(vectorstreambuf &&that);

 protected:
  virtual std::streambuf::int_type underflow();

  virtual std::streambuf::int_type overflow(int_type __c);

  virtual std::streambuf::int_type sync();
  virtual ssize_t read_some(char *buffer, size_t max) = 0;
  virtual ssize_t write_some(char *buffer, size_t max) = 0;
};

class Descriptor {
 private:
  int handle_ = -1;

 public:
  Descriptor() {}
  Descriptor(int handle) : handle_(handle) {}
  Descriptor(const Descriptor &) = delete;
  Descriptor &operator=(const Descriptor &) = delete;
  Descriptor(Descriptor &&that);
  Descriptor &operator=(Descriptor &&that);
  inline int get() const { return handle_; }
  void close();
  inline bool is_valid() const { return handle_ >= 0; }
  inline int operator*() const { return handle_; }
  inline bool operator==(const Descriptor &obj) const {
    return handle_ == obj.handle_;
  }
  inline bool operator!=(const Descriptor &obj) const {
    return handle_ != obj.handle_;
  }
  inline bool operator>(const Descriptor &obj) const {
    return handle_ > obj.handle_;
  }
  inline bool operator<(const Descriptor &obj) const {
    return handle_ < obj.handle_;
  }
  inline bool operator>=(const Descriptor &obj) const {
    return handle_ >= obj.handle_;
  }
  inline bool operator<=(const Descriptor &obj) const {
    return handle_ <= obj.handle_;
  }
  ~Descriptor();
};

class DescriptorStream : public vectorstreambuf {
  Descriptor fd;

 public:
  DescriptorStream();
  DescriptorStream(Descriptor &fd_, uint64_t bufsize = 8192);
  DescriptorStream(const DescriptorStream &) = delete;
  DescriptorStream &operator=(const DescriptorStream &) = delete;
  DescriptorStream(DescriptorStream &&that);
  DescriptorStream &operator=(DescriptorStream &&that);
  inline const Descriptor &descriptor() const { return fd; }
  inline Descriptor &descriptor() { return fd; }

 protected:
  virtual ssize_t read_some(char *buffer, size_t max);

  virtual ssize_t write_some(char *buffer, size_t max);
};

template <class T, class ErrType = bool, class DefaultT = T, ErrType dok = true>
struct result {
  T value;
  const ErrType ok;
  result(T &&value_, ErrType ok_) : ok(ok_), value(std::move(value_)) {}
  result(const T &value_, ErrType ok_) : ok(ok_), value(std::move(value_)) {}
  result(const result &) = delete;
  result &operator=(const result &) = delete;

  result(result &&that) : ok(that.ok) {
    value = std::move(that.value);
    that.value = DefaultT();
  }

  result &operator=(result &&that) {
    std::swap(value, that.value);
    std::swap(ok, that.ok);
    return *this;
  }
};

struct Separator {
  const std::vector<std::string> sep_;
  Separator(const std::vector<std::string> &s) : sep_(s) {}

  std::string scan(std::istream &in, size_t len = std::string::npos) const;
};
}
#endif  // HELPER
