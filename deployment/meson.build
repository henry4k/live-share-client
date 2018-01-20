bin_dir = get_option('prefix')+get_option('bindir')
client_basename = client_executable.full_path().split('/')[-1]
client_path = bin_dir+'/'+client_basename

mxe_root = meson.get_cross_property('mxe_root', false)
if mxe_root != false
    mxe_target = meson.get_cross_property('mxe_target', false)
    mxe_target_dir = mxe_root+'/usr/'+mxe_target
    mxe_objdump = mxe_root+'/usr/bin/'+mxe_target+'-objdump'
    lib_dir = get_option('prefix')+get_option('libdir')
    build_dir = meson.build_root()

    meson.add_install_script('gather-libraries',
                             '-L', mxe_target_dir+'/bin',
                             '-L', mxe_target_dir+'/qt5/bin',
                             '-L', build_dir+'/subprojects/keysequence_widget',
                             '-L', build_dir+'/subprojects/qhotkey',
                             '--objdump', mxe_objdump,
                             '-o', lib_dir,
                             client_path)

    foreach name : ['platforms',
                    'styles',
                    'imageformats',
                    'generic']
        install_subdir(mxe_target_dir+'/qt5/plugins/'+name,
                       install_dir: name)
    endforeach
endif

upx = find_program('upx', required: false)
if upx.found()
    meson.add_install_script('compress-binaries', upx.path())
endif
