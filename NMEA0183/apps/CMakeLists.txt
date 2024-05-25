add_executable(${CMAKE_PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} PUBLIC
    libs
    src
)

add_executable(${CMAKE_PROJECT_NAME}_Bench bench.c)
target_link_libraries(${CMAKE_PROJECT_NAME}_Bench PUBLIC
    libs
    src
)