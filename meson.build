project('unionjson', ['c', 'cpp'],
  version: '0.1.0',
  default_options: ['warning_level=3', 'c_std=c11', 'cpp_std=c++11']
)

# Add common project arguments for C sources.
add_project_arguments('-g', language: 'c')
add_project_arguments('-rdynamic', language: 'c')

# Define the include directory (headers are in "include")
inc = include_directories('include')

# ---------------------------------------------------------------------------
# Build the Library
# ---------------------------------------------------------------------------
# Assume that all files except main.c are part of the library.
lib_sources = [
  'src/json.c',
  'src/obj_hash_linear_probing.c',
  'src/arr_dynamic_array.c'
]
static_lib = static_library('unionjson', lib_sources, install: true, include_directories: inc)
shared_lib = shared_library('unionjson', lib_sources, install: true, include_directories: inc)

# ---------------------------------------------------------------------------
# Build the Main Application
# ---------------------------------------------------------------------------
# main.c is the application entry point. Link it with the library.
app = executable('hello', 'main.c',
  include_directories: inc,
  link_with: static_lib
)

# ---------------------------------------------------------------------------
# Setup Tests (using gtest and gmock)
# ---------------------------------------------------------------------------
if not get_option('disable_tests')
  # Find GoogleTest and GoogleMock dependencies.
  gtest_dep = dependency('gtest', required: true)
  gmock_dep = dependency('gmock', required: true)

  # Gather all test files (assumed to be C++ sources in the "test" folder).
  # You may list files explicitly or use a glob pattern:
  test_sources = files('test/test_main.cc')

  inc = include_directories('include', 'src')

  # Build the test executable.
  # It links with your library (so tests can call library functions)
  # and uses the gtest and gmock libraries.
  test_exe = executable('json_gtest', test_sources,
    include_directories: inc,
    dependencies: [gtest_dep, gmock_dep],
    link_with: static_lib,
    cpp_args: ['-pthread', '-fsanitize=leak,address,undefined', '-std=c++20'],  # Required on many systems for threading support
    link_args: ['-fsanitize=leak,address,undefined'],
  )

  # Register the test with Meson.
  test('json_gtest', test_exe)
endif

# ---------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------

install_headers(
  'include/json.h',
  'include/json.hh'
)
