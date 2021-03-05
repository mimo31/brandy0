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
		plain-float.vs.glsl
		plain.vs.glsl
		rainbow.fs.glsl
		white.fs.glsl
)

add_custom_target(
	brandy0-gresources
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
)

target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C})
