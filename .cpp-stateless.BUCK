
prebuilt_cxx_library(
  name = 'cpp-stateless',
  header_only = True,
  header_namespace = 'stateless++',
  exported_headers  = subdir_glob([ # public include files
    ('stateless++', '**/*.h'), # those will be exported
    ('stateless++', '**/*.hpp'), # those will be exported,
  ]),
  visibility = [
    'PUBLIC',
  ]
)
