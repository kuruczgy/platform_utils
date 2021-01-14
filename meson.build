project('platform_utils', 'c', version : '0.1')
cc = meson.get_compiler('c')

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

pu_log_deps = []
if host_machine.system() == 'linux-android'
  pu_log_deps += cc.find_library('log')
endif
pu_log = library(
  'pu-log',
  'log.c',
  include_directories: incdir,
  dependencies: pu_log_deps
)
pu_log_dep = declare_dependency(link_with: pu_log, include_directories: incdir)

pu_loop = library(
  'pu-loop',
  'loop.c',
  dependencies: ds_vec_dep,
  include_directories: incdir
)
pu_loop_dep = declare_dependency(link_with: pu_loop, include_directories: incdir)

pu_minipc = library(
  'pu-minipc',
  'minipc.c',
  dependencies: [ ds_vec_dep, pu_loop_dep ],
  include_directories: incdir
)
pu_minipc_dep = declare_dependency(link_with: pu_minipc, include_directories: incdir)

platform_utils_test = library(
  'platform_utils_test',
  'test.c',
  dependencies: [ pu_main_dep, pu_log_dep ]
)
platform_utils_dep = declare_dependency(link_with: platform_utils_test)

if not meson.is_subproject()
  executable(
    'minipc-test',
    'loop.c',
    'minipc.c',
    'minipc-test.c',
    dependencies: ds_vec_dep,
    include_directories: incdir
  )
endif
