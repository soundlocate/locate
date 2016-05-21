FILE(GLOB_RECURSE project_SRCS *.cpp *.cxx *.cc *.C *.c *.h *.hpp)
SET(project_LIBS ${SFML_LIBRARIES} ${MKL_LIBRARIES} pthread boost_system)
SET(project_BIN ${PROJECT_NAME})

ADD_EXECUTABLE(${project_BIN} ${project_SRCS})
qt5_use_modules(${project_BIN} WebSockets Widgets)
TARGET_LINK_LIBRARIES(${project_BIN} ${project_LIBS})

SET_TARGET_PROPERTIES(${project_BIN} PROPERTIES VERSION "${APPLICATION_VERSION_MAJOR}.${APPLICATION_VERSION_MINOR}" OUTPUT_NAME ${project_BIN} CLEAN_DIRECT_OUTPUT 1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -std=c++14 -Wall -Wextra -fno-omit-frame-pointer")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -xHOST -O3 -ipo -no-prec-div -fp-model fast=2 -std=c++14 -static-intel -fPIC")
#TARGET_COMPILE_FEATURES(${PROJECT_NAME } PRIVATE cxx_constexpr)

INSTALL(TARGETS ${project_BIN} DESTINATION bin)
