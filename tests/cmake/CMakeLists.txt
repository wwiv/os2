project (cmake_tests)

add_library(foo foo.cpp)
add_executable(smoke smoke.cpp)
target_link_libraries(smoke foo)
