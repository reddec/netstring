project(NetString)
set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_BUILD_TYPE Release)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(./ SRC_LIST)
list(REMOVE_ITEM  SRC_LIST example.cpp)
#add_executable(${PROJECT_NAME} ${SRC_LIST})

add_library(shared-lib SHARED  ${SRC_LIST})
set_target_properties(shared-lib PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

add_library(static-lib STATIC  ${SRC_LIST})
set_target_properties(static-lib PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

install(TARGETS static-lib shared-lib DESTINATION "/usr/local/lib/")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/  DESTINATION "/usr/local/include/" FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN ".git" EXCLUDE)

# Setup DEBIAN control files
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR ${time})
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_DESCRIPTION "Common utils for networking developingF")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Red Dec <net.dev@mail.ru>")

include(CPack)

# Build .DEB package
add_custom_command(TARGET static-lib shared-lib POST_BUILD
                  COMMAND make package)
