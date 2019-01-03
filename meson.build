project('dlib', 'cpp',
        default_options : ['c_std=c11', 'cpp_std=c++14'],
        license: 'Boost Software License')

defines = []
deps = []

deps += dependency('threads')

compiler = meson.get_compiler('c')
#deps += compiler.find_library('m', required : false)

if get_option('support_libpng') == 'yes'
    defines += [ '-DDLIB_PNG_SUPPORT' ]
    deps += dependency('libpng')
elif get_option('support_libpng') == 'auto'
    dep = dependency('libpng', required: false)
    if dep.found()
        deps += dep
    endif
endif

if get_option('support_libgif') == 'yes'
    defines += [ '-DDLIB_GIF_SUPPORT' ]
    deps += dependency('libgif')
elif get_option('support_libgif') == 'auto'
    dep = dependency('libgif', required: false)
    if dep.found()
        deps += dep
    endif
endif

if get_option('support_libjpeg') == 'yes'
    defines += [ '-DDLIB_JPEG_SUPPORT' ]
    deps += dependency('libjpeg')
elif get_option('support_libjpeg') == 'auto'
    dep = dependency('libjpeg', required: false)
    if dep.found()
        deps += dep
    endif
endif

if get_option('support_gui') == 'yes'
    deps += dependency('x11')
elif get_option('support_gui') == 'auto'
    dep = dependency('x11', required: false)
    if dep.found()
        deps += dep
    else
        defines += [ '-DDLIB_NO_GUI_SUPPORT' ]
    endif
else
    defines += [ '-DDLIB_NO_GUI_SUPPORT' ]
endif

if get_option('use_cblas') == 'yes'
    defines += [ '-DDLIB_USE_BLAS' ]
    deps += dependency('cblas')
elif get_option('use_cblas') == 'auto'
    dep = dependency('cblas', required: false)
    if dep.found()
        deps += dep
    endif
endif

if get_option('use_lapack') == 'yes'
    defines += [ '-DDLIB_USE_LAPACK' ]
    deps += dependency('lapack')
elif get_option('use_lapack') == 'auto'
    dep = dependency('lapack', required: false)
    if dep.found()
        deps += dep
    endif
endif

if get_option('enable_asserts')
    defines += [ '-DDLIB_ENABLE_ASSERTS' ]
endif

if get_option('enable_stack_traces')
    defines += [ '-DDLIB_ENABLE_ASSERTS' ]
endif

# It would be a nightmare to try and add dllexport annotations to dlib
# so, practically, we can only build a static library on Windows...
if host_machine.system() == 'windows'
    lib = static_library('dlib', [ 'dlib/all/source.cpp' ],
                         cpp_args: defines,
                         dependencies: deps)
else
# We have to special case Android because it doesn't support library versioning
# XXX: it would be nicer if Meson handled this automatically
#if compiler.get_define('__ANDROID__') != ''
#    lib = library('dlib', [ 'dlib/all/source.cpp' ],
#                  cpp_args: defines,
#                  dependencies: deps)
#else
    lib = library('dlib', [ 'dlib/all/source.cpp' ],
                  version: '19.7.0',
                  cpp_args: defines,
                  dependencies: deps)
#endif
endif
inc = include_directories('.')
dlib_dep = declare_dependency(link_with: lib,
                              include_directories: inc,
                              compile_args: defines,
                              dependencies: deps)

