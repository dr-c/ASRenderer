
#pragma once

#include "./../Geometry/BaseTypedefs.h"

#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkImageFileReader.h>
#include <itkFlipImageFilter.h>


namespace Graphics {


///////////////////////////////////////////////////////////////////////////////
// Image // class declaration //
///////////////////////////////////////////////////////////////////////////////
template<typename TPixel>
class Image
  {
  public:
    static constexpr DimensionType Dimension = 2;

    using PixelType = TPixel;
    using Self = Image<PixelType>;
    using ItkImage = itk::Image<PixelType, Dimension>;

    Image();
    Image(DimensionType i_x, DimensionType i_y, bool i_initialise = true);
    Image(Image const& i_another_image);
    Image(Image&& i_another_image);
    ~Image();

    Image& operator=(Image const& i_another_image);
    Image& operator=(Image&& i_another_image);

    void Read(const char* i_filename);
    void Write(const char* i_filename);

    PixelType const& Get(DimensionType i_x, DimensionType i_y) const;
    void Set(DimensionType i_x, DimensionType i_y, PixelType const& i_c);

    DimensionType GetWidth();
    DimensionType GetHeight();

    void Fill(PixelType const& i_value);

    void FlipVertically();

  private:
    typename ItkImage::Pointer mp_image;
  };

///////////////////////////////////////////////////////////////////////////////
// Image // class definition //
///////////////////////////////////////////////////////////////////////////////
template<typename TPixel>
Image<TPixel>::Image()
  : mp_image()
  {
  }

//-----------------------------------------------------------------------------
template<typename TPixel>
Image<TPixel>::Image(DimensionType i_x, DimensionType i_y, bool i_initialise)
  : mp_image(ItkImage::New())
  {
  ItkImage::SizeType size;
  size[0] = i_x;
  size[1] = i_y;
  ItkImage::RegionType region;
  region.SetSize(size);
  mp_image->SetRegions(region);
  mp_image->Allocate(i_initialise);
  }

//-----------------------------------------------------------------------------
template<typename TPixel>
Image<TPixel>::Image(Image const& i_another_image)
  : mp_image(i_another_image.mp_image)
  {
  }

//-----------------------------------------------------------------------------
template<typename TPixel>
Image<TPixel>::Image(Image&& i_another_image)
  : mp_image(i_another_image.mp_image)
  {
  i_another_image.mp_image = nullptr;
  }

//-----------------------------------------------------------------------------
template<typename TPixel>
Image<TPixel>::~Image()
  {
  }

//-----------------------------------------------------------------------------
template<typename TPixel>
Image<TPixel>& 
Image<TPixel>::operator=(Image const& i_another_image)
  {
  mp_image = i_another_image.mp_image;
  return *this;
  }

//-----------------------------------------------------------------------------
template<typename TPixel>
Image<TPixel>&
Image<TPixel>::operator=(Image&& i_another_image)
  {
  mp_image = i_another_image.mp_image;
  i_another_image.mp_image = nullptr;
  return *this;
  }

//-----------------------------------------------------------------------------
template<typename TPixel>
void
Image<TPixel>::Read(const char* i_filename)
  {
  using ReaderType = itk::ImageFileReader<ItkImage>;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(i_filename);
  reader->Update();
  mp_image = reader->GetOutput();
  }

//-----------------------------------------------------------------------------
template<typename TPixel>
void
Image<TPixel>::Write(const char* i_filename)
  {
  using WriterType = itk::ImageFileWriter<ItkImage>;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(i_filename);
  writer->SetInput(mp_image);
  writer->Update();
  }

//-----------------------------------------------------------------------------
template<typename TPixel>
typename Image<TPixel>::PixelType const&
Image<TPixel>::Get(DimensionType i_x, DimensionType i_y) const
  {
  ItkImage::IndexType index;
  index[0] = i_x;
  index[1] = i_y;
  return mp_image->GetPixel(index);
  }

//-----------------------------------------------------------------------------
template<typename TPixel>
void Image<TPixel>::Set(DimensionType i_x, DimensionType i_y, PixelType const& i_c)
  {
  ItkImage::IndexType index;
  index[0] = i_x;
  index[1] = i_y;
  mp_image->SetPixel(index, i_c);
  }

//-----------------------------------------------------------------------------
template<typename TPixel>
DimensionType
Image<TPixel>::GetWidth()
  {
  return mp_image->GetLargestPossibleRegion().GetSize()[0];
  }

//-----------------------------------------------------------------------------
template<typename TPixel>
DimensionType
Image<TPixel>::GetHeight()
  {
  return mp_image->GetLargestPossibleRegion().GetSize()[1];
  }

//-----------------------------------------------------------------------------
template<typename TPixel>
void
Image<TPixel>::Fill(PixelType const& i_value)
  {
  mp_image->FillBuffer(i_value);
  }

//-----------------------------------------------------------------------------
template<typename TPixel>
void
Image<TPixel>::FlipVertically()
  {
  using FlipImageFilterType = itk::FlipImageFilter<ItkImage>;

  FlipImageFilterType::Pointer flipFilter = FlipImageFilterType::New();
  flipFilter->SetInput(mp_image);

  FlipImageFilterType::FlipAxesArrayType flipAxes;
  flipAxes[0] = false;
  flipAxes[1] = true;
  flipFilter->SetFlipAxes(flipAxes);
  
  flipFilter->Update();

  mp_image = flipFilter->GetOutput();
  }


} // namespace Graphics
