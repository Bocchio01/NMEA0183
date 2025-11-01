file(GLOB TEST_SOURCES *.c)

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE nmea0183_lib)
    add_test(NAME ${test_name} COMMAND ${test_name})
    set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
endforeach()

# Copy test datasets to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/datasets/
    DESTINATION ${CMAKE_BINARY_DIR}/tests/datasets/)
