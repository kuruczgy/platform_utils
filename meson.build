project('platform_utils', 'c', version : '0.1')
cc = meson.get_compiler('c')

system = host_machine.system()

deps_event_loop = []
deps_log = []
if system == 'linux-android'
  deps_event_loop += cc.find_library('android')
  deps_log += cc.find_library('log')
endif

ds_proj = subproject('ds')
ds_vec_dep = ds_proj.get_variable('ds_vec_dep')

incdir = include_directories('include')

pu_main = library(
  'pu-main',
  'main.c',
  include_directories: incdir,
  override_options: [ 'b_lundef=false' ]
)
pu_main_dep = declare_dependency(link_with: pu_main, include_directories: incdir)

pu_log = library(
  'pu-log',
  'log.c',
  include_directories: incdir,
  dependencies: deps_log
)
pu_log_dep = declare_dependency(link_with: pu_log, include_directories: incdir)

pu_event_loop = library(
  'pu-event-loop',
  'event_loop.c',
  dependencies: [ ds_vec_dep, deps_event_loop ],
  include_directories: incdir
)
pu_event_loop_dep = declare_dependency(link_with: pu_event_loop, include_directories: incdir)

if host_machine.system() != 'linux-android'
pu_minipc = library(
  'pu-minipc',
  'minipc.c',
  dependencies: [ ds_vec_dep, pu_event_loop_dep ],
  include_directories: incdir
)
pu_minipc_dep = declare_dependency(link_with: pu_minipc, include_directories: incdir)
endif

platform_utils_test = library(
  'platform_utils_test',
  'test.c',
  dependencies: [ pu_main_dep, pu_log_dep ]
)
platform_utils_dep = declare_dependency(link_with: platform_utils_test)

if not meson.is_subproject()
  executable(
    'minipc-test',
    'minipc-test.c',
    dependencies: [ pu_main_dep, pu_event_loop_dep, pu_minipc_dep, ds_vec_dep ],
    include_directories: incdir
  )
endif
