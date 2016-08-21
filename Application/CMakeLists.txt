
function(declare_sources_directory prefix subpath)
  set(real_subpath ${subpath})
  if("${real_subpath}" STREQUAL "")
    set(real_subpath ".")
  endif()
  file(GLOB ${prefix}_SOURCES "${real_subpath}/*.h" "${real_subpath}/*.cpp")
  source_group("${subpath}" FILES ${${prefix}_SOURCES})
  set(ALL_SOURCES ${ALL_SOURCES} ${${prefix}_SOURCES} PARENT_SCOPE)
endfunction(declare_sources_directory)

declare_sources_directory(ROOT "")
declare_sources_directory(GEOMETRY "Geometry")
declare_sources_directory(GLOBAL "Global")
declare_sources_directory(GRAPHICS "Graphics")

find_package(ITK REQUIRED)
include_directories(${ITK_INCLUDE_DIRS})
link_directories(${ITK_LIBRARY_DIRS})

add_executable(app ${ALL_SOURCES})

target_link_libraries(app ${ITK_LIBRARIES})