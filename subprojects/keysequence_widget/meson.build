project('keysequence_widget', 'cpp',
    default_options: ['cpp_std=c++11'])

qt5 = import('qt5')

deps = []
inc = include_directories('src')
moc_headers = ['src/keysequence_widget.h',
               'src/keysequence_listwidget.h']
sources = ['src/keysequence_widget.cpp',
           'src/keysequence_listwidget.cpp']

deps += dependency('qt5', modules: ['Core', 'Widgets'])

public_cpp_args = []
cpp_args = []
if get_option('default_library') == 'shared'
    public_cpp_args += ['-DKEYSEQUENCE_WIDGET_SHARED']
    cpp_args += ['-DKEYSEQUENCE_WIDGET_BUILDING_SHARED']
endif

keysequence_widget_library = library(
    'keysequence_widget',
    sources,
    qt5.preprocess(moc_headers: moc_headers),
    dependencies: deps,
    include_directories: inc,
    cpp_args: [public_cpp_args, cpp_args])

keysequence_widget_dep = declare_dependency(
    link_with: keysequence_widget_library,
    include_directories: inc,
    compile_args: public_cpp_args)

# vim: set softtabstop=4 shiftwidth=4:
