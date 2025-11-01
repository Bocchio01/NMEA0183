project(nmea0183_examples
    VERSION 1.0.0
    DESCRIPTION "NMEA0183 Library Examples"
    LANGUAGES C
)

# Basic parser example
add_executable(basic_parser
    basic_parser.c
)

target_link_libraries(basic_parser
    PRIVATE nmea0183_lib
)

# Set output directory
set_target_properties(basic_parser
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy test data to build directory for easy execution
file(COPY ${CMAKE_SOURCE_DIR}/tools/matlab/testing/datasets/2023-12-21.txt
    DESTINATION ${CMAKE_BINARY_DIR}/examples/data/)

# Add install target
install(TARGETS basic_parser
    RUNTIME DESTINATION bin
)

# Print example info
message(STATUS "Examples will be built in: ${CMAKE_BINARY_DIR}/bin")