#pragma once

#include "../../include/IChildConfig.hpp"

/**
 * @brief Proxy 설정 파일 정보를 제공하는 인터페이스
 * @author middlefitting
 * @date 2023.06.17
 */
class IProxyConfig : public IChildConfig {
 public:
  virtual std::size_t getClientMaxBodySize() = 0;
  virtual std::size_t getRequestUriLimitSize() = 0;
  virtual std::size_t getRequestHeaderLimitSize() = 0;
};
