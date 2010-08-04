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

def _get_source_files( bld, _filter ):
	src_files = []
	for lib in bld.all_task_gen:
		src_files += [ os.path.join( lib.path.abspath(), src ) for src in Utils.to_list(lib.source)
				if os.path.splitext(src)[1] in _filter ]

	return set(src_files)

def _run_cppcheck(task):
	src_files = [ os.path.abspath(p.srcpath(task.env)) for p in task.inputs ]
	cppcheck_file = os.path.abspath( task.outputs[0].bldpath(task.env) )
	result_file = os.path.abspath( task.outputs[1].bldpath(task.env) )
	inc_dirs = ' '.join( [ '-I %s' % d for d in set( [ os.path.dirname( f ) for f
		in src_files ] ) ] )

	f = open( cppcheck_file, 'w' )
	f.write( '\n'.join( src_files ) )
	f.close()

	cmd = '%s -v --enable=style --file-list=%s --xml --error-exitcode=1 %s >/dev/null 2>%s' % (
			task.env.CPPCHECK, cppcheck_file, inc_dirs, result_file, )
	Utils.pproc.Popen(cmd, shell=True).wait()

def _run_astyle(bld):
	if bld.env.ASTYLE:
		src_files = _get_source_files( bld, ( '.cc', '.h' ) )

		for sfile in src_files:
			cmd = '%s --style=stroustrup --indent=tab -n -q %s' % (
					bld.env['ASTYLE'], sfile )
			Utils.pproc.Popen(cmd, shell=True).wait()

def _run_doxygen( task ):
	if task.env.DOXYGEN:
		doxygen_in = os.path.abspath( task.inputs[0].srcpath(task.env) )
		docs_dir = os.path.abspath( task.outputs[0].bldpath(task.env) )
		doxygen_conf = os.path.abspath( task.outputs[1].bldpath(task.env) )
		doxygen_log = os.path.abspath( task.outputs[2].bldpath(task.env) )
		input_dir = os.path.join( os.path.dirname( doxygen_in ), 'src' )

		doxy_dict = {
				'project_name': APPNAME,
				'output_directory': docs_dir,
				'warn_logfile': doxygen_log,
				'input_dir': input_dir,
			}

		doxygen_config_content = Utils.readf( doxygen_in ) % doxy_dict

		doxy_config = open( doxygen_conf, 'w' )
		doxy_config.write( doxygen_config_content )
		doxy_config.close()
		
		Utils.pproc.Popen( '%s %s' % ( task.env.DOXYGEN, doxygen_conf ),
				shell=True ).wait()

def set_options(opt):
	opt.tool_options('compiler_cxx')

	opt.add_option('--cppcheck', default=False, dest='cppcheck', action='store_true')
	opt.add_option('--static', default=False, dest='static', action='store_true')

	opt.add_option('--build_kind', action='store', default='debug', help='build the selected variants')

def configure(conf):
	conf.env.FULLSTATIC = Options.options.static
	conf.env.RUN_CPPCHECK = Options.options.cppcheck

	conf.env.NAME = 'default'

	conf.check_tool('compiler_cxx')

	conf.find_program('cppcheck', var='CPPCHECK')
	conf.find_program('astyle', var='ASTYLE')
	conf.find_program('doxygen', var='DOXYGEN')
	conf.find_program('dot', var='dot')
	conf.find_program('ctags', var='CTAGS')

	pkg_config_args = '--cflags --libs'
	if conf.env.FULLSTATIC:
		# if you use static we need to link against additional libraries
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

	conf.env.INC_DIRS = [ './src', '/usr/include', ]

	for name in ['debug', 'release']:
		env = conf.env.copy()
		env.set_variant(name)
		env.NAME = name
		conf.set_env_name(name, env)
	
	conf.setenv( 'debug' )
	conf.env.CXXFLAGS = [ '-g', '-Wall', '-Wextra', '-pedantic', '-std=c++0x', ]

	conf.setenv( 'release' )
	conf.env.CXXFLAGS = [ '-Wall', '-Wextra', '-pedantic', '-O2', '-std=c++0x', ]

	conf.setenv( 'default' )

def build(bld):
	bld.add_pre_fun(_run_astyle)

	cc_files = bld.path.ant_glob('**/*.cc').split(' ')
	h_files = bld.path.ant_glob( '**/*.h').split(' ')
	src_files = cc_files + h_files

	if bld.env.RUN_CPPCHECK:
		# run cppcheck
		bld.new_task_gen(
				rule = _run_cppcheck,
				source = src_files,
				target = [ 'cppcheck_files.txt', 'cppcheck.xml', ],
				uselib = [ 'boost_program_options', 'boost_filesystem', 'boost_regex', 'boost_system',
					'glog', ],
				includes = bld.env.INC_DIRS,
			)

	# compile cppverify
	bld.new_task_gen(
			features = 'cxx cprogram',
			source = cc_files,
			target = APPNAME,
			uselib = [ 'boost_program_options', 'boost_filesystem', 'boost_regex', 'boost_system',
				'glog', ],
			includes = bld.env.INC_DIRS,
			cxxflags = bld.env.CXXFLAGS,
		)

	# doxygen task
	bld.new_task_gen(
			rule=_run_doxygen,
			source=[ 'doxyconf.in', ] + src_files,
			target=[ 'docs', 'doxygen.conf', 'doxygen.log' ],
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
