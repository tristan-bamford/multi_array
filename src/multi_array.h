/*
Copyright (c) 2023 Tristan Bamford

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the “Software”), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.
*/

#ifndef TB_MULTI_ARRAY_H
#define TB_MULTI_ARRAY_H

#include <concepts>
#include <algorithm>
#include <initializer_list>
#include <cassert>

namespace tb {

  template<typename T>
    concept Index_type = std::convertible_to<std::size_t, T>;

  // Template alias for nested std::initializer_lists 
  template<typename T, std::size_t M, std::size_t... N>
    struct Nested_initializer_impl {
      using type 
        = std::initializer_list<typename Nested_initializer_impl<T, N...>::type>;
    };

  template<typename T, std::size_t N>
    struct Nested_initializer_impl<T, N> {
      using type = std::initializer_list<T>;
    };

  template<typename T, std::size_t M, std::size_t... N>
    using Nested_initializer 
      = typename Nested_initializer_impl<T, M, N...>::type;

  
  // Type function that aliases built-in arrays
  template<typename T, int M, int... N>
    struct C_array_impl {
      using type = C_array_impl<T, N...>::type[M];
    };

  template<typename T, int N>
    struct C_array_impl<T, N> {
      using type = T[N];
    };

  template<typename T, int M, int... N>
    using C_array = typename C_array_impl<T, M, N...>::type;
  

  // Implementation for array_size()
  template<typename T>
    struct C_array_size_impl 
      : public std::integral_constant<std::size_t, 1> {};

  template<typename T, std::size_t N>
    struct C_array_size_impl<T[N]> 
      : public std::integral_constant<std::size_t, 
                                      C_array_size_impl<T>::value * N> {};

  // Returns the number of elements in a built-in array of any rank/dimension.
  template<typename T>
    auto constexpr array_size(const T&) 
    { return C_array_size_impl<T>::value; }

  // Primary template class for multi_array.
  template<typename T, std::size_t M, std::size_t... N>
    class multi_array {
    public:
      using value_type             = multi_array<T, N...>;
      using pointer                = value_type*;
      using const_pointer          = const value_type*;
      using reference              = value_type&;
      using const_reference        = const value_type&;
      using iterator               = value_type*;
      using const_iterator         = const value_type*;
      using size_type              = std::size_t;
      using difference_type        = std::ptrdiff_t;
      using reverse_iterator       = std::reverse_iterator<iterator>;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;


      static consteval auto size() { return M; }
      static consteval auto max_size() { return M; }
      static consteval bool empty() { return size() == 0; }
      
      static consteval auto order() { return sizeof...(N) + 1; }

      //static consteval auto total_size() { return (M * ... * N); }


      constexpr multi_array() = default;
      constexpr multi_array(const multi_array&) = default;

      constexpr multi_array(const T& value)
      { std::fill(begin(), end(), value); }
      
      constexpr multi_array(const Nested_initializer<T, M, N...>& init_list)
      {
        assert(init_list.size() == M);
        std::copy(init_list.begin(), init_list.end(), sub_array_);
      }

      constexpr reference operator[](std::size_t i) noexcept 
      { return sub_array_[i]; }
      constexpr const_reference operator[](std::size_t i) const noexcept 
      { return sub_array_[i]; }

      template<Index_type Index, Index_type... Indices>
        constexpr auto& operator()(Index i, Indices... j) noexcept 
          requires (sizeof...(Indices) + 1 == order())//(sizeof...(Indices) < order())
        { return sub_array_[i](j...); }

      template<Index_type Index, Index_type... Indices>
        constexpr auto& operator()(Index i, Indices... j) const noexcept
          requires (sizeof...(Indices) + 1 == order())
        { return sub_array_[i](j...); }

      template<Index_type Index, Index_type... Indices>
        constexpr auto& at(Index i, Indices... j) noexcept 
          requires (sizeof...(Indices) + 1 == order())
        { assert(i < M); return sub_array_[i](j...); }

      template<Index_type Index, Index_type... Indices>
        constexpr auto& at(Index i, Indices... j) const noexcept
          requires (sizeof...(Indices) + 1 == order())
        { assert(i < M); return sub_array_[i](j...); }

      constexpr iterator begin() noexcept 
      { return iterator(sub_array_); }
      
      constexpr const_iterator begin() const noexcept  
      { return const_iterator(sub_array_); }
      
      constexpr const_iterator cbegin() const noexcept 
      { return const_iterator(sub_array_); }

      constexpr iterator end() noexcept 
      { return iterator(sub_array_ + M); }
      
      constexpr const_iterator end() const noexcept  
      { return const_iterator(sub_array_ + M); }
      
      constexpr const_iterator cend() const noexcept 
      { return const_iterator(sub_array_ + M); }

      constexpr iterator rbegin() noexcept
      { return reverse_iterator(end()); }
      
      constexpr const_iterator rbegin() const noexcept
      { return const_reverse_iterator(end()); }
      
      constexpr const_iterator crbegin() const noexcept
      { return const_reverse_iterator(end()); }

      constexpr iterator rend() noexcept        
      { return reverse_iterator(begin()); }
      
      constexpr const_iterator rend() const noexcept  
      { return const_reverse_iterator(begin()); }
      
      constexpr const_iterator crend() const noexcept 
      { return const_reverse_iterator(begin()); }

      constexpr reference front() noexcept 
      { return sub_array_[0]; }
      
      constexpr const_reference front() const noexcept 
      { return sub_array_[0]; }

      constexpr reference back() noexcept 
      { return sub_array_[M-1]; }
      
      constexpr const_reference back() const noexcept 
      { return sub_array_[M-1]; }

      constexpr auto data() noexcept 
      { return sub_array_->data(); }
      
      constexpr auto data() const noexcept 
      { return sub_array_->data(); }

      constexpr void fill(const T& value)
      { std::fill(begin(), end(), value); }

      constexpr void swap(multi_array& a) noexcept
      { std::swap_ranges(begin(), end(), a.begin()); }

    private:
      multi_array<T, N...> sub_array_[M];
    };

  // Specialization template class for multi_arrays of order/rank = 1
  template<typename T, std::size_t N>
    class multi_array<T, N> {
    public:
      using value_type             = T;
      using pointer                = value_type*;
      using const_pointer          = const value_type*;
      using reference              = value_type&;
      using const_reference        = const value_type&;
      using iterator               = value_type*;
      using const_iterator         = const value_type*;
      using size_type              = std::size_t;
      using difference_type        = std::ptrdiff_t;
      using reverse_iterator       = std::reverse_iterator<iterator>;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;    

      static consteval auto size() { return N; }
      static consteval auto max_size() { return size(); }
      static consteval bool empty() { return size() == 0; }

      static consteval auto order() { return 1; }

      constexpr multi_array() = default;
      constexpr multi_array(const multi_array&) = default;
      
      constexpr multi_array(const T& value) 
      { std::fill(begin(), end(), value); }

      constexpr multi_array(const std::initializer_list<T>& initializer)
      { 
        assert(initializer.size() == N); 
        std::copy(initializer.begin(), initializer.end(), begin());
      }

      constexpr reference operator[](std::size_t i) noexcept
      { return sub_array_[i]; }
      constexpr const_reference operator[](std::size_t i) const noexcept 
      { return sub_array_[i]; }

      constexpr reference operator()(std::size_t i) noexcept 
      { return sub_array_[i]; }
      constexpr const_reference operator()(std::size_t i) const noexcept 
      { return sub_array_[i]; }

      constexpr reference at(std::size_t i) noexcept 
      { assert(i < N); return sub_array_[i]; }
      constexpr const_reference at(std::size_t i) const noexcept 
      { assert(i < N); return sub_array_[i]; }

      //constexpr auto& operator()(void) noexcept { return *this; }
      //constexpr auto& operator()(void) const noexcept { return *this; }

      constexpr iterator begin() noexcept 
      { return iterator(sub_array_); }
      
      constexpr const_iterator begin() const noexcept  
      { return const_iterator(sub_array_); }
      
      constexpr const_iterator cbegin() const noexcept 
      { return const_iterator(sub_array_); }

      constexpr iterator end() noexcept 
      { return iterator(sub_array_ + N); }
      
      constexpr const_iterator end() const noexcept  
      { return const_iterator(sub_array_ + N); }
      
      constexpr const_iterator cend() const noexcept 
      { return const_iterator(sub_array_ + N); }

      constexpr iterator rbegin() noexcept
      { return reverse_iterator(end()); }
      
      constexpr const_iterator rbegin() const noexcept
      { return const_reverse_iterator(end()); }
      
      constexpr const_iterator crbegin() const noexcept
      { return const_reverse_iterator(end()); }

      constexpr iterator rend() noexcept        
      { return reverse_iterator(begin()); }
      
      constexpr const_iterator rend() const noexcept  
      { return const_reverse_iterator(begin()); }
      
      constexpr const_iterator crend() const noexcept 
      { return const_reverse_iterator(begin()); }

      constexpr reference front() noexcept 
      { return sub_array_[0]; }
      
      constexpr const_reference front() const noexcept 
      { return sub_array_[0]; }

      constexpr reference back() noexcept 
      { return sub_array_[N-1]; }
      
      constexpr const_reference back() const noexcept 
      { return sub_array_[N-1]; }

      constexpr pointer data() noexcept 
      { return sub_array_; }
      
      constexpr const_pointer data() const noexcept 
      { return sub_array_; }

      constexpr void fill(const T& value) noexcept
      { std::fill(begin(), end(), value); }

      constexpr void swap(multi_array& a) noexcept
      { std::swap_ranges(begin(), end(), a.begin()); }

    private:
      T sub_array_[N];
    };

  // Type function for deducing a multi_array from a built-in array
  template<typename T, typename>
    struct multi_array_for_impl;

  template<typename T, std::size_t... I>
    struct multi_array_for_impl<T, std::index_sequence<I...>> {
      using type 
        = multi_array<std::remove_all_extents_t<T>, std::extent_v<T, I>...>;
    };

  template<typename T>
    using multi_array_for 
      = multi_array_for_impl<T, std::make_index_sequence<std::rank_v<T>>>::type; 

  // Comparison operator to test for equivalency
  template<typename T, std::size_t M, std::size_t... N>
    constexpr bool
    operator==(const multi_array<T, M, N...> lhs, 
              const multi_array<T, M, N...> rhs)
    {
      for (std::size_t i = 0; i < M; ++i) {
        if (lhs[i] != rhs[i]) return false;
      }
      return true;
    }

  // Comparison operator to test when not equivalent
  template<typename T, std::size_t M, std::size_t... N>
    constexpr bool
    operator!=(const multi_array<T, M, N...> lhs, 
              const multi_array<T, M, N...> rhs)
    { return !(lhs == rhs); }
 
  // Extracts the I-th element from the array using tuple-like interface.
  template<std::size_t I, typename T, std::size_t N, std::size_t... M>
    constexpr const auto& get(const multi_array<T, N, M...>& a) noexcept
    { static_assert(I < N); return a[I]; }

  template<std::size_t I, std::size_t... J,
           typename T, std::size_t N, std::size_t... M>
      requires (sizeof...(J) <= sizeof...(M)) && (sizeof...(J) > 0)
    constexpr const auto& get(const multi_array<T, N, M...>& a) noexcept
    { static_assert(I < N); return get<J...>(a[I]); }

  template<std::size_t I, typename T, std::size_t N, std::size_t... M>
    constexpr auto& get(multi_array<T, N, M...>& a) noexcept
    { static_assert(I < N); return a[I]; }

  template<std::size_t I, std::size_t... J,
           typename T, std::size_t N, std::size_t... M>
      requires (sizeof...(J) <= sizeof...(M)) && (sizeof...(J) > 0)
    constexpr auto& get(multi_array<T, N, M...>& a) noexcept
    { static_assert(I < N); return get<J...>(a[I]); }


  // Swaps the contents of two multi_arrays
  template<typename T, std::size_t M, std::size_t... N>
    constexpr void 
    swap(multi_array<T, M, N...>& lhs, 
                        multi_array<T, M, N...>& rhs) noexcept
    { lhs.swap(rhs); }

  // Creates a multi_array from a built-in array
  template<typename T, typename R = std::remove_all_extents_t<T>>
    constexpr auto
    to_multi_array(const T& a)
    {
      multi_array_for<T> result;
      std::copy_n((R*)a, array_size(a), result.data());
      return result;
    }

  /*
  template<typename T, std::size_t M, std::size_t... N>
    auto constexpr array_size(const multi_array<T, M, N...>&)
    { return (M * ... * N); }
  */

} // namespace tb
#endif//TB_MULTI_ARRAY_H