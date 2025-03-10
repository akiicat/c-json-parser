project('my_project', 'c',
  version : '1.0',
  default_options : ['warning_level=3', 'c_std=c11', 'fsanitize=address']
)

sources = ['main.c', 'json.c', 'src/obj_hash_linear_probing.c', 'src/arr_dynamic_array.c']

add_project_arguments('-g', language: 'c')
add_project_arguments('-rdynamic', language: 'c')

executable('hello', sources)
