CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(cppverify)
## Flags to pass to the c++ compiler
SET (CMAKE_CXX_FLAGS "-lboost_system -Wall -g -pipe -pedantic")
ADD_SUBDIRECTORY(src)

