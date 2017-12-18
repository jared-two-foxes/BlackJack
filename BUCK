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
  deps = ['//libzmq:libzmq', '//cppzmq:cppzmq'],
  visibility = ['PUBLIC']
)

cxx_binary(
  name = 'server',
  srcs = ['blackjack/apps/server.cpp'],
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
  deps = [':blackjack'],
  visibility = ['PUBLIC']
)

cxx_binary(
  name = 'client',
  srcs = [
    'blackjack/apps/client.cpp',
    'blackjack/apps/ClientApplication.cpp',
  ],
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
  deps = [':blackjack'],
  visibility = ['PUBLIC']
)
