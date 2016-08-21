
#pragma once

#include "./Vector.h"


namespace Geometry {


///////////////////////////////////////////////////////////////////////////////
// Point // struct declaration //
///////////////////////////////////////////////////////////////////////////////
template<typename TValueType, DimensionType NDimension = 3>
struct Point : Array<TValueType, NDimension>
  {
  using Ancestor = Array<TValueType, NDimension>;
  using Self = Point<TValueType, NDimension>;

  static constexpr DimensionType Dimension = Ancestor::Dimension;

  using ValueType = typename Ancestor::ValueType;
  using SizeType = typename Ancestor::SizeType;

  using Reference = typename Ancestor::Reference;
  using ConstReference = typename Ancestor::ConstReference;

  using Data = typename Ancestor::Data;

  using VectorType = Vector<ValueType, Dimension>;

  // Lifetime
  Point();
  template<typename ... TArgs>
  Point(TArgs&& ... i_args); // N args constructor
  template<typename TSelf>
  Point(TSelf&& i_rhs); // Copy - Move constructor

  using Ancestor::Get;
  using Ancestor::Set;
  using Ancestor::operator[];
  using Ancestor::GetData;
  using Ancestor::operator==;
  using Ancestor::operator!=;

  // sum points
  VectorType operator-() const; // unary -
  VectorType operator-(Self const& i_pt) const;
  Self operator-(VectorType const& i_vec) const;
  Self& operator-=(VectorType const& i_vec);
  VectorType operator+() const; // unary +
  VectorType operator+(Self const& i_pt) const;
  Self operator+(VectorType const& i_vec) const;
  Self& operator+=(VectorType const& i_vec);
  };


///////////////////////////////////////////////////////////////////////////////
// Point // struct definition //
///////////////////////////////////////////////////////////////////////////////
template<typename TValueType, DimensionType NPointDimension>
Point<TValueType, NPointDimension>::Point()
  {
  GetData().fill(ValueType());
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NPointDimension>
template<typename ... TArgs>
Point<TValueType, NPointDimension>::Point(TArgs&& ... i_args)
  : Ancestor(std::forward<TArgs>(i_args)...)
  {
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NPointDimension>
template<typename TSelf>
Point<TValueType, NPointDimension>::Point(TSelf&& i_rhs)
  : Ancestor(std::forward<Ancestor>(i_rhs))
  {
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NPointDimension>
typename Point<TValueType, NPointDimension>::VectorType
Point<TValueType, NPointDimension>::operator-() const
  {
  VectorType res;
  Self const& self = *this;
  for(DimensionType i = 0; i < Dimension; ++i)
    res[i] = -self[i];
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NPointDimension>
typename Point<TValueType, NPointDimension>::VectorType
Point<TValueType, NPointDimension>::operator-(Self const& i_pt) const
  {
  VectorType res;
  Self const& self = *this;
  for(DimensionType i = 0; i < Dimension; ++i)
    res[i] = self[i] - i_pt[i];
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NPointDimension>
typename Point<TValueType, NPointDimension>::Self
Point<TValueType, NPointDimension>::operator-(VectorType const& i_vec) const
  {
  Self res;
  Self const& self = *this;
  for(DimensionType i = 0; i < Dimension; ++i)
    res[i] = self[i] - i_vec[i];
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NPointDimension>
typename Point<TValueType, NPointDimension>::Self&
Point<TValueType, NPointDimension>::operator-=(VectorType const& i_vec)
  {
  Self& self = *this;
  for(DimensionType i = 0; i < Dimension; ++i)
    self[i] -= i_vec[i];
  return self;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NPointDimension>
typename Point<TValueType, NPointDimension>::VectorType
Point<TValueType, NPointDimension>::operator+() const
  {
  VectorType res;
  Self const& self = *this;
  for(DimensionType i = 0; i < Dimension; ++i)
    res[i] = self[i];
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NPointDimension>
typename Point<TValueType, NPointDimension>::VectorType
Point<TValueType, NPointDimension>::operator+(Self const& i_pt) const
  {
  VectorType res;
  Self const& self = *this;
  for(DimensionType i = 0; i < Dimension; ++i)
    res[i] = self[i] + i_pt[i];
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NPointDimension>
typename Point<TValueType, NPointDimension>::Self
Point<TValueType, NPointDimension>::operator+(VectorType const& i_vec) const
  {
  Self res;
  Self const& self = *this;
  for(DimensionType i = 0; i < Dimension; ++i)
    res[i] = self[i] + i_vec[i];
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NPointDimension>
typename Point<TValueType, NPointDimension>::Self&
Point<TValueType, NPointDimension>::operator+=(VectorType const& i_vec)
  {
  Self& self = *this;
  for(DimensionType i = 0; i < Dimension; ++i)
    self[i] += i_vec[i];
  return self;
  }


} // namespace Geometry
