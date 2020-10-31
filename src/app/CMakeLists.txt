find_program(GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED)

set(GRESOURCE_C	gresources.c)
set(GRESOURCE_XML gresources.xml)

add_custom_command(
	OUTPUT ${GRESOURCE_C}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND ${GLIB_COMPILE_RESOURCES}
	ARGS
		--generate-source
		--target=${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
		${GRESOURCE_XML}
	VERBATIM
	MAIN_DEPENDENCY ${GRESOURCE_XML}
	DEPENDS
		*.vs.glsl
		*.fs.glsl
)

add_custom_target(
	brandy0-gresources
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
)

add_executable(${PROJECT_NAME}
	main.cpp
	simulator.cpp
	${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
)
set_source_files_properties(
	${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
	PROPERTIES GENERATED TRUE
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)

target_link_libraries(${PROJECT_NAME} brandy0-lib)

add_dependencies(${PROJECT_NAME} brandy0-gresources)

execute_process(COMMAND pkg-config gtkmm-3.0 --cflags OUTPUT_VARIABLE CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
separate_arguments(CFLAGS UNIX_COMMAND "${CFLAGS}")
target_compile_options(${PROJECT_NAME} PRIVATE ${CFLAGS})

execute_process(COMMAND pkg-config gtkmm-3.0 --libs OUTPUT_VARIABLE LFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
target_link_libraries(${PROJECT_NAME} ${LFLAGS} -lepoxy)
