
#pragma once

#include "./../Global/TupleExtends.h"

#include <cassert>


namespace Geometry {


///////////////////////////////////////////////////////////////////////////////
// LinearInterpolationIterator // class declaration //
///////////////////////////////////////////////////////////////////////////////
template<typename TPointType, DimensionType NDirection>
class LinearInterpolationIterator
  {
  public:
    // typedefs
    using PointType = TPointType;

    static constexpr DimensionType Direction = NDirection;
    static constexpr DimensionType Dimension = std::tuple_size<PointType>::value;
    static constexpr DimensionType AcrossDimension = Dimension - 1;

    using AlongType = typename std::tuple_element<Direction, TPointType>::type;
    using AcrossType = typename std::RemoveTypeByIndexToTuple<PointType, Direction>::Type;

    using Self = LinearInterpolationIterator<PointType, Direction>;

    template<DimensionType N>
    using PointIndexType = typename std::tuple_element<N, TPointType>::type;
    template<DimensionType N>
    using AcrossIndexType = typename std::tuple_element<N, AcrossType>::type;

    // TODO: add check that all across types are std::is_integer
    static_assert(Direction < Dimension, "Can you imagine iteration across axis outside Dimension?");
    static_assert(std::is_integral<AlongType>::value, "Iteration along non-integral types not supported!");

    // methods
    LinearInterpolationIterator(PointType const& i_pt_from, PointType const& i_pt_to);
    void GoToBegin();
    void GoToEnd();
    bool IsAtBegin() const;
    bool IsAtEnd() const;
    Self& operator++();
    Self& operator--();
    PointType operator*() const;

    template<DimensionType N>
    PointIndexType<N> Get() const;

  private:
    // nested classes
    struct _Impl;
    struct _RecursiveImpl;

    // data
    std::unique_ptr<_Impl> mp_impl;
  };

///////////////////////////////////////////////////////////////////////////////
// LinearInterpolationIterator // class definition //
///////////////////////////////////////////////////////////////////////////////
template<typename TPointType, DimensionType NDirection>
LinearInterpolationIterator<TPointType, NDirection>
::LinearInterpolationIterator(PointType const& i_pt_from, PointType const& i_pt_to)
  : mp_impl(std::make_unique<_Impl>(i_pt_from, i_pt_to))
  {
  auto d_along = mp_impl->GetAlongCoord(i_pt_to) - mp_impl->GetAlongCoord(i_pt_from);
  if(d_along == 0)
    return;

  _RecursiveImpl::Initialise<AcrossDimension>(*mp_impl, d_along);
  }

//-----------------------------------------------------------------------------
template<typename TPointType, DimensionType NDirection>
void
LinearInterpolationIterator<TPointType, NDirection>::GoToBegin()
  {
  _RecursiveImpl::ResetIteratorToPoint<AcrossDimension>(*mp_impl, mp_impl->m_pt_from);
  }

//-----------------------------------------------------------------------------
template<typename TPointType, DimensionType NDirection>
void
LinearInterpolationIterator<TPointType, NDirection>::GoToEnd()
  {
  _RecursiveImpl::ResetIteratorToPoint<AcrossDimension>(*mp_impl, mp_impl->m_pt_to);
  }

//-----------------------------------------------------------------------------
template<typename TPointType, DimensionType NDirection>
bool
LinearInterpolationIterator<TPointType, NDirection>::IsAtBegin() const
  {
  return mp_impl->m_along == mp_impl->GetAlongCoord(mp_impl->m_pt_from);
  }

//-----------------------------------------------------------------------------
template<typename TPointType, DimensionType NDirection>
bool
LinearInterpolationIterator<TPointType, NDirection>::IsAtEnd() const
  {
  return mp_impl->m_along == mp_impl->GetAlongCoord(mp_impl->m_pt_to);
  }

//-----------------------------------------------------------------------------
template<typename TPointType, DimensionType NDirection>
typename LinearInterpolationIterator<TPointType, NDirection>::Self&
LinearInterpolationIterator<TPointType, NDirection>::operator++()
  {
  _RecursiveImpl::Increment<AcrossDimension>(*mp_impl);

  return *this;
  }

//-----------------------------------------------------------------------------
template<typename TPointType, DimensionType NDirection>
typename LinearInterpolationIterator<TPointType, NDirection>::Self&
LinearInterpolationIterator<TPointType, NDirection>::operator--()
  {
  _RecursiveImpl::Decrement<AcrossDimension>(*mp_impl);

  return *this;
  }

//-----------------------------------------------------------------------------
template<typename TPointType, DimensionType NDirection>
typename LinearInterpolationIterator<TPointType, NDirection>::PointType
LinearInterpolationIterator<TPointType, NDirection>::operator*() const
  {
  PointType point;

  _RecursiveImpl::FillPoint<Dimension>(*mp_impl, point);

  return point;
  }

//-----------------------------------------------------------------------------
template<typename TPointType, DimensionType NDirection>
template<DimensionType N>
typename LinearInterpolationIterator<TPointType, NDirection>::PointIndexType<N>
LinearInterpolationIterator<TPointType, NDirection>::Get() const
  {
  return mp_impl->Get<N>();
  }


///////////////////////////////////////////////////////////////////////////////
// LinearInterpolationIterator::_Impl // struct declaration //
///////////////////////////////////////////////////////////////////////////////
template<typename TPointType, DimensionType NDirection>
struct LinearInterpolationIterator<TPointType, NDirection>::_Impl
  {
  // This class is nested and private at the same time, so might not care about access-right

  // methods
  _Impl(PointType const& i_pt_from, PointType const& i_pt_to);
  template<DimensionType N>
  PointIndexType<N> Get() const;
  template<DimensionType N>
  AcrossIndexType<N> GetAcrossCoord(PointType const& i_point) const;
  AlongType GetAlongCoord(PointType const& i_point) const;

  // nested classes
  template<DimensionType N>
  struct _CurrentPointItem;
  template<DimensionType N>
  struct _PointAcrossItem;

  template<typename T>
  struct _IntegralDirection;
  template<typename T>
  struct _FloatingPointDirection;
  template<typename T, bool IsFloaingPoint>
  struct _DirectionTypeQualifier;
  template<typename T>
  struct _AcrossData;

  // data
  typename _AcrossData<AcrossType>::Type m_across_data;

  PointType const m_pt_from;
  PointType const m_pt_to;

  // TODO : remove it, in case all across directions are Floating Point. For instance, use std::tuple<>
  AlongType m_along_fractional_half_step; // indicate excess of fractional part => need to shift m_across
  AlongType m_along_fractional_step;      // value for change m_fractional_counter on each shift m_across

  AlongType m_along;
  };

///////////////////////////////////////////////////////////////////////////////
// LinearInterpolationIterator::_Impl // struct definition //
///////////////////////////////////////////////////////////////////////////////
template<typename TPointType, DimensionType NDirection>
LinearInterpolationIterator<TPointType, NDirection>::_Impl
::_Impl(PointType const& i_pt_from, PointType const& i_pt_to)
  : m_pt_from(i_pt_from)
  , m_pt_to(i_pt_to)
  {
  }

//-----------------------------------------------------------------------------
template<typename TPointType, DimensionType NDirection>
template<DimensionType N>
typename LinearInterpolationIterator<TPointType, NDirection>::PointIndexType<N>
LinearInterpolationIterator<TPointType, NDirection>::_Impl
::Get() const
  {
  return _CurrentPointItem<N>::Get<(Direction < N), (Direction > N)>(this);
  }

//-----------------------------------------------------------------------------
template<typename TPointType, DimensionType NDirection>
template<DimensionType N>
typename LinearInterpolationIterator<TPointType, NDirection>::AcrossIndexType<N>
LinearInterpolationIterator<TPointType, NDirection>::_Impl
::GetAcrossCoord(PointType const& i_point) const
  {
  return _PointAcrossItem<N>::Get<(Direction <= N)>(this, i_point);
  }

//-----------------------------------------------------------------------------
template<typename TPointType, DimensionType NDirection>
typename LinearInterpolationIterator<TPointType, NDirection>::AlongType
LinearInterpolationIterator<TPointType, NDirection>::_Impl
::GetAlongCoord(PointType const& i_point) const
  {
  return std::get<Direction>(i_point);
  }


///////////////////////////////////////////////////////////////////////////////
// LinearInterpolationIterator::_Impl::_CurrentPointItem // struct //
///////////////////////////////////////////////////////////////////////////////
template<typename TPointType, DimensionType NDirection>
template<DimensionType N>
struct LinearInterpolationIterator<TPointType, NDirection>::_Impl::_CurrentPointItem
  {
  //-----------------------------------------------------------------------------
  template<bool IsLess, bool IsGreater>
  static inline PointIndexType<N> Get(_Impl const* ip_impl);

  //-----------------------------------------------------------------------------
  //template<> static inline PointIndexType<N> Get<true, true>(_Impl const* ip_impl)
  //  {
  //  static_assert(false, "Invalid template parameters. "
  //                "It's impossible to be A<B and A>B at the same time!");
  //  }
  //-----------------------------------------------------------------------------
  template<> static inline PointIndexType<N> Get<false, false>(_Impl const* ip_impl)
    {
    return ip_impl->m_along;
    }
  //-----------------------------------------------------------------------------
  template<> static inline PointIndexType<N> Get<true, false>(_Impl const* ip_impl)
    {
    return std::get<N - 1>(ip_impl->m_across_data).m_position;
    }
  //-----------------------------------------------------------------------------
  template<> static inline PointIndexType<N> Get<false, true>(_Impl const* ip_impl)
    {
    return std::get<N>(ip_impl->m_across_data).m_position;
    }
  };


///////////////////////////////////////////////////////////////////////////////
// LinearInterpolationIterator::_Impl::_PointAcrossItem // struct //
///////////////////////////////////////////////////////////////////////////////
template<typename TPointType, DimensionType NDirection>
template<DimensionType N>
struct LinearInterpolationIterator<TPointType, NDirection>::_Impl::_PointAcrossItem
  {
  //-----------------------------------------------------------------------------
  template<bool IsLess>
  static inline AcrossIndexType<N> \
    Get(_Impl const* ip_impl, PointType const& i_point);

  //-----------------------------------------------------------------------------
  template<> static inline AcrossIndexType<N> \
    Get<true>(_Impl const* ip_impl, PointType const& i_point)
    {
    return std::get<N + 1>(i_point);
    }
  //-----------------------------------------------------------------------------
  template<> static inline AcrossIndexType<N> \
    Get<false>(_Impl const* ip_impl, PointType const& i_point)
    {
    return std::get<N>(i_point);
    }
  };


///////////////////////////////////////////////////////////////////////////////
// LinearInterpolationIterator::_Impl::_IntegralDirection // struct //
///////////////////////////////////////////////////////////////////////////////
template<typename TPointType, DimensionType NDirection>
template<typename T>
struct LinearInterpolationIterator<TPointType, NDirection>::_Impl::_IntegralDirection
  {
  //-----------------------------------------------------------------------------
  void Initialise(T i_d_across, AlongType i_d_along)
    {
    // hope that integral to integral returns the same result as std::trunc
    m_integral_part = i_d_across / i_d_along;
    m_fractional_shift = i_d_across > 0 ? 1 : -1;
    T across_fractional_half_step = std::abs(i_d_across % i_d_along);
    m_fractional_step = across_fractional_half_step * 2;
    }

  //-----------------------------------------------------------------------------
  void Reset(T i_position)
    {
    m_position = i_position;
    m_fractional_counter = 0;
    }

  //-----------------------------------------------------------------------------
  void Increment(_Impl& i_impl)
    {
    m_position += m_integral_part;
    m_fractional_counter += m_fractional_step;
    if(m_fractional_counter >= i_impl.m_along_fractional_half_step)
      {
      m_position += m_fractional_shift;
      m_fractional_counter -= i_impl.m_along_fractional_step;
      }
    }

  //-----------------------------------------------------------------------------
  void Decrement(_Impl& i_impl)
    {
    m_position -= m_integral_part;
    m_fractional_counter -= m_fractional_step;
    if(m_fractional_counter < i_impl.m_along_fractional_half_step)
      {
      m_position -= m_fractional_shift;
      m_fractional_counter += i_impl.m_along_fractional_step;
      }
    }

  char m_fractional_shift; // determine +1 or -1 depends on LineSegment direction(right or left)
  T m_fractional_step;     // value for change m_fractional_counter on each ++ or -- operation 
  T m_fractional_counter;         // current fractional part value

  // with each ++ or -- operation shift m_across on this value
  T m_integral_part;

  // current position
  T m_position;
  };


///////////////////////////////////////////////////////////////////////////////
// LinearInterpolationIterator::_Impl::_FloatingPointDirection // struct //
///////////////////////////////////////////////////////////////////////////////
template<typename TPointType, DimensionType NDirection>
template<typename T>
struct LinearInterpolationIterator<TPointType, NDirection>::_Impl::_FloatingPointDirection
  {
  //-----------------------------------------------------------------------------
  void Initialise(T i_d_across, AlongType i_d_along)
    {
    m_integral_part = i_d_across / i_d_along;
    }

  //-----------------------------------------------------------------------------
  void Reset(T i_position)
    {
    m_position = i_position;
    }

  //-----------------------------------------------------------------------------
  void Increment(_Impl& i_impl)
    {
    m_position += m_integral_part;
    }

  //-----------------------------------------------------------------------------
  void Decrement(_Impl& i_impl)
    {
    m_position -= m_integral_part;
    }

  // with each ++ or -- operation shift m_across on this value
  T m_integral_part;

  // current position
  T m_position;
  };


///////////////////////////////////////////////////////////////////////////////
// LinearInterpolationIterator::_Impl::_DirectionTypeQualifier // struct specialization //
///////////////////////////////////////////////////////////////////////////////
template<typename TPointType, DimensionType NDirection>
template<typename T>
struct LinearInterpolationIterator<TPointType, NDirection>::_Impl::_DirectionTypeQualifier<T, false>
  {
  using Type = _IntegralDirection<T>;
  };
//-----------------------------------------------------------------------------
template<typename TPointType, DimensionType NDirection>
template<typename T>
struct LinearInterpolationIterator<TPointType, NDirection>::_Impl::_DirectionTypeQualifier<T, true>
  {
  using Type = _FloatingPointDirection<T>;
  };


///////////////////////////////////////////////////////////////////////////////
// LinearInterpolationIterator::_Impl::_AcrossData // struct specialization //
///////////////////////////////////////////////////////////////////////////////
template<typename TPointType, DimensionType NDirection>
template<typename... Ts>
struct LinearInterpolationIterator<TPointType, NDirection>::_Impl::_AcrossData<std::tuple<Ts...>>
  {
  template<typename T>
  using DTQualifier = typename _DirectionTypeQualifier<T, std::is_floating_point<T>::value>::Type;

  using Type = std::tuple<DTQualifier<Ts>...>;
  };


///////////////////////////////////////////////////////////////////////////////
// LinearInterpolationIterator::_RecursiveImpl // struct //
///////////////////////////////////////////////////////////////////////////////
template<typename TPointType, DimensionType NDirection>
struct LinearInterpolationIterator<TPointType, NDirection>::_RecursiveImpl
  {
  //-----------------------------------------------------------------------------
  template<DimensionType NDirection>
  static inline void FillPoint(_Impl const& i_impl, PointType& i_point)
    {
    constexpr auto NDirectionNext = NDirection - 1;
    _RecursiveImpl::FillPoint<NDirectionNext>(i_impl, i_point);

    std::get<NDirectionNext>(i_point) = i_impl.Get<NDirectionNext>();
    }
  template<>
  static inline void FillPoint<0>(_Impl const& i_impl, PointType& i_point)
    {
    }

  //-----------------------------------------------------------------------------
  template<DimensionType NDirection>
  static inline void Initialise(_Impl& i_impl, AlongType i_d_along)
    {
    constexpr auto NDirectionNext = NDirection - 1;
    _RecursiveImpl::Initialise<NDirectionNext>(i_impl, i_d_along);

    AcrossIndexType<NDirectionNext> d_across
      = i_impl.GetAcrossCoord<NDirectionNext>(i_impl.m_pt_to)
      - i_impl.GetAcrossCoord<NDirectionNext>(i_impl.m_pt_from);

    std::get<NDirectionNext>(i_impl.m_across_data).Initialise(d_across, i_d_along);
    }
  template<>
  static inline void Initialise<0>(_Impl& i_impl, AlongType i_d_along)
    {
    i_impl.m_along_fractional_half_step = std::abs(i_d_along);
    i_impl.m_along_fractional_step = i_impl.m_along_fractional_half_step * 2;
    }

  //-----------------------------------------------------------------------------
  template<DimensionType NDirection>
  static inline void ResetIteratorToPoint(_Impl& i_impl, PointType const& i_point)
    {
    constexpr auto NDirectionNext = NDirection - 1;
    _RecursiveImpl::ResetIteratorToPoint<NDirectionNext>(i_impl, i_point);

    std::get<NDirectionNext>(i_impl.m_across_data).Reset(i_impl.GetAcrossCoord<NDirectionNext>(i_point));
    }
  template<>
  static inline void ResetIteratorToPoint<0>(_Impl& i_impl, PointType const& i_point)
    {
    i_impl.m_along = i_impl.GetAlongCoord(i_point);
    }

  //-----------------------------------------------------------------------------
  template<DimensionType NDirection>
  static inline void Increment(_Impl& i_impl)
    {
    constexpr auto NDirectionNext = NDirection - 1;
    _RecursiveImpl::Increment<NDirectionNext>(i_impl);

    std::get<NDirectionNext>(i_impl.m_across_data).Increment(i_impl);
    }
  template<>
  static inline void Increment<0>(_Impl& i_impl)
    {
    ++i_impl.m_along;
    }

  //-----------------------------------------------------------------------------
  template<DimensionType NDirection>
  static inline void Decrement(_Impl& i_impl)
    {
    constexpr auto NDirectionNext = NDirection - 1;
    _RecursiveImpl::Decrement<NDirectionNext>(i_impl);

    std::get<NDirectionNext>(i_impl.m_across_data).Decrement(i_impl);
    }
  template<>
  static inline void Decrement<0>(_Impl& i_impl)
    {
    --i_impl.m_along;
    }
  };


} // namespace Geometry
