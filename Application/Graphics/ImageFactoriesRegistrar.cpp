
#include <itkBMPImageIOFactory.h>
#include <itkPNGImageIOFactory.h>
#include <itkJPEGImageIOFactory.h>


///////////////////////////////////////////////////////////////////////////////
// ImageFactoriesRegistrar // struct //
///////////////////////////////////////////////////////////////////////////////
struct ImageFactoriesRegistrar
  {
  ImageFactoriesRegistrar()
    {
    auto registeredIOs = itk::ObjectFactoryBase::CreateAllInstance("itkImageIOBase");
    if(registeredIOs.empty())
      {
      itk::PNGImageIOFactory::RegisterOneFactory();
      itk::BMPImageIOFactory::RegisterOneFactory();
      itk::JPEGImageIOFactory::RegisterOneFactory();
      }
    }
  };

//-----------------------------------------------------------------------------
static ImageFactoriesRegistrar s_registrar;

