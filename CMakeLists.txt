cmake_minimum_required(VERSION 3.1)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
		STRING "Choose the type of build." FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
		"Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

project(libback)
add_library(back SHARED back)
target_link_libraries(back ${CMAKE_DL_LIBS} supc++)

install(TARGETS back
	LIBRARY DESTINATION lib
	)

set(${PROJECT_NAME}_TESTS
	abrt
	segv
	)

enable_testing()
add_custom_target(all_tests)
foreach(test ${${PROJECT_NAME}_TESTS})
	add_executable(${test} EXCLUDE_FROM_ALL ${test}.cc)
	add_test(NAME ${test} COMMAND sh -c "$<TARGET_FILE:${test}>")
	add_dependencies(all_tests ${test})
endforeach(test)
build_command(CTEST_CUSTOM_PRE_TEST TARGET all_tests)
string(CONFIGURE \"@CTEST_CUSTOM_PRE_TEST@\" CTEST_CUSTOM_PRE_TEST_QUOTED ESCAPE_QUOTES)
file(WRITE "${CMAKE_BINARY_DIR}/CTestCustom.cmake" "set(CTEST_CUSTOM_PRE_TEST ${CTEST_CUSTOM_PRE_TEST_QUOTED})" "\n")

add_dependencies(all_tests back)
set_property(TEST abrt segv PROPERTY ENVIRONMENT LD_PRELOAD=$<TARGET_FILE:back>)
set_tests_properties(abrt segv PROPERTIES PASS_REGULAR_EXPRESSION "SIGNAL")
