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
find_library(SUPCPP_LIBRARY supc++)
if(SUPCPP_LIBRARY)
	target_link_libraries(back PUBLIC SUPCPP_LIBRARY)
endif()

add_executable(abrt abrt.cc)
add_executable(segv segv.cc)

install(TARGETS back
	LIBRARY DESTINATION lib
	)

enable_testing()
add_test(NAME abrt COMMAND sh -c "$<TARGET_FILE:abrt>")
add_test(NAME segv COMMAND sh -c "$<TARGET_FILE:segv>")

set_property(TEST abrt segv PROPERTY ENVIRONMENT LD_PRELOAD=$<TARGET_FILE:back>)
set_tests_properties(abrt segv PROPERTIES PASS_REGULAR_EXPRESSION "SIGNAL")
