#ifndef LABEL_H
#define LABEL_H

#include <string>
#include <iostream>
#include <sstream>

class Label {
  static unsigned next;
  const unsigned num;
public:
  Label() : num(next++) {}
  std::string str() const {
    std::stringstream ss;
    ss << "L" << num;
    return ss.str();
  }
};  

inline std::ostream& operator<<(std::ostream& out, const Label& label) {
  out << label.str();
  return out;
}

#endif // LABEL_H
