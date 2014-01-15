cmake_minimum_required(VERSION 2.6)

add_definitions(-std=c99)
add_definitions(-D_POSIX_C_SOURCE=200112L)

find_package(Threads REQUIRED)
find_package(PNG)

if(PNG_FOUND)
  add_executable(convert convert.c)
  include_directories(${PNG_INCLUDE_DIRS})
  add_definitions(${PNG_DEFINITIONS})
  target_link_libraries(convert ${PNG_LIBRARIES})
endif()

add_executable(checker checker.c)

add_executable(smooth main.c)
target_link_libraries(smooth rt ${CMAKE_THREAD_LIBS_INIT})

add_custom_command(OUTPUT image.in
	COMMAND $<TARGET_FILE:checker> -w 7680 -h 4320 > image.in
	DEPENDS checker)

add_custom_command(OUTPUT image.out
	COMMAND $<TARGET_FILE:smooth>
	DEPENDS smooth image.in)

add_custom_command(OUTPUT image.ppm
	COMMAND $<TARGET_FILE:convert> image.out image.ppm
	DEPENDS convert image.out)

add_custom_target(run DEPENDS image.ppm)
