# Install script for directory: C:/Tyffon/Repositories/expoc-test/donut_examples

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/donut_examples")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Tyffon/Repositories/expoc-test/donut_examples/build/donut/cmake_install.cmake")
  include("C:/Tyffon/Repositories/expoc-test/donut_examples/build/feature_demo/cmake_install.cmake")
  include("C:/Tyffon/Repositories/expoc-test/donut_examples/build/examples/basic_triangle/cmake_install.cmake")
  include("C:/Tyffon/Repositories/expoc-test/donut_examples/build/examples/vertex_buffer/cmake_install.cmake")
  include("C:/Tyffon/Repositories/expoc-test/donut_examples/build/examples/deferred_shading/cmake_install.cmake")
  include("C:/Tyffon/Repositories/expoc-test/donut_examples/build/examples/bindless_rendering/cmake_install.cmake")
  include("C:/Tyffon/Repositories/expoc-test/donut_examples/build/examples/variable_shading/cmake_install.cmake")
  include("C:/Tyffon/Repositories/expoc-test/donut_examples/build/examples/rt_triangle/cmake_install.cmake")
  include("C:/Tyffon/Repositories/expoc-test/donut_examples/build/examples/rt_shadows/cmake_install.cmake")
  include("C:/Tyffon/Repositories/expoc-test/donut_examples/build/examples/rt_reflections/cmake_install.cmake")
  include("C:/Tyffon/Repositories/expoc-test/donut_examples/build/examples/rt_bindless/cmake_install.cmake")
  include("C:/Tyffon/Repositories/expoc-test/donut_examples/build/examples/meshlets/cmake_install.cmake")
  include("C:/Tyffon/Repositories/expoc-test/donut_examples/build/examples/threaded_rendering/cmake_install.cmake")
  include("C:/Tyffon/Repositories/expoc-test/donut_examples/build/examples/shader_specializations/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Tyffon/Repositories/expoc-test/donut_examples/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
