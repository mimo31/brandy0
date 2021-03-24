#set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,clang-analyzer-*,bugprone-*,misc-*,cppcoreguidelines-*,concurrency-*,modernize-*,performance-*,portability-*,readability-*,-readability-braces-around-statements,-modernize-use-trailing-return-type)

set(PRIMARY_SOURCES
	about-window.cpp
	annotated-entry.cpp
	application.cpp
	bc-selector.cpp
	brandy-window.cpp
	config-state.cpp
	config-state-abstr.cpp
	config-window.cpp
	conv-utils.cpp
	display-area.cpp
	export-window.cpp
	graphics.cpp
	listener-manager.cpp
	main.cpp
	obstacle-shape.cpp
	preset-window.cpp
	shape-config-widget.cpp
	shape-config-window.cpp
	simulation-params-preset.cpp
	simulation-state.cpp
	simulation-state-abstr.cpp
	simulation-window.cpp
	simulator.cpp
	simulator-classic.cpp
	start-state.cpp
	start-window.cpp
	style-manager.cpp
	validator-manager.cpp
	validity-indicator.cpp
	video-exporter.cpp
)

add_executable(${PROJECT_NAME} ${PRIMARY_SOURCES})
add_executable(${PROJECT_NAME}-test ${PRIMARY_SOURCES})

add_subdirectory(shaders)
set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/shaders/gresources.c PROPERTY GENERATED 1)

add_subdirectory(tests)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)
target_compile_options(${PROJECT_NAME}-test PRIVATE -Wall -Wextra $<$<COMPILE_LANGUAGE:CXX>:-std=c++17> -DTESTING)

include_directories(${PROJECT_NAME} . tests alias)
include_directories(${PROJECT_NAME}-test . tests alias)

target_link_libraries(${PROJECT_NAME} -pthread brandy0-lib -lavcodec -lavformat -lavutil -lswscale)
target_link_libraries(${PROJECT_NAME}-test -pthread brandy0-lib -lavcodec -lavformat -lavutil -lswscale)

add_dependencies(${PROJECT_NAME} brandy0-gresources)
add_dependencies(${PROJECT_NAME}-test brandy0-gresources)

execute_process(COMMAND pkg-config gtkmm-3.0 --cflags OUTPUT_VARIABLE CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
separate_arguments(CFLAGS UNIX_COMMAND "${CFLAGS}")
target_compile_options(${PROJECT_NAME} PRIVATE ${CFLAGS})
target_compile_options(${PROJECT_NAME}-test PRIVATE ${CFLAGS})

execute_process(COMMAND pkg-config gtkmm-3.0 --libs OUTPUT_VARIABLE LFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
target_link_libraries(${PROJECT_NAME} ${LFLAGS} -lepoxy)
target_link_libraries(${PROJECT_NAME}-test ${LFLAGS} -lepoxy)