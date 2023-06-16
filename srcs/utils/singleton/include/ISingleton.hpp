#pragma once

/**
 * @brief 디폴트 생성자용 싱글톤 인터페이스
 * @author middlefitting
 * @date 2023.06.15
 */
template <typename T>
class ISingleton {
 public:
  virtual T& getInstance() = 0;
};
