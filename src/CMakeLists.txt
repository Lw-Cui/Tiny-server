INCLUDE_DIRECTORIES(${SERVER_INCLUDE_DIRS})
ADD_LIBRARY(${SERVER_LIBRARIES}
        ../include/easylogging++.h
        ../include/NetService.hpp
        ../include/NetService.hpp NetService.cpp)
