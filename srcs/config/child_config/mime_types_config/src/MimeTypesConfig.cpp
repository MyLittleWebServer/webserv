#include "../include/MimeTypesConfig.hpp"

MimeTypesConfig::MimeTypesConfig() {}

MimeTypesConfig::~MimeTypesConfig() {}

MimeTypesConfig::MimeTypesConfig(const MimeTypesConfig& src) { *this = src; }

MimeTypesConfig& MimeTypesConfig::operator=(const MimeTypesConfig& src) {
  if (this != &src) {
    // do something
  }
  return *this;
}

void MimeTypesConfig::setVariable(const std::string& key,
                                  const std::string& value) {
  std::cout << "MimeTypesConfig::setVariable() called" << std::endl;
  std::cout << "key: " << key << std::endl;
  std::cout << "value: " << value << std::endl;
  _data.insert(std::pair<std::string, std::string>(key, value));
}
