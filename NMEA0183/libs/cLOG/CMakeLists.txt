project(cLOG
    VERSION 1.0
    DESCRIPTION "C library for logging"
    LANGUAGES C
)

add_library(${PROJECT_NAME}
    cLOG.c
    cLOG.h
)

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})