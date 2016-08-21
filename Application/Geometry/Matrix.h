
#pragma once

#include "./Point.h"


namespace Geometry {


///////////////////////////////////////////////////////////////////////////////
// Matrix // struct declaration //
///////////////////////////////////////////////////////////////////////////////
template<typename TValueType, DimensionType NRowsCount = 3, DimensionType NColumnsCount = 3>
struct Matrix
  {
  using Self = Matrix<TValueType, NRowsCount, NColumnsCount>;

  static constexpr DimensionType RowsCount = NRowsCount;
  static constexpr DimensionType ColumnsCount = NColumnsCount;

  using ValueType = TValueType;

  using Reference = ValueType&;
  using ConstReference = ValueType const&;

  using RawPointer = ValueType*;
  using ConstRawPointer = ValueType const*;

  using Data = std::array<ValueType, RowsCount * ColumnsCount>;

  using VectorLeft = Vector<ValueType, RowsCount>;
  using VectorRight = Vector<ValueType, ColumnsCount>;

  using PointLeft = Point<ValueType, RowsCount>;
  using PointRight = Point<ValueType, ColumnsCount>;

  template<DimensionType N>
  using MatrixLeft = Matrix<ValueType, N, RowsCount>;
  template<DimensionType N>
  using MatrixRight = Matrix<ValueType, ColumnsCount, N>;

  // CompileTime safe element access
  template<DimensionType NRow, DimensionType NColumn>
  Reference Get();
  template<DimensionType NRow, DimensionType NColumn>
  ConstReference Get() const;
  template<DimensionType NRow, DimensionType NColumn>
  void Set(ConstReference i_value);

  // Runtime non-safe element access
  Reference operator()(DimensionType i_row, DimensionType i_column);
  ConstReference operator()(DimensionType i_row, DimensionType i_column) const;

  RawPointer operator[](DimensionType i_row);
  ConstRawPointer operator[](DimensionType i_row) const;

  Data& GetData();
  Data const& GetData() const;

  // Comparison operators
  bool operator==(Matrix const& i_rhs) const;
  bool operator!=(Matrix const& i_rhs) const;

  // multiple Matrix by Scalar/Vector/Point/Matrix
  Self operator/(ValueType const& i_scalar) const;
  Self operator*(ValueType const& i_scalar) const;
  VectorLeft operator*(VectorRight const& i_vector) const;
  PointLeft operator*(PointRight const& i_point) const;
  template<DimensionType NColumnsCountNew>
  Matrix<TValueType, NRowsCount, NColumnsCountNew> operator*(Matrix<ValueType, ColumnsCount, NColumnsCountNew> const& i_matrix) const;

  Self& operator/=(ValueType const& i_scalar);
  Self& operator*=(ValueType const& i_scalar);
  Self& operator*=(Matrix<ValueType, ColumnsCount, ColumnsCount> const& i_matrix);

  // multiple from left Matrix by Scalar/Vector/Point/Matrix
  Self PreMultiply(ValueType const& i_scalar) const;
  VectorRight PreMultiply(VectorLeft const& i_vector) const;
  PointRight PreMultiply(PointLeft const& i_point) const;
  template<DimensionType NRowsCountNew>
  Matrix<TValueType, NRowsCountNew, NColumnsCount> PreMultiply(Matrix<ValueType, NRowsCountNew, RowsCount> const& i_matrix) const;

  // multiple from right Matrix by Scalar/Vector/Point/Matrix
  Self PostMultiply(ValueType const& i_scalar) const;
  VectorLeft PostMultiply(VectorRight const& i_vector) const;
  PointLeft PostMultiply(PointRight const& i_point) const;
  template<DimensionType NColumnsCountNew>
  Matrix<TValueType, NRowsCount, NColumnsCountNew> PostMultiply(Matrix<ValueType, ColumnsCount, NColumnsCountNew> const& i_matrix) const;

  // +-
  Self operator+(Self const& i_matrix);
  Self& operator+=(Self const& i_matrix);
  Self operator-(Self const& i_matrix);
  Self& operator-=(Self const& i_matrix);

  void MakeIdentity();

  private:
    Data m_data;
  };


///////////////////////////////////////////////////////////////////////////////
// Matrix // struct definition //
///////////////////////////////////////////////////////////////////////////////
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
template<DimensionType NRow, DimensionType NColumn>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::Reference
Matrix<TValueType, NRowsCount, NColumnsCount>::Get()
  {
  return const_cast<Reference>(const_cast<Self const*>(this)->Get<NRow, NColumn>());
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
template<DimensionType NRow, DimensionType NColumn>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::ConstReference
Matrix<TValueType, NRowsCount, NColumnsCount>::Get() const
  {
  static_assert(NRow < RowsCount, "Wrong Row Number!");
  static_assert(NColumn < ColumnsCount, "Wrong Column Number!");
  return operator()(NRow, NColumn);
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
template<DimensionType NRow, DimensionType NColumn>
void
Matrix<TValueType, NRowsCount, NColumnsCount>::Set(ConstReference i_value)
  {
  static_assert(NRow < RowsCount, "Wrong Row Number!");
  static_assert(NColumn < ColumnsCount, "Wrong Column Number!");
  operator[](NRow, NColumn) = i_value;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::Reference
Matrix<TValueType, NRowsCount, NColumnsCount>::operator()(DimensionType i_row, DimensionType i_column)
  {
  return const_cast<Reference>(const_cast<Self const*>(this)->operator()(i_row, i_column));
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::ConstReference
Matrix<TValueType, NRowsCount, NColumnsCount>::operator()(DimensionType i_row, DimensionType i_column) const
  {
  return m_data[i_row * NRowsCount + i_column];
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::RawPointer
Matrix<TValueType, NRowsCount, NColumnsCount>::operator[](DimensionType i_row)
  {
  return const_cast<RawPointer>(const_cast<Self const*>(this)->operator[](i_row));
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::ConstRawPointer
Matrix<TValueType, NRowsCount, NColumnsCount>::operator[](DimensionType i_row) const
  {
  return &m_data[i_row * RowsCount];
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::Data&
Matrix<TValueType, NRowsCount, NColumnsCount>::GetData()
  {
  return const_cast<Data&>(const_cast<Self const*>(this)->GetData());
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::Data const&
Matrix<TValueType, NRowsCount, NColumnsCount>::GetData() const
  {
  return m_data;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
bool
Matrix<TValueType, NRowsCount, NColumnsCount>::operator==(Matrix const& i_rhs) const
  {
  return m_data == i_rhs.m_data;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
bool
Matrix<TValueType, NRowsCount, NColumnsCount>::operator!=(Matrix const& i_rhs) const
  {
  return m_data != i_rhs.m_data;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::Self
Matrix<TValueType, NRowsCount, NColumnsCount>::operator/(ValueType const& i_scalar) const
  {
  Self res;
  for(DimensionType i = 0; i < m_data.size(); ++i)
    res.GetData()[i] += m_data[i] / i_scalar;
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::Self
Matrix<TValueType, NRowsCount, NColumnsCount>::operator*(ValueType const& i_scalar) const
  {
  Self res;
  for(DimensionType i = 0; i < m_data.size(); ++i)
    res.GetData()[i] += m_data[i] * i_scalar;
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::VectorLeft
Matrix<TValueType, NRowsCount, NColumnsCount>::operator*(VectorRight const& i_vector) const
  {
  VectorLeft res;
  Self const& self = *this;
  for(DimensionType row = 0; row < RowsCount; ++row)
    {
    res[row] = 0;
    for(DimensionType column = 0; column < ColumnsCount; ++column)
      res[row] += self(row, column) * i_vector[column];
    }
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::PointLeft
Matrix<TValueType, NRowsCount, NColumnsCount>::operator*(PointRight const& i_point) const
  {
  PointLeft res;
  Self const& self = *this;
  for(DimensionType row = 0; row < RowsCount; ++row)
    {
    res[row] = 0;
    for(DimensionType column = 0; column < ColumnsCount; ++column)
      res[row] += self(row, column) * i_point[column];
    }
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
template<DimensionType NColumnsCountNew>
Matrix<TValueType, NRowsCount, NColumnsCountNew>
Matrix<TValueType, NRowsCount, NColumnsCount>::operator*(Matrix<ValueType, ColumnsCount, NColumnsCountNew> const& i_matrix) const
  {
  Matrix<ValueType, RowsCount, NColumnsCountNew> res;
  Self const& self = *this;
  for(DimensionType row = 0; row < RowsCount; ++row)
    for(DimensionType column = 0; column < NColumnsCountNew; ++column)
      {
      res(row, column) = 0;
      for(DimensionType inner = 0; inner < ColumnsCount; ++inner)
        res(row, column) = self(row, inner) * i_matrix(inner, column);
      }
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::Self&
Matrix<TValueType, NRowsCount, NColumnsCount>::operator/=(ValueType const& i_scalar)
  {
  for(auto& value : m_data)
    value /= i_scalar;
  return *this;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::Self&
Matrix<TValueType, NRowsCount, NColumnsCount>::operator*=(ValueType const& i_scalar)
  {
  for(auto& value : m_data)
    value *= i_scalar;
  return *this;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::Self&
Matrix<TValueType, NRowsCount, NColumnsCount>::operator*=(Matrix<ValueType, ColumnsCount, ColumnsCount> const& i_matrix)
  {
  Self& self = *this;
  self = self * i_matrix;
  return self;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::Self
Matrix<TValueType, NRowsCount, NColumnsCount>::PreMultiply(ValueType const& i_scalar) const
  {
  return this->operator*(i_scalar);
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::VectorRight
Matrix<TValueType, NRowsCount, NColumnsCount>::PreMultiply(VectorLeft const& i_vector) const
  {
  VectorRight res;
  Self const& self = *this;
  for(DimensionType column = 0; column < ColumnsCount; ++column)
    {
    res[column] = 0;
    for(DimensionType row = 0; row < ColumnsCount; ++row)
      res[column] += self(row, column) * i_vector[row];
    }
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::PointRight
Matrix<TValueType, NRowsCount, NColumnsCount>::PreMultiply(PointLeft const& i_point) const
  {
  PointRight res;
  Self const& self = *this;
  for(DimensionType column = 0; column < ColumnsCount; ++column)
    {
    res[column] = 0;
    for(DimensionType row = 0; row < ColumnsCount; ++row)
      res[column] += self(row, column) * i_point[row];
    }
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
template<DimensionType NRowsCountNew>
Matrix<TValueType, NRowsCountNew, NColumnsCount>
Matrix<TValueType, NRowsCount, NColumnsCount>::PreMultiply(Matrix<ValueType, NRowsCountNew, RowsCount> const& i_matrix) const
  {
  Matrix<TValueType, NRowsCountNew, NColumnsCount> res;
  Self const& self = *this;
  for(DimensionType row = 0; row < NRowsCountNew; ++row)
    for(DimensionType column = 0; column < NColumnsCount; ++column)
      {
      res(row, column) = 0;
      for(DimensionType inner = 0; inner < NRowsCount; ++inner)
        res(row, column) = i_matrix(row, inner) * self(inner, column);
      }
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::Self
Matrix<TValueType, NRowsCount, NColumnsCount>::PostMultiply(ValueType const& i_scalar) const
  {
  return this->operator*(i_scalar);
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::VectorLeft
Matrix<TValueType, NRowsCount, NColumnsCount>::PostMultiply(VectorRight const& i_vector) const
  {
  return this->operator*(i_vector);
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::PointLeft
Matrix<TValueType, NRowsCount, NColumnsCount>::PostMultiply(PointRight const& i_point) const
  {
  return this->operator*(i_point);
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
template<DimensionType NColumnsCountNew>
Matrix<TValueType, NRowsCount, NColumnsCountNew>
Matrix<TValueType, NRowsCount, NColumnsCount>::PostMultiply(Matrix<ValueType, ColumnsCount, NColumnsCountNew> const& i_matrix) const
  {
  return this->operator*(i_matrix);
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::Self
Matrix<TValueType, NRowsCount, NColumnsCount>::operator+(Self const& i_matrix)
  {
  Self res;
  for(DimensionType i = 0; i < m_data.size(); ++i)
    res.GetData()[i] += m_data[i] + i_matrix.m_data[i];
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::Self&
Matrix<TValueType, NRowsCount, NColumnsCount>::operator+=(Self const& i_matrix)
  {
  for(DimensionType i = 0; i < m_data.size(); ++i)
    m_data[i] += i_matrix.m_data[i];
  return *this;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::Self
Matrix<TValueType, NRowsCount, NColumnsCount>::operator-(Self const& i_matrix)
  {
  Self res;
  for(DimensionType i = 0; i < m_data.size(); ++i)
    res.GetData()[i] += m_data[i] - i_matrix.m_data[i];
  return res;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
typename Matrix<TValueType, NRowsCount, NColumnsCount>::Self&
Matrix<TValueType, NRowsCount, NColumnsCount>::operator-=(Self const& i_matrix)
  {
  for(DimensionType i = 0; i < m_data.size(); ++i)
    m_data[i] -= i_matrix.m_data[i];
  return *this;
  }

//-----------------------------------------------------------------------------
template<typename TValueType, DimensionType NRowsCount, DimensionType NColumnsCount>
void
Matrix<TValueType, NRowsCount, NColumnsCount>::MakeIdentity()
  {
  static_assert(RowsCount == ColumnsCount, "Identity matix mean that matrix is square.");
  for(DimensionType i = 0; i < m_data.size(); ++i)
    m_data[i] = 0;
  for(DimensionType i = 0; i < RowsCount; ++i)
    this->operator()(i, i) = 1;
  }


} // namespace Geometry
