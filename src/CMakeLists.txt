cmake_minimum_required(VERSION 2.8)
SET(CPPVERIFY_SOURCE
	cppverify.cc
	cppverify.h
	fileloader.cc
	fileloader.h
	)

SET(CPPVERIFY_MODULES ${PROJECT_SOURCE_DIR}/cmake_modules)
MESSAGE(STATUS "Local module path: " ${CPPVERIFY_MODULES})

# Append our own module path to the environment
SET(CMAKE_MODULE_PATH ${CPPVERIFY_MODULES} ${CMAKE_MODULE_PATH})
# Find the requred packages
FIND_PACKAGE(Poco REQUIRED)
MESSAGE(STATUS "Include path for Poco: " ${POCO_INCLUDE_DIR})
MESSAGE(STATUS "Source directory: " ${PROJECT_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src ${POCO_INCLUDE_DIR})
SET(CPPVERIFY_TARGET cppverify)
ADD_EXECUTABLE(${CPPVERIFY_TARGET} ${CPPVERIFY_SOURCE})
