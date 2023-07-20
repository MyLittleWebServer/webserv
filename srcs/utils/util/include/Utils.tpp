/**
 * @file Utils.tpp
 * @brief 유틸리티 함수를 정의한 템플릿 소스파일입니다.
 * @details
 * @author tocatoca
 * @date 2023-07-20
 * @copyright Copyright (c) 2023
 */

#pragma once

template <typename T>
std::string itos(T num) {
  std::stringstream ss;
  ss << num;
  return (ss.str());
}