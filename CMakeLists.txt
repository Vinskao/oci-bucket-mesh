cmake_minimum_required(VERSION 3.25)

project(oci_bucket_mesh VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(OCI_BUCKET_MESH_BUILD_IMPLEMENTATION "Build implementation targets" OFF)

include(CTest)

add_library(oci_bucket_mesh_core INTERFACE)
target_include_directories(oci_bucket_mesh_core INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(bucketmesh_control_api
  ${CMAKE_CURRENT_SOURCE_DIR}/apps/control_api/main.cpp
)
target_link_libraries(bucketmesh_control_api PRIVATE oci_bucket_mesh_core)

add_executable(bucketmesh_domain_tests
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/mesh_domain_tests.cpp
)
target_link_libraries(bucketmesh_domain_tests PRIVATE oci_bucket_mesh_core)

if(BUILD_TESTING)
  add_test(NAME bucketmesh_domain_tests COMMAND bucketmesh_domain_tests)
endif()

message(STATUS "OciBucketMesh core implementation scaffold enabled")

if(OCI_BUCKET_MESH_BUILD_IMPLEMENTATION)
  message(STATUS "Implementation build path is enabled; add reviewed Drogon/gRPC/OCI dependencies here when available.")
endif()
