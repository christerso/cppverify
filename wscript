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
		doxy_dict = {
				'project_name': APPNAME,
				'output_directory': os.path.join( bld.bdir, '%s/docs' % bld.env.NAME ),
				'warn_logfile': os.path.join( bld.bdir, '%s/doxygen.log' % bld.env.NAME ),
			}
		doxy_config_content = Utils.readf( './doxyconf.in' ) % doxy_dict
		doxy_config_fname = os.path.join( bld.bdir, '%s/doxygen.conf' % bld.env.NAME )

		doxy_config = open( doxy_config_fname, 'w' )
		doxy_config.write( doxy_config_content )
		doxy_config.close()
		
		Utils.pproc.Popen( '%s %s' % ( bld.env['DOXYGEN'], doxy_config_fname ),
				shell=True ).wait()

def set_options(opt):
	opt.tool_options('compiler_cxx')

	opt.add_option('--cppcheck', default=False, dest='cppcheck', action='store_true')
	opt.add_option('--static', default=False, dest='static', action='store_true')

	opt.add_option('--build_kind', action='store', default='debug', help='build the selected variants')

def configure(conf):
	conf.env.FULLSTATIC = Options.options.static
	conf.env.NAME = 'default'

	conf.check_tool('compiler_cxx')

	conf.find_program('cppcheck', var='CPPCHECK')
	conf.find_program('astyle', var='ASTYLE')
	conf.find_program('doxygen', var='DOXYGEN')
	conf.find_program('dot', var='dot')
	conf.find_program('ctags', var='CTAGS')

	pkg_config_args = '--cflags --libs'
	if conf.env.FULLSTATIC:
		pkg_config_args = '%s %s' % (pkg_config_args, '--static')
	
	conf.check_cfg(package='libboost_program_options', args=pkg_config_args,
			uselib_store='boost_program_options', mandatory=True)
	conf.check_cfg(package='libboost_filesystem', args=pkg_config_args,
			uselib_store='boost_filesystem', mandatory=True)
	conf.check_cfg(package='libboost_regex', args=pkg_config_args,
			uselib_store='boost_regex', mandatory=True)
	conf.check_cfg(package='libboost_system', args=pkg_config_args,
			uselib_store='boost_system', mandatory=True)

	conf.check_cfg(package='libglog', args=pkg_config_args,
			uselib_store='glog', mandatory=True)

	conf.check_cfg(package='libgtest', args='--cflags --libs',
			uselib_store='gtest', mandatory=False)

	for name in ['debug', 'release']:
		env = conf.env.copy()
		env.set_variant(name)
		conf.set_env_name(name, env)
		conf.env.NAME = name
	
	conf.setenv( 'debug' )
	conf.env.CXXFLAGS = [ '-g', '-Wall', '-Wextra', '-pedantic', '-std=c++0x', ]

	conf.setenv( 'release' )
	conf.env.CXXFLAGS = [ '-Wall', '-Wextra', '-pedantic', '-std=c++0x', ]

def build(bld):
	if Options.options.cppcheck:
		bld.add_pre_fun(_run_cppcheck)

	bld.add_pre_fun(_run_astyle)
	bld.add_post_fun(_run_doxygen)

	bld.new_task_gen(
			features = 'cxx cprogram',
			source = bld.path.ant_glob('**/*.cc'),
			target = APPNAME,
			uselib = [ 'boost_program_options', 'boost_filesystem', 'boost_regex', 'boost_system',
				'glog', ],
			includes = './src /usr/include',
			cxxflags = bld.env.CXXFLAGS,
		)

	#Setup the build kinds to run
	for obj in bld.all_task_gen[:]: 
		for x in ['debug', 'release']:
			cloned_obj = obj.clone(x) 
			kind = Options.options.build_kind
			if kind.find(x) < 0:
				cloned_obj.posted = True

		# disable the default one
		obj.posted = True
