
genrule(
  name = 'win-platform.hpp',
  srcs = ['win-platform.hpp.in'],
  out = 'platform.hpp',
  cmd = '(type $SRCS) > $OUT',
)

cxx_library(
  name = 'libzmq',
  header_namespace = '',
  exported_headers = subdir_glob([
    ('include', '*.h'),
  ]),
  exported_platform_headers = [
    ('default', { 'platform.hpp': ':win-platform.hpp' }),
    ('.*x86_64$', { 'platform.hpp': ':win-platform.hpp' }),
  ],
  headers = subdir_glob([
    ('src', '*.hpp'),
  ]),
  srcs = glob([
    'src/*.cpp',
    'src/*.c',   # windows only?
  ]),
  preprocessor_flags = [
    '/DWIN32',
    '/D_WIN32',
    '/D_WINDOWS',
  ],
  exported_preprocessor_flags = [
    '/DZMQ_STATIC',
    '/DZMQ_CUSTOM_PLATFORM_HPP',
    '/DFD_SETSIZE=16384',
    '/D_CRT_SECURE_NO_WARNINGS',
    '/D_WINSOCK_DEPRECATED_NO_WARNINGS',
  ],
  exported_linker_flags = [
    'ws2_32.lib',
    'rpcrt4.lib',
    'iphlpapi.lib',
  ],
  compiler_flags = [
    '/EHsc',
  ],
  visibility = [
    'PUBLIC',
  ],
)
