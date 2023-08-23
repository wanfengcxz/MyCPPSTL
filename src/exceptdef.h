//
// Created by 晚风吹行舟 on 2023/5/5.
//

#ifndef MYCPPSTL_EXCEPTDEF_H
#define MYCPPSTL_EXCEPTDEF_H

#include <stdexcept>

#include <cassert>

namespace stl
{

#define STL_DEBUG(expr) \
  assert(expr)

#define THROW_LENGTH_ERROR_IF(expr, what) \
  if ((expr)) throw std::length_error(what)

#define THROW_OUT_OF_RANGE_IF(expr, what) \
  if ((expr)) throw std::out_of_range(what)

#define THROW_RUNTIME_ERROR_IF(expr, what) \
  if ((expr)) throw std::runtime_error(what)

} // namespace stl

#endif //MYCPPSTL_EXCEPTDEF_H
