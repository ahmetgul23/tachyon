#ifndef TACHYON_BASE_CONTAINERS_CONTAINER_UTIL_H_
#define TACHYON_BASE_CONTAINERS_CONTAINER_UTIL_H_

#include <algorithm>
#include <iterator>
#include <optional>
#include <utility>
#include <vector>

#include "absl/algorithm/container.h"

#include "tachyon/base/functional/functor_traits.h"
#include "tachyon/base/logging.h"
#include "tachyon/base/numerics/checked_math.h"
#include "tachyon/base/random.h"

namespace tachyon::base {

template <typename T>
std::vector<T> CreateRangedVector(T start, T end, T step = 1) {
  CHECK_LT(start, end);
  CHECK_GT(step, T{0});
  std::vector<T> ret;
  size_t size = static_cast<size_t>((end - start + step - 1) / step);
  ret.reserve(size);
  T v = start;
  std::generate_n(std::back_inserter(ret), size, [&v, step]() {
    T ret = v;
    v += step;
    return ret;
  });
  return ret;
}

template <typename Generator,
          typename FunctorTraits = internal::MakeFunctorTraits<Generator>,
          typename RunType = typename FunctorTraits::RunType,
          typename ReturnType = typename FunctorTraits::ReturnType,
          typename ArgList = internal::ExtractArgs<RunType>,
          size_t ArgNum = internal::GetSize<ArgList>,
          std::enable_if_t<ArgNum == 0>* = nullptr>
std::vector<ReturnType> CreateVector(size_t size, Generator&& generator) {
  std::vector<ReturnType> ret;
  ret.reserve(size);
  std::generate_n(std::back_inserter(ret), size,
                  std::forward<Generator>(generator));
  return ret;
}

template <typename Generator,
          typename FunctorTraits = internal::MakeFunctorTraits<Generator>,
          typename RunType = typename FunctorTraits::RunType,
          typename ReturnType = typename FunctorTraits::ReturnType,
          typename ArgList = internal::ExtractArgs<RunType>,
          size_t ArgNum = internal::GetSize<ArgList>,
          std::enable_if_t<ArgNum == 1>* = nullptr>
std::vector<ReturnType> CreateVector(size_t size, Generator&& generator) {
  std::vector<ReturnType> ret;
  ret.reserve(size);
  size_t idx = 0;
  std::generate_n(
      std::back_inserter(ret), size,
      [&idx, generator = std::forward<Generator>(generator)]() mutable {
        return generator(idx++);
      });
  return ret;
}

template <typename T,
          std::enable_if_t<!internal::IsCallableObject<T>::value>* = nullptr>
std::vector<T> CreateVector(size_t size, const T& initial_value) {
  std::vector<T> ret;
  ret.resize(size, initial_value);
  return ret;
}

template <typename Iterator, typename UnaryOp,
          typename FunctorTraits = internal::MakeFunctorTraits<UnaryOp>,
          typename RunType = typename FunctorTraits::RunType,
          typename ReturnType = typename FunctorTraits::ReturnType,
          typename ArgList = internal::ExtractArgs<RunType>,
          size_t ArgNum = internal::GetSize<ArgList>,
          std::enable_if_t<ArgNum == 1>* = nullptr>
std::vector<ReturnType> Map(Iterator begin, Iterator end, UnaryOp&& op) {
  std::vector<ReturnType> ret;
  ret.reserve(std::distance(begin, end));
  std::transform(begin, end, std::back_inserter(ret),
                 std::forward<UnaryOp>(op));
  return ret;
}

template <typename Iterator, typename UnaryOp,
          typename FunctorTraits = internal::MakeFunctorTraits<UnaryOp>,
          typename RunType = typename FunctorTraits::RunType,
          typename ReturnType = typename FunctorTraits::ReturnType,
          typename ArgList = internal::ExtractArgs<RunType>,
          size_t ArgNum = internal::GetSize<ArgList>,
          std::enable_if_t<ArgNum == 2>* = nullptr>
std::vector<ReturnType> Map(Iterator begin, Iterator end, UnaryOp&& op) {
  std::vector<ReturnType> ret;
  ret.reserve(std::distance(begin, end));
  size_t idx = 0;
  std::transform(begin, end, std::back_inserter(ret),
                 [&idx, op = std::forward<UnaryOp>(op)](auto& item) mutable {
                   return op(idx++, item);
                 });
  return ret;
}

template <typename Container, typename UnaryOp>
auto Map(Container&& container, UnaryOp&& op) {
  return Map(std::begin(container), std::end(container),
             std::forward<UnaryOp>(op));
}

template <typename Iterator, typename UnaryOp,
          typename FunctorTraits = internal::MakeFunctorTraits<UnaryOp>,
          typename ReturnType = typename FunctorTraits::ReturnType::value_type>
std::vector<ReturnType> FlatMap(Iterator begin, Iterator end, UnaryOp&& op) {
  std::vector<std::vector<ReturnType>> tmp;
  tmp.reserve(std::distance(begin, end));
  std::transform(begin, end, std::back_inserter(tmp),
                 std::forward<UnaryOp>(op));

  base::CheckedNumeric<size_t> size = 0;
  for (size_t i = 0; i < tmp.size(); ++i) {
    size += tmp[i].size();
  }

  std::vector<ReturnType> ret;
  ret.reserve(size.ValueOrDie());
  std::for_each(tmp.begin(), tmp.end(), [&ret](std::vector<ReturnType>& vec) {
    ret.insert(ret.end(), std::make_move_iterator(vec.begin()),
               std::make_move_iterator(vec.end()));
  });
  return ret;
}

template <typename Container, typename UnaryOp>
auto FlatMap(Container&& container, UnaryOp&& op) {
  return FlatMap(std::begin(container), std::end(container),
                 std::forward<UnaryOp>(op));
}

template <typename Iterator, typename T>
std::optional<size_t> FindIndex(Iterator begin, Iterator end, const T& value) {
  auto it = std::find(begin, end, value);
  if (it == end) return std::nullopt;
  return std::distance(begin, it);
}

template <typename Container, typename T>
std::optional<size_t> FindIndex(const Container& container, const T& value) {
  return FindIndex(std::begin(container), std::end(container), value);
}

template <typename Iterator, typename UnaryOp>
std::optional<size_t> FindIndexIf(Iterator begin, Iterator end, UnaryOp&& op) {
  auto it = std::find_if(begin, end, std::forward<UnaryOp>(op));
  if (it == end) return std::nullopt;
  return std::distance(begin, it);
}

template <typename Container, typename UnaryOp>
std::optional<size_t> FindIndexIf(const Container& container, UnaryOp&& op) {
  return FindIndexIf(std::begin(container), std::end(container),
                     std::forward<UnaryOp>(op));
}

namespace internal {

template <typename Iterator, typename T>
std::vector<size_t> DoFindIndices(Iterator begin, Iterator end, const T& value,
                                  const std::random_access_iterator_tag&) {
  std::vector<size_t> matches;
  auto it = std::find(begin, end, value);
  while (it != end) {
    size_t index = std::distance(begin, it);
    matches.push_back(index);
    it = std::find(it + 1, end, value);
  }
  return matches;
}

template <typename Iterator, typename T>
std::vector<size_t> DoFindIndices(Iterator begin, Iterator end, const T& value,
                                  const std::input_iterator_tag&) {
  std::vector<size_t> matches;
  auto it = std::find(begin, end, value);
  auto last_it = begin;
  size_t last_index = 0;
  while (it != end) {
    size_t index = last_index + std::distance(last_it, it);
    matches.push_back(index);
    last_it = it;
    last_index = index;
    it = std::find(last_it + 1, end, value);
  }
  return matches;
}

template <typename Iterator, typename UnaryOp>
std::vector<size_t> DoFindIndicesIf(Iterator begin, Iterator end, UnaryOp&& op,
                                    const std::random_access_iterator_tag&) {
  std::vector<size_t> matches;
  auto it = std::find_if(begin, end, std::forward<UnaryOp>(op));
  while (it != end) {
    size_t index = std::distance(begin, it);
    matches.push_back(index);
    it = std::find_if(it + 1, end, std::forward<UnaryOp>(op));
  }
  return matches;
}

template <typename Iterator, typename UnaryOp>
std::vector<size_t> DoFindIndicesIf(Iterator begin, Iterator end, UnaryOp&& op,
                                    const std::input_iterator_tag&) {
  std::vector<size_t> matches;
  auto it = std::find_if(begin, end, std::forward<UnaryOp>(op));
  auto last_it = begin;
  size_t last_index = 0;
  while (it != end) {
    size_t index = last_index + std::distance(last_it, it);
    matches.push_back(index);
    last_it = it;
    last_index = index;
    it = std::find_if(last_it + 1, end, std::forward<UnaryOp>(op));
  }
  return matches;
}

}  // namespace internal

template <typename Iterator, typename T>
std::vector<size_t> FindIndices(Iterator begin, Iterator end, const T& value) {
  using iterator_category =
      typename std::iterator_traits<Iterator>::iterator_category;
  return internal::DoFindIndices(begin, end, value, iterator_category());
}

template <typename Container, typename T>
std::vector<size_t> FindIndices(const Container& container, const T& value) {
  return FindIndices(std::begin(container), std::end(container), value);
}

template <typename Iterator, typename UnaryOp>
std::vector<size_t> FindIndicesIf(Iterator begin, Iterator end, UnaryOp&& op) {
  using iterator_category =
      typename std::iterator_traits<Iterator>::iterator_category;
  return internal::DoFindIndicesIf(begin, end, std::forward<UnaryOp>(op),
                                   iterator_category());
}

template <typename Container, typename UnaryOp>
std::vector<size_t> FindIndicesIf(const Container& container, UnaryOp&& op) {
  return FindIndicesIf(std::begin(container), std::end(container),
                       std::forward<UnaryOp>(op));
}

template <typename Container>
void Shuffle(Container& container) {
  absl::c_shuffle(container, GetAbslBitGen());
}

}  // namespace tachyon::base

#endif  // TACHYON_BASE_CONTAINERS_CONTAINER_UTIL_H_
