#ifndef ADDRESS_H
#define ADDRESS_H

#include <string>
#include <iostream>
#include <sstream>

class Address {
public:
  virtual ~Address() {}
  virtual std::string str() const = 0;
};

inline std::ostream& operator<<(std::ostream& out, const Address& addr) {
  out << addr.str();
  return out;
}

class NameAddress : public Address {
  const std::string& name;
public:
  NameAddress(const std::string& n) : name(n) {}
  virtual std::string str() const {return name;}
};

class ConstAddress : public Address {
  const float val;
public:
  ConstAddress(float v) : val(v) {}
  virtual std::string str() const {
    std::stringstream ss;
    ss << val;
    return ss.str();
  }
};

class TempAddress : public Address {
  static unsigned next;
  const unsigned num;
public:
  TempAddress() : num(next++) {}
  virtual std::string str() const {
    std::stringstream ss;
    ss << "t" << num;
    return ss.str();
  }
  static void reset() {next = 0;}
};    

#endif // ADDRESS_H
