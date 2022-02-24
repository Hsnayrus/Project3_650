#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <cstring>
#include <sstream>

std::string num2Str(int i) {
  std::stringstream newStream;
  newStream << i;
  return newStream.str();
}

int str2num(std::string num) {
  return atoi(num.c_str());
}
#endif