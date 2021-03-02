add_subdirectory(shaders)

add_executable(${PROJECT_NAME}
	about-window.cpp
	annotated-entry.cpp
	application.cpp
	bc-selector.cpp
	brandy-window.cpp
	config-state.cpp
	config-window.cpp
	conv-utils.cpp
	display-area.cpp
	export-window.cpp
	graphics.cpp
	listener-manager.cpp
	main.cpp
	obstacle-shape.cpp
	shape-config-widget.cpp
	shape-config-window.cpp
	simulation-state.cpp
	simulation-window.cpp
	simulator.cpp
	simulator-classic.cpp
	start-state.cpp
	start-window.cpp
	styles.cpp
	validator-manager.cpp
	validity-indicator.cpp
	video-exporter.cpp
	${CMAKE_CURRENT_BINARY_DIR}/shaders/gresources.c
)

add_subdirectory(tests)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)

include_directories(${PROJECT_NAME} . tests)

target_link_libraries(${PROJECT_NAME} -pthread brandy0-lib -lavcodec -lavformat -lavutil -lswscale)

add_dependencies(${PROJECT_NAME} brandy0-gresources)

execute_process(COMMAND pkg-config gtkmm-3.0 --cflags OUTPUT_VARIABLE CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
separate_arguments(CFLAGS UNIX_COMMAND "${CFLAGS}")
target_compile_options(${PROJECT_NAME} PRIVATE ${CFLAGS})

execute_process(COMMAND pkg-config gtkmm-3.0 --libs OUTPUT_VARIABLE LFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
target_link_libraries(${PROJECT_NAME} ${LFLAGS} -lepoxy)
