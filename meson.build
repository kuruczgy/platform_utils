project('platform_utils', 'c', version : '0.1')

ds_proj = subproject('ds')
ds_vec_dep = ds_proj.get_variable('ds_vec_dep')

incdir = include_directories('include')

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

executable(
  'minipc-test',
  'loop.c',
  'minipc.c',
  'minipc-test.c',
  dependencies: ds_vec_dep,
  include_directories: incdir
)
