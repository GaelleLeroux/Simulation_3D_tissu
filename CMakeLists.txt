cmake_minimum_required(VERSION 3.10)

project(simulation LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)
find_package(OpenGL REQUIRED)


find_package(GLEW)
if(NOT ${GLEW_FOUND} OR APPLE)

  message(WARNING "Glew not found, manual compilation of glew added to the project")
  
  option(glew-cmake_BUILD_SHARED "Build the shared glew library" OFF)
  option(ONLY_LIBS "Do not build executables" ON)

  add_subdirectory(extern/glew)
  include_directories(${PROJECT_PATH}/extern/glew/include)
  set(USED_GLEW_LIBRARY libglew_static)
else()
  include_directories(${GLEW_INCLUDE_DIRS})
  set(USED_GLEW_LIBRARY ${GLEW_LIBRARIES})
endif()

# find_package(GLUT)
# if(NOT ${GLUT_FOUND})

#   message(WARNING "Glut not found, manual compilation of Glut added to the project")

#   option(FREEGLUT_BUILD_SHARED_LIBS "Build FreeGLUT shared library." OFF)
#   option(FREEGLUT_BUILD_DEMOS "Build FreeGLUT demos." OFF )
#   option(FREEGLUT_PRINT_ERRORS "Lib prints errors to stderr" OFF)
#   option(FREEGLUT_PRINT_WARNINGS "Lib prints warnings to stderr" OFF)
#   option(INSTALL_PDB "Also install .pdb files" OFF)

#   add_subdirectory(extern/freeglut)
#   include_directories(${CMAKE_SOURCE_DIR}/extern/freeglut/include)
#   set(USED_GLUT_LIBRARY freeglut_static)
# else()
#   include_directories(${GLUT_INCLUDE_DIRS})
#   set(USED_GLUT_LIBRARY ${GLUT_LIBRARIES})
# endif()

include_directories(${CMAKE_SOURCE_DIR}/extern)

# add_subdirectory(${CMAKE_SOURCE_DIR}/src/lib)
# include_directories(${CMAKE_SOURCE_DIR}/src/lib)

foreach(dir_n RANGE 1 2)
  set(dir "programme_${dir_n}")

  message("Programme ${dir}" )

  file(GLOB_RECURSE source_files "src/${dir}/*.cpp")
  file(GLOB_RECURSE header_files "src/${dir}/*.h" "src/${dir}/*.hpp") 
  file(GLOB_RECURSE shader_files "shaders/*.vert" "shaders/*.geom" "shaders/*.frag" "shaders/*.comp")



message("Source files")
foreach(line IN LISTS source_files)
    message("${line}")
endforeach() 

  message("Header files")
foreach(line IN LISTS header_files)
    message("${line}")
endforeach() 

message("Shader files")
foreach(line IN LISTS shader_files)
    message("${line}")
endforeach() 

if (WIN32)
    set(LINK_FLAGS )
  elseif (APPLE)
    set(LINK_FLAGS )
  elseif (UNIX)
    set(LINK_FLAGS )
    add_definitions(-g -Wall -Wextra -Wno-comment -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -DCORRECTION)
  endif ()

  add_executable(${dir} ${source_files} ${header_files} ${shader_files})
  target_link_libraries(${dir} ${USED_GLEW_LIBRARY} ${USED_GLUT_LIBRARY} ${OPENGL_LIBRARIES} Qt5::Widgets Qt5::OpenGL ${LINK_FLAGS_PERSO})
  set_target_properties(${dir} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endforeach()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT programme_1)
