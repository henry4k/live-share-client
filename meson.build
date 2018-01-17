project('live-share-client', 'cpp')

qt5 = import('qt5')

sources = ['main.cpp',
           'Recorder.cpp',
           'RecordingManager.cpp',
           'Upload.cpp',
           'UploadManager.cpp',
           'UploadQueueEntry.cpp',
           'UploadQueueWidget.cpp',
           'MainWindow.cpp',
           'ExecutableValidator.cpp']

moc_headers = ['Recorder.h',
               'RecordingManager.h',
               'Upload.h',
               'UploadManager.h',
               'UploadQueueEntry.h',
               'UploadQueueWidget.h',
               'MainWindow.h',
               'ExecutableValidator.h']

dependencies = [dependency('qt5', modules: ['Core', 'Gui', 'Widgets', 'Network']),
                subproject('qhotkey').get_variable('qhotkey_dep'),
                subproject('keysequence_widget').get_variable('keysequence_widget_dep')]

host_system = host_machine.system()
if host_system == 'windows'
    sources += 'WindowInfo_win.cpp'
elif host_system == 'darwin'
    sources += 'WindowInfo_cocoa.cpp'
else
    dependencies += dependency('x11')
    sources += 'WindowInfo_x11.cpp'
endif

executable('live-share-client',
           sources,
           qt5.preprocess(moc_headers: moc_headers, ui_files: 'MainWindow.ui'),
           gui_app: true,
           dependencies: dependencies)
