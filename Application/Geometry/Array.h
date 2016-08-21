
#pragma once

#include "./BaseTypedefs.h"
#include "./../Global/TupleExtends.h"

#include <array>


namespace Geometry {


///////////////////////////////////////////////////////////////////////////////
// Array // struct declaration //
///////////////////////////////////////////////////////////////////////////////
template<typename TValueType, DimensionType NDimension = 3>
struct Array
  {
  static_assert(NDimension >= 2, "0 and 1-Dimentional arrays not supported.");

  using Self = Array<TValueType, NDimension>;

  static constexpr DimensionType Dimension = NDimension;

  using ValueType = TValueType;
  using SizeType = DimensionType;

  using Reference = ValueType&;
  using ConstReference = ValueType const&;

  using Data = std::array<ValueType, Dimension>;

  // Lifetime
  Array();
  template<typename ... TArgs>
  Array(TArgs&& ... i_args); // N args constructor
  template<typename TSelf>
  Array(TSelf&& i_rhs); // Copy - Move constructor

  // CompileTime safe element access via Axis class
  template<DimensionType N>
  Reference Get();
  template<DimensionType N>
  ConstReference Get() const;
  template<DimensionType N>
  void Set(ConstReference i_value);
  
  // Runtime non-safe element access via index
  Reference operator[](SizeType i_index);
  ConstReference operator[](SizeType i_index) const;

  Data& GetData();
  Data const& GetData() const;

  template<DimensionType NDirection = Dimension>
  Array<TValueType, NDimension + 1> Extend(ValueType const& i_value) const;

  // Comparison operators
  bool operator==(Array const& i_rhs) const;
  bool operator!=(Array const& i_rhs) const;

  protected:
    template<DimensionType N>
    static void _DimensionCheckEqual();
    template<DimensionType N>
    static void _DimensionCheckLess();

  private:
    template<DimensionType N, typename T, typename ... TArgs>
    void _Init(T const& value, TArgs&& ... i_args);

    template<DimensionType N>
    void _Init();

    Data m_data;
  };


///////////////////////////////////////////////////////////////////////////////
// Array // struct definition //
///////////////////////////////////////////////////////////////////////////////
template<typename TValueType, DimensionType NDimension>
Array<TValueType, NDimension>::Array()
  {
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
template<typename ... TArgs>
Array<TValueType, NDimension>::Array(TArgs&& ... i_args)
  {
  _DimensionCheckEqual<sizeof...(TArgs)>();
  _Init<0>(std::forward<TArgs>(i_args)...);
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
template<typename TSelf>
Array<TValueType, NDimension>::Array(TSelf&& i_rhs)
  : m_data(i_rhs.m_data)
  {
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
template<DimensionType N>
typename Array<TValueType, NDimension>::Reference
Array<TValueType, NDimension>::Get()
  {
  return const_cast<Reference>(const_cast<Self const*>(this)->Get<N>());
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
template<DimensionType N>
typename Array<TValueType, NDimension>::ConstReference
Array<TValueType, NDimension>::Get() const
  {
  _DimensionCheckLess<N>();
  return operator[](N);
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
template<DimensionType N>
void
Array<TValueType, NDimension>::Set(ConstReference i_value)
  {
  _DimensionCheckLess<N>();
  operator[](N) = i_value;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
typename Array<TValueType, NDimension>::Reference
Array<TValueType, NDimension>::operator[](SizeType i_index)
  {
  return const_cast<Reference>(const_cast<Self const*>(this)->operator[](i_index));
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
typename Array<TValueType, NDimension>::ConstReference
Array<TValueType, NDimension>::operator[](SizeType i_index) const
  {
  return m_data[i_index];
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
typename Array<TValueType, NDimension>::Data&
Array<TValueType, NDimension>::GetData()
  {
  return const_cast<Data&>(const_cast<Self const*>(this)->GetData());
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
typename Array<TValueType, NDimension>::Data const&
Array<TValueType, NDimension>::GetData() const
  {
  return m_data;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
template<DimensionType NDirection>
Array<TValueType, NDimension + 1>
Array<TValueType, NDimension>::Extend(ValueType const& i_value) const
  {
  static constexpr DimensionType NResDimension = NDimension + 1;
  Array<TValueType, NResDimension> res;
  for(DimensionType i = 0; i < NDirection; ++i)
    res[i] = m_data[i];
  res[NDirection] = i_value;
  for(DimensionType i = NDirection; i < NDimension; ++i)
    res[i + 1] = m_data[i];
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
bool
Array<TValueType, NDimension>::operator==(Array const& i_rhs) const
  {
  return m_data == i_rhs.m_data;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
bool
Array<TValueType, NDimension>::operator!=(Array const& i_rhs) const
  {
  return m_data != i_rhs.m_data;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
template<DimensionType N>
void
Array<TValueType, NDimension>::_DimensionCheckEqual()
  {
  static_assert(N == Dimension, "Wrong Dimension!");
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
template<DimensionType N>
void
Array<TValueType, NDimension>::_DimensionCheckLess()
  {
  static_assert(N < Dimension, "Wrong Dimension!");
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
template<DimensionType N, typename T, typename ... TArgs>
void Array<TValueType, NDimension>::_Init(T const& value, TArgs&&... i_args)
  {
  static_assert(N < Dimension, "Wrong Dimension!");

  _Init<N + 1>(std::forward<TArgs>(i_args)...);
  operator[](N) = value;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
template<DimensionType N>
void Array<TValueType, NDimension>::_Init()
  {
  _DimensionCheckEqual<N>();
  }


} // namespace Geometry


namespace std {


///////////////////////////////////////////////////////////////////////////////
// get(Geometry::Array) // function specialisation //
///////////////////////////////////////////////////////////////////////////////
template<std::size_t I, typename TArg, DimensionType NArg>
TArg& get(Geometry::Array<TArg, NArg>& i_array)
  {
  return get<I>(i_array.GetData());
  }

//-----------------------------------------------------------------------------
template<std::size_t I, typename TArg, DimensionType NArg>
TArg const& get(Geometry::Array<TArg, NArg> const& i_array)
  {
  return get<I>(i_array.GetData());
  }


namespace Details {


///////////////////////////////////////////////////////////////////////////////
// TupleResolver // struct specialisation for derived from Geometry::Array classes//
///////////////////////////////////////////////////////////////////////////////
template<typename T,
  typename TArg,
  size_t NArg>
struct ArrayLikeResolver<T, TArg, NArg,
  typename enable_if<is_base_of<Geometry::Array<TArg, NArg>, T>::value>::type>
  : public integral_constant<std::size_t, NArg>
  {
  using type = TArg;
  };


} // namespace Details


} // namespace std
