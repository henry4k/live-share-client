project('qhotkey', 'cpp',
    default_options: ['cpp_std=c++11'])

qt5 = import('qt5')

qt_modules = ['Core']
deps = []
inc = include_directories('QHotkey')
moc_headers = ['QHotkey/qhotkey.h',
               'QHotkey/qhotkey_p.h']
sources = ['QHotkey/qhotkey.cpp']

cxx = meson.get_compiler('cpp')
host_system = host_machine.system()
if host_system == 'darwin'
    deps += declare_dependency(link_args: ['-framework', 'Carbon'])
    sources += 'QHotkey/qhotkey_mac.cpp'
elif host_system == 'windows' or host_system == 'cygwin'
    deps += cxx.find_library('user32')
    sources += 'QHotkey/qhotkey_win.cpp'
elif host_system == 'linux' or host_system == 'bsd'
    deps += cxx.find_library('X11')
    qt_modules += 'X11Extras'
    sources += 'QHotkey/qhotkey_x11.cpp'
else
    error('Unsupported host system.')
endif

deps += dependency('qt5', modules: qt_modules)

qhotkey_library = library('qhotkey',
                          sources,
                          qt5.preprocess(moc_headers: moc_headers),
                          dependencies: deps,
                          include_directories: inc,
                          cpp_args: ['-DQHOTKEY_LIB', '-DQHOTKEY_LIB_BUILD'])

qhotkey_dep = declare_dependency(link_with: qhotkey_library,
                                 include_directories: inc,
                                 cpp_args: ['-DQHOTKEY_LIB'])

# vim: set softtabstop=4 shiftwidth=4:
