project('chowdren-extractor', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++17'])

boost = dependency('boost', modules: ['program_options', 'filesystem'])
zlib  = dependency('zlib')

executable('chowdren-extractor', 
  'chowdren_extractor.cpp', 'stb.cpp', 'util.cpp', 'chowimg.cpp',
  'archive.cpp', 'image.cpp', 'sound.cpp', 'shader.cpp', 'probe.cpp',
  install : true, dependencies: [ boost, zlib ])

executable('chowimg', 'chowimg_standalone.cpp', 'chowimg.cpp', 'util.cpp', 'stb.cpp',
  install: true, dependencies: [ boost ])
