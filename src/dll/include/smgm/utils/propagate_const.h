#pragma once

#if __cpp_lib_experimental_propagate_const >= 201505
#include <experimental/propagate_const>

namespace smgm::utils {
template <typename T>
using propagate_const = std::experimental::propagate_const<T>;
}
#else
#include <type_traits>

namespace smgm::utils {
template <typename T>
class propagate_const {
 public:
  using element_type = std::remove_reference_t<decltype(*std::declval<T&>())>;

  constexpr propagate_const() = default;

  template <typename... Args>
  constexpr propagate_const(Args&&... args)
      : data_(std::forward<Args>(args)...) {}

  propagate_const(const propagate_const&) = delete;
  propagate_const(propagate_const&& rhs) : data_(std::move(rhs.data_)) {}

  inline element_type* get() { return data_.get(); }
  inline const element_type* get() const { return data_.get(); }

  inline constexpr element_type& operator*() { return *data_; }
  inline constexpr const element_type& operator*() const { return *data_; }
  inline constexpr element_type* operator->() { return data_.get(); }
  inline constexpr const element_type* operator->() const {
    return data_.get();
  }

 private:
  T data_;
};
}  // namespace smgm::utils
#endif
