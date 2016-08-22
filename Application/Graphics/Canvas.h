
#pragma once

#include "./Image.h"
#include "./../Geometry/Vector.h"
#include "./../Geometry/Point.h"

#include <itkRGBPixel.h>


namespace Graphics {


///////////////////////////////////////////////////////////////////////////////
// Canvas // class declaration //
///////////////////////////////////////////////////////////////////////////////
class Canvas
  {
  public:
    using Self = Canvas;
    
    using Color = itk::RGBPixel<unsigned char>;
    using Image = Graphics::Image<Color>;
    using Buffer = Graphics::Image<int>;

    using Point = Geometry::Point<int, 3>;
    using Vector = Geometry::Vector<int, 3>;
    using Normal = Geometry::Vector<float, 3>;
    using TexturePoint = Geometry::Point<float, 3>;

    Canvas(DimensionType i_w, DimensionType i_h);

    Image& GetImage();
    Image const& GetImage() const;
    void SetTextureImage(Image&& i_img);
    void SetLightDirection(Normal const& i_light_direction);

    void DrawLine(Point i_pt1, Point i_pt2, Color const& i_color);
    void DrawTriangle(Point const& i_pt1, Point const& i_pt2, Point const& i_pt3, Color const& i_color);
    void DrawFilledTriangle(Point const& i_pt1, Point const& i_pt2, Point const& i_pt3, Color const& i_color);
    void DrawFilledTriangle(Point const& i_pt1, Point const& i_pt2, Point const& i_pt3,
                            TexturePoint const& i_tx1, TexturePoint const& i_tx2, TexturePoint const& i_tx3, float i_intensity);
    void DrawFilledTriangleGouraud(Point const& i_pt1, Point const& i_pt2, Point const& i_pt3,
                                   Normal const& i_n1, Normal const& i_n2, Normal const& i_n3);
    void DrawFilledTrianglePhong(Point const& i_pt1, Point const& i_pt2, Point const& i_pt3,
                                Normal const& i_n1, Normal const& i_n2, Normal const& i_n3);
    void DrawFilledTriangleGouraud(Point const& i_pt1, Point const& i_pt2, Point const& i_pt3,
                                   TexturePoint const& i_tx1, TexturePoint const& i_tx2, TexturePoint const& i_tx3,
                                   Normal const& i_n1, Normal const& i_n2, Normal const& i_n3);
    void DrawFilledTrianglePhong(Point const& i_pt1, Point const& i_pt2, Point const& i_pt3,
                                 TexturePoint const& i_tx1, TexturePoint const& i_tx2, TexturePoint const& i_tx3,
                                 Normal const& i_n1, Normal const& i_n2, Normal const& i_n3);

  protected:
    bool _Set(int i_x, int i_y, int i_z, Color const& i_color);
    bool _Set(Point const& i_pt, Color const& i_color);

    Color _GetColorFromTexture(int i_x, int i_y, float i_intensity);
    Normal::ValueType _GetIntensityFromNormal(Normal const& i_normal);
    Color _GetGrayColorFromIntensity(int i_intensity);

    template<typename TPoint, typename F>
    void _DrawHLine(TPoint const& i_pt1, TPoint const& i_pt2, F i_color_getter);

    template<typename TPoint, typename F>
    void _DrawFilledTriangle(TPoint const* ip_pt1, TPoint const* ip_pt2, TPoint const* ip_pt3, F i_color_getter);

    template<DimensionType NDirection, typename TPoint>
    static void _Sort3PointsInDirection(TPoint const*& ip_pt1, TPoint const*& ip_pt2, TPoint const*& ip_pt3);

  private:
    Image m_image;
    Image m_texture_image;
    Buffer m_z_buffer;
    Normal m_light_direction;
  };


} // namespace Graphics
