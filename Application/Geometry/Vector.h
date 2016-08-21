
#pragma once

#include "./Array.h"

#include <cmath>
#include <limits>


namespace Geometry {


///////////////////////////////////////////////////////////////////////////////
// Vector // struct declaration //
///////////////////////////////////////////////////////////////////////////////
template<typename TValueType, DimensionType NDimension = 3>
struct Vector : Array<TValueType, NDimension>
  {
  using Ancestor = Array<TValueType, NDimension>;
  using Self = Vector<TValueType, NDimension>;

  static constexpr DimensionType Dimension = Ancestor::Dimension;

  using ValueType = typename Ancestor::ValueType;
  using SizeType = typename Ancestor::SizeType;

  using Reference = typename Ancestor::Reference;
  using ConstReference = typename Ancestor::ConstReference;

  using Data = typename Ancestor::Data;

  // Lifetime
  Vector();
  template<typename ... TArgs>
  Vector(TArgs&& ... i_args); // N args constructor
  template<typename TSelf>
  Vector(TSelf&& i_rhs); // Copy - Move constructor

  using Ancestor::Get;
  using Ancestor::Set;
  using Ancestor::operator[];
  using Ancestor::GetData;
  using Ancestor::operator==;
  using Ancestor::operator!=;

  RealValueType GetSquaredLength() const;
  RealValueType GetLength() const;
  void Normalise();

  // multiple vectors
  ValueType operator*(Self const& i_vec) const; // dot/inner/scalar product
  Self operator^(Self const& i_vec) const; // cross product(only for 3D)

  // multiple vector by scalar
  template<typename T>
  Self operator*(T const& i_value) const;
  template<typename T>
  Self& operator*=(T const& i_value);
  template<typename T>
  Self operator/(T const& i_value) const;
  template<typename T>
  Self& operator/=(T const& i_value);

  // sum vectors
  Self operator-() const; // unary -
  Self operator-(Self const& i_vec) const;
  Self& operator-=(Self const& i_vec);
  Self operator+() const; // unary + // seems useless
  Self operator+(Self const& i_vec) const;
  Self& operator+=(Self const& i_vec);
  };


///////////////////////////////////////////////////////////////////////////////
// Vector // struct definition //
///////////////////////////////////////////////////////////////////////////////
template<typename TValueType, DimensionType NDimension>
Vector<TValueType, NDimension>::Vector()
  {
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
template<typename ... TArgs>
Vector<TValueType, NDimension>::Vector(TArgs&& ... i_args)
  : Ancestor(std::forward<TArgs>(i_args)...)
  {
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
template<typename TSelf>
Vector<TValueType, NDimension>::Vector(TSelf&& i_rhs)
  : Ancestor(std::forward<Ancestor>(i_rhs))
  {
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
RealValueType
Vector<TValueType, NDimension>::GetSquaredLength() const
  {
  RealValueType sum = 0.;
  for(auto const& value : GetData())
    sum += value * value;
  return sum;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
RealValueType
Vector<TValueType, NDimension>::GetLength() const
  {
  return std::sqrt(GetSquaredLength());
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
void
Vector<TValueType, NDimension>::Normalise()
  {
  RealValueType const length = GetLength();
  if(length < std::numeric_limits<RealValueType>::min())
    return;

  auto const inversed_length = static_cast<TValueType>(1. / length);
  for(auto& value : GetData())
    value *= inversed_length;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
typename Vector<TValueType, NDimension>::ValueType
Vector<TValueType, NDimension>::operator*(Self const& i_vec) const
  {
  ValueType res = 0;
  Self const& self = *this;
  for(DimensionType i = 0; i < Dimension; ++i)
    res += self[i] * i_vec[i];
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
typename Vector<TValueType, NDimension>::Self
Vector<TValueType, NDimension>::operator^(Self const& i_vec) const
  {
  Ancestor::template _DimensionCheckEqual<3>();; // Defined only for 3D

  Self const& a = *this;
  Self const& b = i_vec;
  Self c;
  c[0] = a[1] * b[2] - a[2] * b[1];
  c[1] = a[2] * b[0] - a[0] * b[2];
  c[2] = a[0] * b[1] - a[1] * b[0];
  return c;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
template<typename T>
typename Vector<TValueType, NDimension>::Self
Vector<TValueType, NDimension>::operator*(T const& i_value) const
  {
  Self res;
  Self const& self = *this;
  for(DimensionType i = 0; i < Dimension; ++i)
    res[i] = self[i] * i_value;
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
template<typename T>
typename Vector<TValueType, NDimension>::Self&
Vector<TValueType, NDimension>::operator*=(T const& i_value)
  {
  Self& self = *this;
  for(DimensionType i = 0; i < Dimension; ++i)
    self[i] *= i_value;
  return self;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
template<typename T>
typename Vector<TValueType, NDimension>::Self
Vector<TValueType, NDimension>::operator/(T const& i_value) const
  {
  Self res;
  Self const& self = *this;
  for(DimensionType i = 0; i < Dimension; ++i)
    res[i] = self[i] / i_value;
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
template<typename T>
typename Vector<TValueType, NDimension>::Self&
Vector<TValueType, NDimension>::operator/=(T const& i_value)
  {
  Self& self = *this;
  for(DimensionType i = 0; i < Dimension; ++i)
    self[i] /= i_value;
  return self;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
typename Vector<TValueType, NDimension>::Self
Vector<TValueType, NDimension>::operator-() const
  {
  Self res;
  Self const& self = *this;
  for(DimensionType i = 0; i < Dimension; ++i)
    res[i] = -self[i];
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
typename Vector<TValueType, NDimension>::Self
Vector<TValueType, NDimension>::operator-(Self const& i_vec) const
  {
  Self res;
  Self const& self = *this;
  for(DimensionType i = 0; i < Dimension; ++i)
    res[i] = self[i] - i_vec[i];
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
typename Vector<TValueType, NDimension>::Self&
Vector<TValueType, NDimension>::operator-=(Self const& i_vec)
  {
  Self& self = *this;
  for(DimensionType i = 0; i < Dimension; ++i)
    self[i] -= i_vec[i];
  return self;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
typename Vector<TValueType, NDimension>::Self
Vector<TValueType, NDimension>::operator+() const
  {
  return *this;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
typename Vector<TValueType, NDimension>::Self
Vector<TValueType, NDimension>::operator+(Self const& i_vec) const
  {
  Self res;
  Self const& self = *this;
  for(DimensionType i = 0; i < Dimension; ++i)
    res[i] = self[i] + i_vec[i];
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NDimension>
typename Vector<TValueType, NDimension>::Self&
Vector<TValueType, NDimension>::operator+=(Self const& i_vec)
  {
  Self& self = *this;
  for(DimensionType i = 0; i < Dimension; ++i)
    self[i] += i_vec[i];
  return self;
  }


} // namespace Geometry
