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
  visibility = ['PUBLIC']
)

cxx_binary(
  name = 'main',
  srcs = ['blackjack/apps/main.cpp'],
  compiler_flags = [
    '/EHsc',
  ],
  deps = [':blackjack','libzmq//:libzmq'],
  visibility = ['PUBLIC']
)
