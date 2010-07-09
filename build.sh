#!/bin/bash
./waf configure && ./waf && doxygen doxyconf && cppcheck src --xml 2> cppcheck.xml

