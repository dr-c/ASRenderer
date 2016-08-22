
#include "./Canvas.h"

#include "./../Geometry/LinearInterpolationIterator.h"

#include <algorithm>

template<typename TPointType, DimensionType NDirection>
using LIIterator = Geometry::LinearInterpolationIterator<TPointType, NDirection>;

template<typename TPointType>
using DrawHLineIterator = LIIterator<typename std::RemoveTypeByIndex<TPointType, 1>::Type, 0>;

namespace Graphics {


//-----------------------------------------------------------------------------
Canvas::Canvas(DimensionType i_w, DimensionType i_h)
  : m_image(i_w, i_h, false)
  , m_texture_image()
  , m_z_buffer(i_w, i_h, false)
  {
  Color black;
  black.Fill(0);
  m_image.Fill(black);
  m_z_buffer.Fill(std::numeric_limits<Buffer::PixelType>::min());
  }

//-----------------------------------------------------------------------------
Canvas::Image&
Canvas::GetImage()
  {
  return m_image;
  }

//-----------------------------------------------------------------------------
Canvas::Image const&
Canvas::GetImage() const
  {
  return m_image;
  }

//-----------------------------------------------------------------------------
void
Canvas::SetTextureImage(Image&& i_img)
  {
  m_texture_image = std::move(i_img);
  }

//-----------------------------------------------------------------------------
void
Canvas::SetLightDirection(Normal const& i_light_direction)
  {
  m_light_direction = i_light_direction;
  }

//-----------------------------------------------------------------------------
bool
Canvas::_Set(int i_x, int i_y, int i_z, Color const& i_color)
  {
  if(static_cast<unsigned int>(i_x) >= m_image.GetWidth()
     || static_cast<unsigned int>(i_y) >= m_image.GetHeight())
    return false;

  auto z_item = m_z_buffer.Get(i_x, i_y);
  if(i_z < z_item)
    return false;

  m_image.Set(i_x, i_y, i_color);
  m_z_buffer.Set(i_x, i_y, i_z);
  return true;
  }

//-----------------------------------------------------------------------------
bool
Canvas::_Set(Point const& i_pt, Color const& i_color)
  {
  return _Set(std::get<0>(i_pt), std::get<1>(i_pt), std::get<2>(i_pt), i_color);
  }

//-----------------------------------------------------------------------------
Canvas::Color
Canvas::_GetColorFromTexture(int i_x, int i_y, float i_intensity)
  {
  auto& texture_color = m_texture_image.Get(i_x, i_y);
  Color color;
  for(std::size_t i = 0; i < 3; ++i)
    color[i] = static_cast<Color::ComponentType>(texture_color[i] * i_intensity);
  return color;
  }

//-----------------------------------------------------------------------------
Canvas::Normal::ValueType
Canvas::_GetIntensityFromNormal(Normal const& i_normal)
  {
  auto intensity = m_light_direction * i_normal;
  if(intensity < 0)
    return 0.;
  return intensity;
  }

//-----------------------------------------------------------------------------
Canvas::Color
Canvas::_GetGrayColorFromIntensity(int i_intensity)
  {
  Color color;
  for(std::size_t i = 0; i < 3; ++i)
    color[i] = static_cast<Color::ComponentType>(i_intensity);
  return color;
  }

//-----------------------------------------------------------------------------
void
Canvas::DrawLine(Point i_pt1, Point i_pt2, Color const& i_color)
  {
  int const dx = std::get<0>(i_pt2) - std::get<0>(i_pt1);
  int const dy = std::get<1>(i_pt2) - std::get<1>(i_pt1);
  int const abs_dx = abs(dx);
  int const abs_dy = abs(dy);

  if((abs_dx >= abs_dy && dx < 0) || (abs_dx < abs_dy && dy < 0))
    std::swap(i_pt1, i_pt2);

  if(abs_dx >= abs_dy)
    {
    LIIterator<Point, 0> it_line(i_pt1, i_pt2);
    for(it_line.GoToBegin(); ; ++it_line)
      {
      _Set(*it_line, i_color);
      if(it_line.IsAtEnd())
        break;
      }
    }
  else
    {
    LIIterator<Point, 1> it_line(i_pt1, i_pt2);
    for(it_line.GoToBegin(); ; ++it_line)
      {
      _Set(*it_line, i_color);
      if(it_line.IsAtEnd())
        break;
      }
    }
  }

//-----------------------------------------------------------------------------
void
Canvas::DrawTriangle(Point const& i_pt1, Point const& i_pt2, Point const& i_pt3, Color const& i_color)
  {
  DrawLine(i_pt1, i_pt2, i_color);
  DrawLine(i_pt2, i_pt3, i_color);
  DrawLine(i_pt3, i_pt1, i_color);
  }

//-----------------------------------------------------------------------------
void
Canvas::DrawFilledTriangle(Point const& i_pt1, Point const& i_pt2, Point const& i_pt3, Color const& i_color)
  {
  auto f = [&i_color](DrawHLineIterator<Point> const& i_iter)
    {
    (void) i_iter;
    return i_color;
    };

  _DrawFilledTriangle(&i_pt1, &i_pt2, &i_pt3, f);
  }

//-----------------------------------------------------------------------------
void
Canvas::DrawFilledTriangle(Point const& i_pt1, Point const& i_pt2, Point const& i_pt3,
                           TexturePoint const& i_tx1, TexturePoint const& i_tx2, TexturePoint const& i_tx3, float i_intensity)
  {
  using PointWithTexture = Geometry::Point<int, 5>;

  PointWithTexture pt1(std::get<0>(i_pt1), std::get<1>(i_pt1), std::get<2>(i_pt1), 
                       static_cast<int>(std::get<0>(i_tx1) * m_texture_image.GetWidth()),
                       static_cast<int>(std::get<1>(i_tx1) * m_texture_image.GetHeight()));
  PointWithTexture pt2(std::get<0>(i_pt2), std::get<1>(i_pt2), std::get<2>(i_pt2),
                       static_cast<int>(std::get<0>(i_tx2) * m_texture_image.GetWidth()),
                       static_cast<int>(std::get<1>(i_tx2) * m_texture_image.GetHeight()));
  PointWithTexture pt3(std::get<0>(i_pt3), std::get<1>(i_pt3), std::get<2>(i_pt3),
                       static_cast<int>(std::get<0>(i_tx3) * m_texture_image.GetWidth()),
                       static_cast<int>(std::get<1>(i_tx3) * m_texture_image.GetHeight()));

  auto f = [this, i_intensity](DrawHLineIterator<PointWithTexture> const& i_iter)
    {
    auto texture_x = i_iter.Get<2>();
    auto texture_y = i_iter.Get<3>();
    return _GetColorFromTexture(texture_x, texture_y, i_intensity);
    };

  _DrawFilledTriangle(&pt1, &pt2, &pt3, f);
  }

//-----------------------------------------------------------------------------
void
Canvas::DrawFilledTriangleGouraud(Point const& i_pt1, Point const& i_pt2, Point const& i_pt3,
                                  Normal const& i_n1, Normal const& i_n2, Normal const& i_n3)
  {
  using PointWithIntensity = std::tuple<Point::ValueType, Point::ValueType, Point::ValueType, Normal::ValueType>;

  PointWithIntensity pt1(std::get<0>(i_pt1), std::get<1>(i_pt1), std::get<2>(i_pt1), _GetIntensityFromNormal(i_n1));
  PointWithIntensity pt2(std::get<0>(i_pt2), std::get<1>(i_pt2), std::get<2>(i_pt2), _GetIntensityFromNormal(i_n2));
  PointWithIntensity pt3(std::get<0>(i_pt3), std::get<1>(i_pt3), std::get<2>(i_pt3), _GetIntensityFromNormal(i_n3));

  auto f = [this](DrawHLineIterator<PointWithIntensity> const& i_iter)
    {
    auto intensity = i_iter.Get<2>();
    Color color;
    for(std::size_t i = 0; i < 3; ++i)
      color[i] = intensity * 255;
    return color;
    };

  _DrawFilledTriangle(&pt1, &pt2, &pt3, f);
  }

//-----------------------------------------------------------------------------
void
Canvas::DrawFilledTrianglePhong(Point const& i_pt1, Point const& i_pt2, Point const& i_pt3,
                                Normal const& i_n1, Normal const& i_n2, Normal const& i_n3)
  {
  using PointWithNormal = std::tuple<Point::ValueType, Point::ValueType, Point::ValueType,
                                     Normal::ValueType, Normal::ValueType, Normal::ValueType>;

  PointWithNormal pt1(std::get<0>(i_pt1), std::get<1>(i_pt1), std::get<2>(i_pt1),
                      std::get<0>(i_n1), std::get<1>(i_n1), std::get<2>(i_n1));
  PointWithNormal pt2(std::get<0>(i_pt2), std::get<1>(i_pt2), std::get<2>(i_pt2),
                      std::get<0>(i_n2), std::get<1>(i_n2), std::get<2>(i_n2));
  PointWithNormal pt3(std::get<0>(i_pt3), std::get<1>(i_pt3), std::get<2>(i_pt3),
                      std::get<0>(i_n3), std::get<1>(i_n3), std::get<2>(i_n3));

  auto f = [this](DrawHLineIterator<PointWithNormal> const& i_iter)
    {
    Normal normal(i_iter.Get<2>(), i_iter.Get<3>(), i_iter.Get<4>());
    auto intensity = _GetIntensityFromNormal(normal);
    Color color;
    for(std::size_t i = 0; i < 3; ++i)
      color[i] = intensity * 255;
    return color;
    };

  _DrawFilledTriangle(&pt1, &pt2, &pt3, f);
  }

//-----------------------------------------------------------------------------
void
Canvas::DrawFilledTriangleGouraud(Point const& i_pt1, Point const& i_pt2, Point const& i_pt3,
                                  TexturePoint const& i_tx1, TexturePoint const& i_tx2, TexturePoint const& i_tx3,
                                  Normal const& i_n1, Normal const& i_n2, Normal const& i_n3)
  {
  using PointWithTextureAndIntensity = std::tuple<Point::ValueType, Point::ValueType, Point::ValueType, 
    int, int, Normal::ValueType>;

  PointWithTextureAndIntensity pt1(std::get<0>(i_pt1), std::get<1>(i_pt1), std::get<2>(i_pt1),
                                static_cast<int>(std::get<0>(i_tx1) * m_texture_image.GetWidth()),
                                static_cast<int>(std::get<1>(i_tx1) * m_texture_image.GetHeight()),
                                _GetIntensityFromNormal(i_n1));
  PointWithTextureAndIntensity pt2(std::get<0>(i_pt2), std::get<1>(i_pt2), std::get<2>(i_pt2),
                                static_cast<int>(std::get<0>(i_tx2) * m_texture_image.GetWidth()),
                                static_cast<int>(std::get<1>(i_tx2) * m_texture_image.GetHeight()),
                                _GetIntensityFromNormal(i_n2));
  PointWithTextureAndIntensity pt3(std::get<0>(i_pt3), std::get<1>(i_pt3), std::get<2>(i_pt3),
                                static_cast<int>(std::get<0>(i_tx3) * m_texture_image.GetWidth()),
                                static_cast<int>(std::get<1>(i_tx3) * m_texture_image.GetHeight()),
                                _GetIntensityFromNormal(i_n3));

  auto f = [this](DrawHLineIterator<PointWithTextureAndIntensity> const& i_iter)
    {
    auto texture_x = i_iter.Get<2>();
    auto texture_y = i_iter.Get<3>();
    auto intensity = i_iter.Get<4>();
    return _GetColorFromTexture(texture_x, texture_y, intensity);
    };

  _DrawFilledTriangle(&pt1, &pt2, &pt3, f);
  }

//-----------------------------------------------------------------------------
void
Canvas::DrawFilledTrianglePhong(Point const& i_pt1, Point const& i_pt2, Point const& i_pt3,
                                TexturePoint const& i_tx1, TexturePoint const& i_tx2, TexturePoint const& i_tx3,
                                Normal const& i_n1, Normal const& i_n2, Normal const& i_n3)
  {
  using PointWithTextureAndNormal = std::tuple<Point::ValueType, Point::ValueType, Point::ValueType, 
    int, int, Normal::ValueType, Normal::ValueType, Normal::ValueType>;

  PointWithTextureAndNormal pt1(std::get<0>(i_pt1), std::get<1>(i_pt1), std::get<2>(i_pt1),
                                static_cast<int>(std::get<0>(i_tx1) * m_texture_image.GetWidth()),
                                static_cast<int>(std::get<1>(i_tx1) * m_texture_image.GetHeight()),
                                std::get<0>(i_n1), std::get<1>(i_n1), std::get<2>(i_n1));
  PointWithTextureAndNormal pt2(std::get<0>(i_pt2), std::get<1>(i_pt2), std::get<2>(i_pt2),
                                static_cast<int>(std::get<0>(i_tx2) * m_texture_image.GetWidth()),
                                static_cast<int>(std::get<1>(i_tx2) * m_texture_image.GetHeight()),
                                std::get<0>(i_n2), std::get<1>(i_n2), std::get<2>(i_n2));
  PointWithTextureAndNormal pt3(std::get<0>(i_pt3), std::get<1>(i_pt3), std::get<2>(i_pt3),
                                static_cast<int>(std::get<0>(i_tx3) * m_texture_image.GetWidth()),
                                static_cast<int>(std::get<1>(i_tx3) * m_texture_image.GetHeight()),
                                std::get<0>(i_n3), std::get<1>(i_n3), std::get<2>(i_n3));

  auto f = [this](DrawHLineIterator<PointWithTextureAndNormal> const& i_iter)
    {
    auto texture_x = i_iter.Get<2>();
    auto texture_y = i_iter.Get<3>();
    Normal normal(i_iter.Get<4>(), i_iter.Get<5>(), i_iter.Get<6>());
    auto intensity = _GetIntensityFromNormal(normal);
    return _GetColorFromTexture(texture_x, texture_y, intensity);
    };

  _DrawFilledTriangle(&pt1, &pt2, &pt3, f);
  }

//-----------------------------------------------------------------------------
template<typename TPoint, typename F>
void Canvas::_DrawHLine(TPoint const& i_pt1, TPoint const& i_pt2, F i_color_getter)
  {
  static_assert(std::tuple_size<TPoint>::value >= 3, "_DrawHLine is possible only for 3D+ points");

  assert(std::get<0>(i_pt1) <= std::get<0>(i_pt2));
  assert(std::get<1>(i_pt1) == std::get<1>(i_pt2));

  // Small optimization. Because of it is Horizontal Line, Y is const. 
  // No sence to iterate over it. Remember it and iterate in N-1 direction.
  int const y = std::get<1>(i_pt1);
  auto shrinked_pt1 = std::RemoveItem<1>(i_pt1);
  auto shrinked_pt2 = std::RemoveItem<1>(i_pt2);
  DrawHLineIterator<TPoint> it_line(shrinked_pt1, shrinked_pt2);
  for(it_line.GoToBegin(); ; ++it_line)
    {
    _Set(it_line.Get<0>(), y, it_line.Get<1>(), i_color_getter(it_line));
    if(it_line.IsAtEnd())
      break;
    }
  }

//-----------------------------------------------------------------------------
template<typename TPoint, typename F>
void
Canvas::_DrawFilledTriangle(TPoint const* ip_pt1, TPoint const* ip_pt2, TPoint const* ip_pt3, F i_color_getter)
  {
  _Sort3PointsInDirection<1>(ip_pt1, ip_pt2, ip_pt3);

  if(std::get<1>(*ip_pt1) == std::get<1>(*ip_pt3)) // All 3 points lies on a horizontal line. Draw the line
    {
    _Sort3PointsInDirection<0>(ip_pt1, ip_pt2, ip_pt3);

    if(std::get<0>(*ip_pt1) == std::get<0>(*ip_pt3)) // All 3 points merges to a point. Draw the point
      {
      _Sort3PointsInDirection<2>(ip_pt1, ip_pt2, ip_pt3);
      _DrawHLine(*ip_pt3, *ip_pt3, i_color_getter);
      return;
      }

    // Lenaer equation of line 13: (Z13<X>-z1)/(z3-z1) == (X-x1)/(x3-x1)
    // Z13<X> == z1+dz13*(X-x1)/dx13  =>  Z13<x2> == z1+dz13*dx12/dx13
    int const z13_x2 = std::get<2>(*ip_pt1) + (std::get<2>(*ip_pt3) - std::get<2>(*ip_pt1))
      * (std::get<0>(*ip_pt2) - std::get<0>(*ip_pt1)) / (std::get<0>(*ip_pt3) - std::get<0>(*ip_pt1));
    if(z13_x2 >= std::get<2>(*ip_pt2)) // No sence to draw 2 another lines. This line is upper by Z
      {
      _DrawHLine(*ip_pt1, *ip_pt3, i_color_getter);
      }
    else // No sence to draw another line. These 2 lines are upper by Z
      {
      _DrawHLine(*ip_pt1, *ip_pt2, i_color_getter);
      _DrawHLine(*ip_pt2, *ip_pt3, i_color_getter);
      }
    return;
    }

  int const dx12 = std::get<0>(*ip_pt2) - std::get<0>(*ip_pt1);
  int const dx13 = std::get<0>(*ip_pt3) - std::get<0>(*ip_pt1);
  int const dy12 = std::get<1>(*ip_pt2) - std::get<1>(*ip_pt1);
  int const dy13 = std::get<1>(*ip_pt3) - std::get<1>(*ip_pt1);

  // Draw lines from left to right
  if(dx12 * dy13 < dx13 * dy12) // 12 and 23 - left / 13 - right
    {
    LIIterator<TPoint, 1> it_line_left_bottom(*ip_pt1, *ip_pt2); it_line_left_bottom.GoToBegin();
    LIIterator<TPoint, 1> it_line_left_top(*ip_pt2, *ip_pt3);    it_line_left_top.GoToBegin();
    LIIterator<TPoint, 1> it_line_right(*ip_pt1, *ip_pt3);       it_line_right.GoToBegin();

    for(; !it_line_left_bottom.IsAtEnd(); ++it_line_left_bottom, ++it_line_right)
      _DrawHLine(*it_line_left_bottom, *it_line_right, i_color_getter);
    for(; ; ++it_line_left_top, ++it_line_right)
      {
      _DrawHLine(*it_line_left_top, *it_line_right, i_color_getter);
      if(it_line_left_top.IsAtEnd())
        break;
      }
    }
  else // 13 - left / 12 and 23 - right
    {
    LIIterator<TPoint, 1> it_line_right_bottom(*ip_pt1, *ip_pt2); it_line_right_bottom.GoToBegin();
    LIIterator<TPoint, 1> it_line_right_top(*ip_pt2, *ip_pt3);    it_line_right_top.GoToBegin();
    LIIterator<TPoint, 1> it_line_left(*ip_pt1, *ip_pt3);         it_line_left.GoToBegin();

    for(; !it_line_right_bottom.IsAtEnd(); ++it_line_left, ++it_line_right_bottom)
      _DrawHLine(*it_line_left, *it_line_right_bottom, i_color_getter);
    for(; ; ++it_line_left, ++it_line_right_top)
      {
      _DrawHLine(*it_line_left, *it_line_right_top, i_color_getter);
      if(it_line_right_top.IsAtEnd())
        break;
      }
    }
  }

//-----------------------------------------------------------------------------
template<DimensionType NDirection, typename TPoint>
void
Canvas::_Sort3PointsInDirection(TPoint const*& ip_pt1, TPoint const*& ip_pt2, TPoint const*& ip_pt3)
  {
  if(std::get<NDirection>(*ip_pt1) > std::get<NDirection>(*ip_pt2))
    std::swap(ip_pt1, ip_pt2);
  if(std::get<NDirection>(*ip_pt1) > std::get<NDirection>(*ip_pt3))
    std::swap(ip_pt1, ip_pt3);
  if(std::get<NDirection>(*ip_pt2) > std::get<NDirection>(*ip_pt3))
    std::swap(ip_pt2, ip_pt3);
  }


} // namespace Graphics
