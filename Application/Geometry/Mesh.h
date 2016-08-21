
#pragma once

#include <array>
#include <vector>
#include <fstream>
#include <cassert>


namespace Geometry {


///////////////////////////////////////////////////////////////////////////////
// Mesh // class declaration //
///////////////////////////////////////////////////////////////////////////////
template<typename TVertexType, typename TTextureType, typename TNormalType>
class Mesh
  {
  public:
    using Index = std::array<std::size_t, 3>;
    using Face = std::tuple<Index, Index, Index>; // v, vt, vn

    Mesh(const char* i_filename);
    Mesh(Mesh&& i_rhs);

    TVertexType const& vertex(size_t i_idx) const;
    TTextureType const& texture(size_t i_idx) const;
    TNormalType const& normal(size_t i_idx) const;
    std::vector<Face> const& faces() const;

  private:
    std::vector<TVertexType>  m_vertices;
    std::vector<TTextureType> m_textures;
    std::vector<TNormalType>  m_normales;
    std::vector<Face>         m_faces;
  };


///////////////////////////////////////////////////////////////////////////////
// Mesh // class definition //
///////////////////////////////////////////////////////////////////////////////
template<typename TVertexType, typename TTextureType, typename TNormalType>
Mesh<TVertexType, TTextureType, TNormalType>::Mesh(const char* i_filename)
  {
  std::ifstream in;
  in.open(i_filename, std::ifstream::in);
  if(in.fail())
    return;
  std::string line;
  char trash;
  while(!in.eof())
    {
    std::getline(in, line);
    std::istringstream iss(line.c_str());

    if(!line.compare(0, 2, "f "))
      {
      iss >> trash;
      Face face;
      Index::value_type v_idx, t_idx, n_idx;
      std::size_t i = 0;
      while(iss >> v_idx >> trash >> t_idx >> trash >> n_idx)
        {
        // in wavefront obj all indices start at 1, not zero
        std::get<0>(face)[i] = --v_idx;
        std::get<1>(face)[i] = --t_idx;
        std::get<2>(face)[i] = --n_idx;
        ++i;
        }
      assert(i == 3);
      m_faces.emplace_back(std::move(face));
      }
    else if(!line.compare(0, 2, "v "))
      {
      iss >> trash;
      TVertexType vertex;
      for(int i = 0; i < 3; i++)
        iss >> vertex[i];
      m_vertices.emplace_back(std::move(vertex));
      }
    else if(!line.compare(0, 3, "vt "))
      {
      iss >> trash >> trash;
      TTextureType texture;
      for(int i = 0; i < 3; i++)
        iss >> texture[i];
      m_textures.emplace_back(std::move(texture));
      }
    else if(!line.compare(0, 3, "vn "))
      {
      iss >> trash >> trash;
      TNormalType nornal;
      for(int i = 0; i < 3; i++)
        iss >> nornal[i];
      m_normales.emplace_back(std::move(nornal));
      }
    }
  std::cerr << " #f = " << m_faces.size() << "; #v = " << m_vertices.size() << "; #vt " << m_textures.size() << "; #vn " << m_normales.size() << std::endl;
  }

//-----------------------------------------------------------------------------
template<typename TVertexType, typename TTextureType, typename TNormalType>
Mesh<TVertexType, TTextureType, TNormalType>::Mesh(Mesh&& i_rhs)
  : m_vertices(std::move(i_rhs.m_vertices))
  , m_textures(std::move(i_rhs.m_textures))
  , m_normales(std::move(i_rhs.m_normales))
  , m_faces(std::move(i_rhs.m_faces))
  {
  }

//-----------------------------------------------------------------------------
template<typename TVertexType, typename TTextureType, typename TNormalType>
typename TVertexType const&
Mesh<TVertexType, TTextureType, TNormalType>::vertex(size_t i_idx) const
  {
  return m_vertices[i_idx];
  }

//-----------------------------------------------------------------------------
template<typename TVertexType, typename TTextureType, typename TNormalType>
typename TTextureType const&
Mesh<TVertexType, TTextureType, TNormalType>::texture(size_t i_idx) const
  {
  return m_textures[i_idx];
  }

//-----------------------------------------------------------------------------
template<typename TVertexType, typename TTextureType, typename TNormalType>
typename TNormalType const&
Mesh<TVertexType, TTextureType, TNormalType>::normal(size_t i_idx) const
  {
  return m_normales[i_idx];
  }

//-----------------------------------------------------------------------------
template<typename TVertexType, typename TTextureType, typename TNormalType>
std::vector<typename Mesh<TVertexType, TTextureType, TNormalType>::Face> const&
Mesh<TVertexType, TTextureType, TNormalType>::faces() const
  {
  return m_faces;
  }


} // namespace Geometry
