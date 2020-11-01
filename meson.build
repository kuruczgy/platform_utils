project('minipc', 'c', version : '0.1')

ds_proj = subproject('ds')
ds_vec = ds_proj.get_variable('ds_vec_dep')

incdir = include_directories('include')

minipc = library(
  'minipc',
  'minipc.c',
  'loop.c',
  dependencies : [ ds_vec ],
  include_directories : incdir
)
minipc_dep = declare_dependency(link_with : minipc, include_directories : incdir)

executable(
  'minipc',
  'loop.c',
  'minipc.c',
  'main.c',
  dependencies : [ ds_vec ],
  include_directories : incdir
)
