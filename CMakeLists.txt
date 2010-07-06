CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(cppverify)
# Flags to pass to the c++ compiler
SET (CMAKE_CXX_FLAGS "-Wall -g -pipe -pedantic")
ADD_SUBDIRECTORY(src)

