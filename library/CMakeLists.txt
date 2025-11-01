project(nmea0183_lib
    VERSION 1.0.0
    DESCRIPTION "NMEA0183 Core Library"
    LANGUAGES C
)

# Library source files
set(NMEA0183_SOURCES
    src/nmea0183.c
    src/sensors.c
    src/sensors/mwv.c
    src/sensors/xdr.c
)

# Library header files
set(NMEA0183_HEADERS
    include/nmea0183/nmea0183.h
    include/nmea0183/sensors.h
    include/nmea0183/sensors/mwv.h
    include/nmea0183/sensors/xdr.h
)

# Create library
add_library(${PROJECT_NAME} STATIC ${NMEA0183_SOURCES} ${NMEA0183_HEADERS})

# Set include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Compiler definitions for embedded systems
target_compile_definitions(${PROJECT_NAME}
    PUBLIC
    NMEA0183_NO_MALLOC=1 # Static allocation only
)

# Set library properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${NMEA0183_HEADERS}"
)

# Installation rules
install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/nmea0183
)

install(DIRECTORY include/nmea0183
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)