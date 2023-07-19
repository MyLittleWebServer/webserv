#pragma once

template <typename T>
std::string itos(T num) {
  std::stringstream ss;
  ss << num;
  return (ss.str());
}