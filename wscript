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
	if bld.env['CPPCHECK']:
		for lib in bld.all_task_gen:
			for source_file in Utils.to_list(lib.source):
				cmd = '%s %s' % (
						bld.env['CPPCHECK'],
						os.path.join(lib.path.abspath(), source_file)
					)
				Utils.pproc.Popen(cmd, shell=True).wait()

def _run_astyle(bld):
	if bld.env['ASTYLE']:
		for lib in bld.all_task_gen:
			for source_file in Utils.to_list(lib.source):
				cmd = '%s --style=stroustrup --indent=tab -n -q %s' % (
						bld.env['ASTYLE'],
						os.path.join(lib.path.abspath(), source_file)
					)
				Utils.pproc.Popen(cmd, shell=True).wait()

def _run_doxygen( bld ):
	if bld.env['DOXYGEN']:
		Utils.pproc.Popen( '%s ./doxyconf' % bld.env['DOXYGEN'],
				shell=True ).wait()

def set_options(opt):
	opt.tool_options('compiler_cxx')

	opt.add_option('--cppcheck', default=False, dest='cppcheck', action='store_true')

def configure(conf):
	conf.check_tool('compiler_cxx')
	conf.check_cxx(lib='boost_filesystem', uselib_store='boost_filesystem', mandatory=True)
	conf.check_cxx(lib='boost_regex', uselib_store='boost_regex', mandatory=True)
	conf.check_cxx(lib='boost_system', uselib_store='boost_system', mandatory=True)

	conf.find_program('cppcheck', var='CPPCHECK')
	conf.find_program('astyle', var='ASTYLE')
	conf.find_program('doxygen', var='DOXYGEN')
	conf.find_program('dot', var='dot')
	conf.find_program('ctags', var='CTAGS')

def build(bld):
	if Options.options.cppcheck:
		bld.add_pre_fun(_run_cppcheck)

	bld.add_pre_fun(_run_astyle)
	bld.add_post_fun(_run_doxygen)

	bld.new_task_gen(
			features = 'cxx cprogram',
			source = bld.path.ant_glob('**/*.cc'),
			target = APPNAME,
			uselib = [ 'boost_filesystem', 'boost_regex', 'boost_system' ],
			includes = './src /usr/include',
			cxxflags = [ '-static', '-Wall', '-pedantic', '-std=c++0x', ]
		)

