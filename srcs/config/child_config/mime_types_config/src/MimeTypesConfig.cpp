#include "../include/MimeTypesConfig.hpp"

MimeTypesConfig::MimeTypesConfig() {}

MimeTypesConfig::~MimeTypesConfig() {}

MimeTypesConfig::MimeTypesConfig(const MimeTypesConfig& src) { *this = src; }

MimeTypesConfig& MimeTypesConfig::operator=(const MimeTypesConfig& src) {
  if (this != &src) {
    _data = src._data;
  }
  return *this;
}

void MimeTypesConfig::setVariable(const std::string& key,
                                  const std::string& value) {
  std::map<std::string, t_vector_string>::iterator it = _data.find(key);
  t_vector_string elemtets = ft_split(value, ' ');

  if (it != _data.end()) {
    for (t_vector_string::iterator it2 = elemtets.begin();
         it2 != elemtets.end(); ++it2) {
      it->second.push_back(*it2);
    }
  } else {
    _data.insert(std::pair<std::string, t_vector_string>(key, elemtets));
  }

  for (t_vector_string::iterator it2 = elemtets.begin(); it2 != elemtets.end();
       ++it2) {
    std::map<std::string, t_vector_string>::iterator it = _data.find(*it2);
    if (it != _data.end()) {
      it->second.push_back(key);
    } else {
      t_vector_string temp;
      temp.push_back(key);
      _data.insert(std::pair<std::string, t_vector_string>(*it2, temp));
    }
  }
}

const std::string& MimeTypesConfig::getVariable(const std::string& key) {
  std::map<std::string, t_vector_string>::iterator it = _data.find(key);
  if (it != _data.end()) {
    return it->second[0];
  }
  throw ExceptionThrower::InvalidConfigException(NOT_SUPPORT_CONFIG);
}
