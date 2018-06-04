cxx_library(
  name = 'framework',
  header_namespace = 'framework',
  srcs = glob([
    'framework/src/**/*.cpp',
    'framework/src/**/*.c',
  ]),
  headers = subdir_glob([ # private include files
    ('framework/detail', '**/*.h'), # they are only accessible inside the library
    ('framework/detail', '**/*.hpp'),
  ]),
  exported_headers = subdir_glob([ # public include files
    ('framework/include', '**/*.h'), # those will be exported
    ('framework/include', '**/*.hpp'), # and accessible via <framework/header.h>
  ]),
  compiler_flags = [
    '/EHsc',
  ],
  preprocessor_flags = [
    '/DWIN32',
    '/D_WIN32',
    '/D_WINDOWS',
  ],
  deps = ['//cppzmq:cppzmq', '//libzmq:libzmq'],
  visibility = ['PUBLIC']
)

cxx_library(
  name = 'blackjack',
  header_namespace = 'blackjack',
  srcs = glob([
    'blackjack/src/**/*.cpp',
  ]),
  headers = subdir_glob([ # private include files
    ('blackjack/detail', '**/*.h'), # they are only accessible inside the library
    ('blackjack/detail', '**/*.hpp'),
  ]),
  exported_headers = subdir_glob([ # public include files
    ('blackjack/include', '**/*.h'), # those will be exported
    ('blackjack/include', '**/*.hpp'), # and accessible via <blackjack/header.h>
  ]),
  compiler_flags = [
    '/EHsc',
  ],
  preprocessor_flags = [
    '/DWIN32',
    '/D_WIN32',
    '/D_WINDOWS',
  ],
  deps = [],
  visibility = ['PUBLIC']
)

cxx_binary(
  name = 'server',
  srcs = glob([
    'blackjack/apps/server/**/*.cpp',
    'blackjack/apps/shared/**/*.cpp',
  ]),
  headers = subdir_glob([ # private include files
    ('blackjack/apps/server', '**/*.hpp'),
    ('blackjack/apps',        'shared/**/*.hpp'),
  ]),
  compiler_flags = [
    '/EHsc',
  ],
  preprocessor_flags = [
    '/DWIN32',
    '/D_WIN32',
    '/D_WINDOWS',
  ],
  linker_flags = [
    'kernel32.lib',
    'user32.lib',
    'gdi32.lib',
    'winspool.lib',
    'comdlg32.lib',
    'advapi32.lib',
    'shell32.lib',
    'ole32.lib',
    'oleaut32.lib',
    'uuid.lib',
    'odbc32.lib',
    'odbccp32.lib',
  ],
  deps = [':framework',':blackjack','//cpp-stateless:cpp-stateless'],
  visibility = ['PUBLIC']
)

cxx_binary(
  name = 'client',
  srcs = glob([
    'blackjack/apps/client/**/*.cpp',
    'blackjack/apps/shared/**/*.cpp',
  ]),
  headers = subdir_glob([ # private include files
    ('blackjack/apps/client', '**/*.hpp'),
    ('blackjack/apps',        'shared/**/*.hpp'),
  ]),
  compiler_flags = [
    '/EHsc',
  ],
  preprocessor_flags = [
    '/DWIN32',
    '/D_WIN32',
    '/D_WINDOWS',
  ],
  linker_flags = [
    'kernel32.lib',
    'user32.lib',
    'gdi32.lib',
    'winspool.lib',
    'comdlg32.lib',
    'advapi32.lib',
    'shell32.lib',
    'ole32.lib',
    'oleaut32.lib',
    'uuid.lib',
    'odbc32.lib',
    'odbccp32.lib',
  ],
  deps = [':blackjack', ':framework'],
  visibility = ['PUBLIC']
)
