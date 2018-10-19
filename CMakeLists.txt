cmake_minimum_required(VERSION 2.8)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBION REQUIRED libion)

find_package(GTest REQUIRED)

add_executable(ion-unit-tests
	src/ion_test_fixture.cpp
	src/allocate_test.cpp
	src/formerly_valid_handle_test.cpp
	src/invalid_values_test.cpp
	src/map_test.cpp
	src/exit_test.cpp
)

target_include_directories(ion-unit-tests
	PRIVATE ${GTEST_INCLUDE_DIR}
)

target_link_libraries(ion-unit-tests
	${LIBION_LIBRARIES}
	${GTEST_LIBRARY}
	${GTEST_MAIN_LIBRARY}
	pthread
)

install(TARGETS ion-unit-tests RUNTIME DESTINATION bin)

add_executable(ion_test
	src/ion_test.c
)

target_link_libraries(ion_test
	${LIBION_LIBRARIES}
)
target_include_directories(ion_test PUBLIC ${LIBION_INCLUDE_DIRS})

install(TARGETS ion_test RUNTIME DESTINATION bin)
