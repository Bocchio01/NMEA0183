add_library(libs
    libs.c
    libs.h
)

add_subdirectory(cLOG)

target_include_directories(libs PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/cLOG
)

target_link_libraries(libs PUBLIC
    cLOG
)