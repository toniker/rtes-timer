cmake_minimum_required(VERSION 3.25)
project(rtes_timer)
set(CMAKE_CXX_STANDARD 20)

Include(FetchContent)

FetchContent_Declare(
        etl
        GIT_REPOSITORY https://github.com/ETLCPP/etl
        GIT_TAG        20.37.0
)

FetchContent_MakeAvailable(etl)


add_executable(rtes_timer main.cpp)
target_link_libraries(rtes_timer PRIVATE etl::etl)
add_compile_options(-pthread)

