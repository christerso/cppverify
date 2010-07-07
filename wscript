#! /usr/bin/env python
# encoding: utf-8

import os
import sys

import Options
import Utils

# the following two variables are used by the target "waf dist"
VERSION='0.0.1'
APPNAME='cppverify'

# these variables are mandatory ('/' are converted automatically)
top = '.'
out = 'build'

def _run_cppcheck(bld):
	for lib in bld.all_task_gen:
		for source_file in Utils.to_list(lib.source):
			cmd = '%s %s' % (
					bld.env['CPPCHECK'],
					os.path.join(lib.path.abspath(), source_file)
				)
			Utils.pproc.Popen(cmd, shell=True).wait()

def _run_astyle(bld):
	for lib in bld.all_task_gen:
		for source_file in Utils.to_list(lib.source):
			cmd = '%s --style=k/r --indent=tab -n -q %s' % (
					bld.env['ASTYLE'],
					os.path.join(lib.path.abspath(), source_file)
				)
			Utils.pproc.Popen(cmd, shell=True).wait()

def set_options(opt):
	opt.tool_options('compiler_cxx')

	opt.add_option('--cppcheck', default=False, dest='cppcheck', action='store_true')

def configure(conf):
	conf.check_tool('compiler_cxx')
	conf.check_cxx(lib='PocoFoundation', uselib_store='PocoFoundation', mandatory=True)

	conf.find_program('cppcheck', var='CPPCHECK')
	conf.find_program('astyle', var='ASTYLE')
	conf.find_program('ctags', var='CTAGS')

def build(bld):
	if Options.options.cppcheck:
		bld.add_pre_fun(_run_cppcheck)

	bld.add_pre_fun(_run_astyle)

	bld.new_task_gen(
			features = 'cxx cprogram',
			source = bld.path.ant_glob('**/*.cc'),
			target = APPNAME,
			uselib = 'PocoFoundation',
			includes = './src /usr/include',
			cxxflags = [ '-static', '-Wall', '-pedantic', '-std=c++0x', ]
		)

	# 2. A simple static library
	#bld(
		#features = 'cc cstaticlib',
		#source = 'test_staticlib.c',
		#target='my_static_lib')

	# note: the syntax bld(...) is equivalent to bld.new_task_gen(...)
	#       instantiates and object of the class task_gen (from the file TaskGen.py)

	# if we had a subfolder we would add the following:
	#bld.add_subdirs('src')

