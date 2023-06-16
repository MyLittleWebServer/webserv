#pragma once

/**
 * @brief 디폴트 생성자로 싱글톤을 구현하고자 하는 클래스에서 상속받아 사용한다.
 * @author middlefitting
 * @date 2023.06.15
 */
template <typename T>
class Singleton : public ISingleton<T> {
 private:
  Singleton(void){};
  virtual ~Singleton(void){};
  Singleton(const src&){};
  Singleton& operator=(const src&){};

 public:
  static T& getInstance() {
    static T instance;
    return instance;
  }
};
