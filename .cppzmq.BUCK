
prebuilt_cxx_library(
  name = 'cppzmq',
  header_only = True,
  header_namespace = '',
  exported_headers  = subdir_glob([ # public include files
    ('', '**/*.h'), # those will be exported
    ('', '**/*.hpp'), # those will be exported,
  ]),
  visibility = [
    'PUBLIC',
  ]
)
