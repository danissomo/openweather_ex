cmake_minimum_required(VERSION 3.10)
project(httpget)

add_executable(app main.cpp)
add_subdirectory(json)
target_link_libraries(app PRIVATE curl nlohmann_json::nlohmann_json)

