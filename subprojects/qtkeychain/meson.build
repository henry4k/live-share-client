project('qtkeychain', 'cpp',
    default_options: ['cpp_std=c++11'])

qt5 = import('qt5')

qt_modules = ['Core']
deps = []
inc = include_directories('.')
moc_headers = ['keychain.h',
               'keychain_p.h']
sources = ['keychain.cpp']
public_cpp_args = []
cpp_args = []

cxx = meson.get_compiler('cpp')
host_system = host_machine.system()
if host_system == 'darwin'
    deps += cxx.find_library('CoreFoundation')
    deps += cxx.find_library('Security')
    sources += ['keychain_mac.cpp']
elif host_system == 'windows' or host_system == 'cygwin'
    cpp_args += ['-DUSE_CREDENTIAL_STORE']
    deps += cxx.find_library('crypt32')
    moc_headers += ['plaintextstore_p.h']
    sources += ['keychain_win.cpp',
                'plaintextstore.cpp']
    #FIXME: mingw bug; otherwise getting undefined refs to RtlSecureZeroMemory there
    if cxx.get_id() == 'gcc'
        cpp_args += ['-O2']
    endif
elif host_system == 'linux' or host_system == 'bsd'
    qt_modules += 'DBus'
    deps += dependency('libsecret-1')
    moc_headers += ['gnomekeyring_p.h',
                    'libsecret_p.h',
                    'plaintextstore_p.h']
    sources += ['keychain_unix.cpp',
                'gnomekeyring.cpp',
                'libsecret.cpp',
                'plaintextstore.cpp']

    qdbusxml2cpp = find_program('qdbusxml2cpp-qt5', 'qdbusxml2cpp')
    gen_dbus_interface = find_program(files('gen-dbus-interface'))
    kwallet_interface = custom_target('kwallet_interface',
        command: [gen_dbus_interface,
                  '@OUTDIR@',
                  '@INPUT@',
                  qdbusxml2cpp,
                  '--proxy', 'kwallet_interface',
                  '--classname', 'KWalletInterface'],
        input: ['org.kde.KWallet.xml'],
        output: ['kwallet_interface.h',
                 'kwallet_interface.cpp'])
    sources     += [kwallet_interface[1]] # .cpp
    moc_headers += [kwallet_interface.full_path()] # .h
    # FIXME: This is a hack as qt5.preprocess does not accept CustomTargetIndex objects.
else
    error('Unsupported host system.')
endif

deps += dependency('qt5', modules: qt_modules)

if get_option('default_library') == 'shared'
    public_cpp_args += ['-DQTKEYCHAIN_SHARED']
    cpp_args += ['-DQTKEYCHAIN_BUILDING_SHARED']
endif

#subdir('translations')

qtkeychain_library = library('qtkeychain',
                             sources,
                             qt5.preprocess(moc_headers: moc_headers),
                             dependencies: deps,
                             include_directories: inc,
                             cpp_args: [public_cpp_args, cpp_args])

qtkeychain_dep = declare_dependency(link_with: qtkeychain_library,
                                    include_directories: inc,
                                    compile_args: public_cpp_args)

# vim: set softtabstop=4 shiftwidth=4:
