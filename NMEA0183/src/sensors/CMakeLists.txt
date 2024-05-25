project(sensors
    VERSION 1.0
    DESCRIPTION "Sensors submodules"
    LANGUAGES C
)

add_library(${PROJECT_NAME}
    mwv.c
    mwv.h
    xdr.c
    xdr.h
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC libs)
