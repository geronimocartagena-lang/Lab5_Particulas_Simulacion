cmake_minimum_required(VERSION 3.16)

project(Lab5_Parte1_Particulas LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Lab5_Parte1_Particulas main.cpp
    Vector2D.h)

include(GNUInstallDirs)
install(TARGETS Lab5_Parte1_Particulas
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
