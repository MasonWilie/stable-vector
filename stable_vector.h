#pragma once

#include <array>
#include <limits>
#include <memory>
#include <new>
#include <sstream>
#include <type_traits>
#include <vector>

namespace sv {

namespace {

constexpr int ceil_to_power_of_two(const int original,
                                   const int current_bit = 0) {
  constexpr const int MAX_POWER =
      std::numeric_limits<int>::max() - (std::numeric_limits<int>::max() >> 1);
  const int current_power = 1 << current_bit;

  if (current_power >= original || current_bit == MAX_POWER) {
    return current_power;
  }

  return ceil_to_power_of_two(original, current_bit + 1);
}

template <typename unsigned_int_t>
constexpr bool is_power_of_two(const unsigned_int_t number) {
  return number != 0 && (number & (number - 1)) == 0;
}

template <int DIVISOR>
int mod_power_of_two(const int dividend) {
  static_assert(is_power_of_two(DIVISOR));

  return dividend & (DIVISOR - 1);
}

}  // namespace

template <typename T, int BLOCK_SIZE = 1024>
class stable_vector {
 public:
  using value_type = T;
  using const_value_type = const T;
  using reference = value_type&;
  using const_reference = const_value_type&;
  using size_type = size_t;

 public:
  stable_vector();

  size_type size() const noexcept;
  size_type max_size() const noexcept;
  size_type capacity() const noexcept;
  [[nodiscard]] bool empty() const noexcept;
  void reserve(const size_type new_size);

  reference at(const size_type index);
  const_reference at(const size_type index) const;

  reference operator[](const size_type index);
  const_reference operator[](const size_type index) const;

  reference front();
  const_reference front() const;

  reference back();
  const_reference back() const;

  void push_back(const value_type& val);
  void push_back(value_type&& val);

  template <class... Args>
  reference emplace_back(Args&&... args);

 private:
  static constexpr const int EFFECTIVE_BLOCK_SIZE =
      ceil_to_power_of_two(BLOCK_SIZE);

  using storage_t = typename std::aligned_storage<sizeof(value_type),
                                                  alignof(value_type)>::type;
  using block_t = std::array<storage_t, EFFECTIVE_BLOCK_SIZE>;
  using block_ptr_t = std::unique_ptr<block_t>;

  static size_type calculate_block_index(const size_type index) noexcept;
  static size_type calculate_block_offset(const size_type index) noexcept;

  void validate_index_bounds(const size_type index) const;

  storage_t& create_next_storage();

  std::vector<block_ptr_t> _blocks;
  size_type _size;
};

template <typename T, int BLOCK_SIZE>
stable_vector<T, BLOCK_SIZE>::stable_vector() : _size(0) {}

template <typename T, int BLOCK_SIZE>
inline auto stable_vector<T, BLOCK_SIZE>::size() const noexcept -> size_type {
  return _size;
}

template <typename T, int BLOCK_SIZE>
inline auto stable_vector<T, BLOCK_SIZE>::max_size() const noexcept
    -> size_type {
  return std::numeric_limits<size_type>::max();
}

template <typename T, int BLOCK_SIZE>
inline auto stable_vector<T, BLOCK_SIZE>::capacity() const noexcept
    -> size_type {
  return _blocks.size() * EFFECTIVE_BLOCK_SIZE;
}

template <typename T, int BLOCK_SIZE>
[[nodiscard]] inline bool stable_vector<T, BLOCK_SIZE>::empty() const noexcept {
  return _size == 0;
}

template <typename T, int BLOCK_SIZE>
inline void stable_vector<T, BLOCK_SIZE>::reserve(const size_type new_size) {
  _blocks.reserve((new_size + EFFECTIVE_BLOCK_SIZE - 1) / EFFECTIVE_BLOCK_SIZE);
}

template <typename T, int BLOCK_SIZE>
auto stable_vector<T, BLOCK_SIZE>::at(const size_type index) -> reference {
  validate_index_bounds(index);
  return (*this)[index];
}

template <typename T, int BLOCK_SIZE>
auto stable_vector<T, BLOCK_SIZE>::at(const size_type index) const
    -> const_reference {
  validate_index_bounds(index);
  return (*this)[index];
}

template <typename T, int BLOCK_SIZE>
auto stable_vector<T, BLOCK_SIZE>::operator[](const size_type index)
    -> reference {
  const auto block_index = calculate_block_index(index);
  const auto block_offset = calculate_block_offset(index);

  auto& storage = (*_blocks[block_index])[block_offset];

  return *std::launder(reinterpret_cast<value_type*>(&storage));
}

template <typename T, int BLOCK_SIZE>
auto stable_vector<T, BLOCK_SIZE>::operator[](const size_type index) const
    -> const_reference {
  const auto block_index = calculate_block_index(index);
  const auto block_offset = calculate_block_offset(index);

  const auto& storage = (*_blocks[block_index])[block_offset];

  return *std::launder(reinterpret_cast<const_value_type*>(&storage));
}

template <typename T, int BLOCK_SIZE>
inline auto stable_vector<T, BLOCK_SIZE>::calculate_block_index(
    const size_type index) noexcept -> size_type {
  return index / EFFECTIVE_BLOCK_SIZE;
}

template <typename T, int BLOCK_SIZE>
inline auto stable_vector<T, BLOCK_SIZE>::calculate_block_offset(
    const size_type index) noexcept -> size_type {
  return mod_power_of_two<EFFECTIVE_BLOCK_SIZE>(index);
}

template <typename T, int BLOCK_SIZE>
void stable_vector<T, BLOCK_SIZE>::validate_index_bounds(
    const size_type index) const {
  if (index >= _size) {
    std::stringstream err;
    err << index << " out of range for vector of size = " << _size;
    throw std::out_of_range(err.str());
  }
}

template <typename T, int BLOCK_SIZE>
auto stable_vector<T, BLOCK_SIZE>::front() -> reference {
  return (*this)[0];
}

template <typename T, int BLOCK_SIZE>
auto stable_vector<T, BLOCK_SIZE>::front() const -> const_reference {
  return (*this)[0];
}

template <typename T, int BLOCK_SIZE>
auto stable_vector<T, BLOCK_SIZE>::back() -> reference {
  return (*this)[_size - 1];
}

template <typename T, int BLOCK_SIZE>
auto stable_vector<T, BLOCK_SIZE>::back() const -> const_reference {
  return (*this)[_size - 1];
}

template <typename T, int BLOCK_SIZE>
void stable_vector<T, BLOCK_SIZE>::push_back(const value_type& val) {
  auto& storage = create_next_storage();

  ::new (&storage) value_type(val);
}

template <typename T, int BLOCK_SIZE>
void stable_vector<T, BLOCK_SIZE>::push_back(value_type&& val) {
  emplace_back(std::move(val));
}

template <typename T, int BLOCK_SIZE>
template <class... Args>
auto stable_vector<T, BLOCK_SIZE>::emplace_back(Args&&... args) -> reference {
  auto& storage = create_next_storage();
  return *(::new (&storage) value_type(std::forward<Args>(args)...));
}

template <typename T, int BLOCK_SIZE>
auto stable_vector<T, BLOCK_SIZE>::create_next_storage() -> storage_t& {
  if (_size == capacity()) {
    _blocks.emplace_back(block_ptr_t(new block_t));
  }

  const auto block_index = calculate_block_index(_size);
  const auto block_offset = calculate_block_offset(_size);

  auto& storage = (*_blocks[block_index])[block_offset];

  ++_size;

  return storage;
}

}  // namespace sv
