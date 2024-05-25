project(src
    VERSION 1.0
    DESCRIPTION "Core for the NMEA0183 library"
    LANGUAGES C
)

add_subdirectory(sensors)

add_library(${PROJECT_NAME}
    NMEA0183.h
    NMEA0183.c
    sensors.h
    sensors.c
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC
    libs
    sensors
)