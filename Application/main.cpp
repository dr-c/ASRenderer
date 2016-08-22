
#include "./Config.h"

#include "./Geometry/Mesh.h"
#include "./Geometry/Matrix.h"
#include "./Graphics/Canvas.h"

#include <string>
#include <chrono>

int main(int i_argc, char** i_argv)
  {
  using WorldPoint = Geometry::Point<float, 3>;
  using ScreenPoint = Geometry::Point<int, 3>;
  using TexturePoint = Geometry::Point<float, 3>;
  using Vector = Geometry::Vector<float, 3>;
  using TransformMatrix = Geometry::Matrix<float, 4, 4>;
  using Mesh = Geometry::Mesh<WorldPoint, TexturePoint, Vector>;
  using Canvas = Graphics::Canvas;
  using Image = Canvas::Image;

  std::string source_dir = PROJECT_SOURCE_DIR;

  int width = 1024;
  int height = 1024;
  int depth = 10000;

  auto input_filename = source_dir + "/_inputs/african_head.obj";
  Mesh mesh(input_filename.c_str());
  Canvas canvas(width, height);

  auto texture_filename = source_dir + "/_inputs/african_head_diffuse.png";
  Image texture_image;
  texture_image.Read(texture_filename.c_str());
  texture_image.FlipVertically();
  canvas.SetTextureImage(std::move(texture_image));

  int const half_width = width >> 1;
  int const half_height = height >> 1;
  int const half_depth = height >> 1;

  TransformMatrix tranform_matrix;
  tranform_matrix.MakeIdentity();
  tranform_matrix(3, 2) = -0.2f;

  auto world_to_screen = [half_width, half_height, half_depth, &tranform_matrix](WorldPoint const& i_v)
    {
    auto v0 = tranform_matrix * Geometry::Vector<float, 4>(i_v[0], i_v[1], i_v[2], 1.f);
    v0 /= v0[3];

    auto const x = (v0[0] * half_width * 0.8) + half_width;
    auto const y = (v0[1] * half_height * 0.8) + half_height;
    auto const z = (v0[2] * half_depth * 0.8) + half_depth;
    //return ScreenPoint(x, y, z);

    return ScreenPoint(static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));
    };

  Vector camera_direction(0.f, 0.f, -1.f);
  camera_direction.Normalise();
  Vector light_direction(0.f, -0.5f, 1.f);
  light_direction.Normalise();
  canvas.SetLightDirection(light_direction);

  auto t1 = std::chrono::high_resolution_clock::now();

  for(auto const& face : mesh.faces())
    {
    auto const& vertex_indices = std::get<0>(face);
    auto const world_v0 = mesh.vertex(vertex_indices[0]);
    auto const world_v1 = mesh.vertex(vertex_indices[1]);
    auto const world_v2 = mesh.vertex(vertex_indices[2]);

    auto const& texture_indices = std::get<1>(face);
    auto const texture_v0 = mesh.texture(texture_indices[0]);
    auto const texture_v1 = mesh.texture(texture_indices[1]);
    auto const texture_v2 = mesh.texture(texture_indices[2]);

    auto const& normal_indices = std::get<2>(face);
    auto const normal_v0 = mesh.normal(normal_indices[0]);
    auto const normal_v1 = mesh.normal(normal_indices[1]);
    auto const normal_v2 = mesh.normal(normal_indices[2]);

    auto world_normal = (world_v2 - world_v0) ^ (world_v1 - world_v0);
    world_normal.Normalise();
    auto intensity = camera_direction * world_normal;
    if(intensity <= 0)
      continue;

    auto screen_v0 = world_to_screen(world_v0);
    auto screen_v1 = world_to_screen(world_v1);
    auto screen_v2 = world_to_screen(world_v2);

    //canvas.DrawFilledTrianglePhong(screen_v0, screen_v1, screen_v2, normal_v0, normal_v1, normal_v2);

    //static auto c = 0;
    //if(++c > 10)
    //  break;

    //Canvas::Color color;
    //for(std::size_t i = 0; i < 3; ++i)
    //  color[i] = static_cast<Canvas::Color::ComponentType>(255 * intensity);
    //canvas.DrawFilledTriangle(screen_v0, screen_v1, screen_v2, color);

    //canvas.DrawFilledTriangle(screen_v0, screen_v1, screen_v2, texture_v0, texture_v1, texture_v2, intensity);
    canvas.DrawFilledTriangleGouraud(screen_v0, screen_v1, screen_v2, texture_v0, texture_v1, texture_v2, normal_v0, normal_v1, normal_v2);
    }

  auto t2 = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
  std::cout << duration;
  //system("PAUSE");

  auto& image = canvas.GetImage();
  image.FlipVertically(); // i want to have the origin at the left bottom corner of the image
  auto output_filename = source_dir + "/_outputs/head.png";
  image.Write(output_filename.c_str());

  return 0;
  }

