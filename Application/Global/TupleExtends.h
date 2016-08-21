
#pragma once

#include <tuple>
#include <type_traits>


namespace std {


namespace Details {


///////////////////////////////////////////////////////////////////////////////
// _RemoveByIndexVariadic // struct declaration / definition / specialisation//
///////////////////////////////////////////////////////////////////////////////
template<size_t I, typename T>
struct _RemoveByIndexVariadic;

//-----------------------------------------------------------------------------
template<size_t I, typename T, typename... Ts>
struct _RemoveByIndexVariadic<I, tuple<T, Ts...>>
  {
  using type = decltype(
    tuple_cat(
      declval<tuple<T>>(),
      declval<typename _RemoveByIndexVariadic<I - 1, tuple<Ts...>>::type>()
    ));
  };

//-----------------------------------------------------------------------------
template<typename T, typename... Ts>
struct _RemoveByIndexVariadic<0, tuple<T, Ts...>>
  {
  typedef tuple<Ts...> type;
  };


///////////////////////////////////////////////////////////////////////////////
// _RemoveByIndexRecursive // struct declaration / definition / specialisation//
///////////////////////////////////////////////////////////////////////////////
template<typename T, long N, size_t I>
struct _RemoveByIndexRecursive;

//-----------------------------------------------------------------------------
template<typename T, long N, size_t I>
struct _RemoveByIndexRecursive
  {
  using Type = decltype(
    tuple_cat(
      declval<typename _RemoveByIndexRecursive<T, N - 1, I - 1>::Type>(),
      declval<tuple<typename tuple_element<I - 1, T>::type>>()
    ));
  };

//-----------------------------------------------------------------------------
template<typename T, size_t I>
struct _RemoveByIndexRecursive<T, 0, I>
  {
  using Type = typename _RemoveByIndexRecursive<T, -1, I - 1>::Type;
  };

//-----------------------------------------------------------------------------
template<typename T, long N>
struct _RemoveByIndexRecursive<T, N, 0>
  {
  using Type = tuple<>;
  };


///////////////////////////////////////////////////////////////////////////////
// _RemoveItemHelper // struct definition / specialisations //
///////////////////////////////////////////////////////////////////////////////
template<typename TTo, typename TFrom, size_t N, size_t I, bool IsIGreaterN>
struct _RemoveItemHelper;

//-----------------------------------------------------------------------------
template<typename TTo, typename TFrom, size_t N, size_t I>
struct _RemoveItemHelper<TTo, TFrom, N, I, true>
  {
  static void CopyItem(TTo& o_tuple_to, TFrom const& i_tuple_from)
    {
    _RemoveItemHelper<TTo, TFrom, N, I - 1, (I - 1 > N)>::CopyItem(o_tuple_to, i_tuple_from);
    std::get<I - 1>(o_tuple_to) = std::get<I>(i_tuple_from);
    }
  };

//-----------------------------------------------------------------------------
template<typename TTo, typename TFrom, size_t N, size_t I>
struct _RemoveItemHelper<TTo, TFrom, N, I, false>
  {
  static void CopyItem(TTo& o_tuple_to, TFrom const& i_tuple_from)
    {
    _RemoveItemHelper<TTo, TFrom, N, I - 1, false>::CopyItem(o_tuple_to, i_tuple_from);
    std::get<I - 1>(o_tuple_to) = std::get<I - 1>(i_tuple_from);
    }
  };

//-----------------------------------------------------------------------------
template<typename TTo, typename TFrom, size_t N>
struct _RemoveItemHelper<TTo, TFrom, N, 0, false>
  {
  static void CopyItem(TTo& o_tuple_to, TFrom const& i_tuple_from)
    {
    // End recursion
    }
  };


///////////////////////////////////////////////////////////////////////////////
// ArrayLikeResolver // struct declaration //
///////////////////////////////////////////////////////////////////////////////
template<typename T,
  typename TArg,
  size_t NArg,
  typename Enable = void>
  struct ArrayLikeResolver;


} // namespace Details


///////////////////////////////////////////////////////////////////////////////
// RemoveTypeByIndexToTuple // struct definition //
///////////////////////////////////////////////////////////////////////////////
template<typename T, size_t N>
struct RemoveTypeByIndexToTuple
  {
  static constexpr auto Size = tuple_size<T>::value;

  static_assert(N < Size, "Out of range!");

  using Type = typename Details::_RemoveByIndexRecursive<T, Size - N - 1, Size>::Type;
  };


///////////////////////////////////////////////////////////////////////////////
// RemoveTypeByIndex // struct definition //
///////////////////////////////////////////////////////////////////////////////
template<typename T, size_t N>
struct RemoveTypeByIndex
  {
  static constexpr auto Size = tuple_size<T>::value;

  static_assert(N < Size, "Out of range!");

  using Type = typename Details::_RemoveByIndexRecursive<T, Size - N - 1, Size>::Type;
  };

///////////////////////////////////////////////////////////////////////////////
// RemoveTypeByIndex // struct specialisation for array-like classes //
///////////////////////////////////////////////////////////////////////////////
template<template<typename, size_t> class T,
  typename TArg,
  size_t NArg,
  size_t N>
  struct RemoveTypeByIndex<T<TArg, NArg>, N>
  {
  static constexpr auto Size = tuple_size<T<TArg, NArg>>::value;

  static_assert(N < Size, "Out of range!");

  using Type = T<TArg, Size - 1>;
  };


///////////////////////////////////////////////////////////////////////////////
// tuple_size // class specialisation for all array-like classes //
///////////////////////////////////////////////////////////////////////////////
template<template<typename, size_t> class T,
  typename TArg,
  size_t NArg>
  class tuple_size<T<TArg, NArg>>
  : public Details::ArrayLikeResolver<T<TArg, NArg>, TArg, NArg>
  {
  };


///////////////////////////////////////////////////////////////////////////////
// tuple_element // struct specialisation for all array-like classes //
///////////////////////////////////////////////////////////////////////////////
template<size_t N,
  template<typename, size_t> class T,
  typename TArg,
  size_t NArg>
  struct tuple_element<N, T<TArg, NArg>>
  {
  using type = typename Details::ArrayLikeResolver<T<TArg, NArg>, TArg, NArg>::type;
  };


///////////////////////////////////////////////////////////////////////////////
// RemoveItem // function definition //
///////////////////////////////////////////////////////////////////////////////
template<size_t N, typename T>
typename RemoveTypeByIndex<T, N>::Type
RemoveItem(T const& i_tuple)
  {
  static constexpr size_t Size = std::tuple_size<T>::value;
  static constexpr size_t I = Size - 1;
  using ResType = RemoveTypeByIndex<T, N>::Type;

  ResType res;
  Details::_RemoveItemHelper<ResType, T, N, I, (I > N)>::CopyItem(res, i_tuple);
  return res;
  }


} // namespace std