project('live-share-client', 'cpp')
qt5 = import('qt5')
executable('live-share-client',
           'main.cpp',
           'Recorder.cpp',
           'RecordingManager.cpp',
           'WindowInfo.cpp',
           'MainWindow.cpp',
           'ExecutableValidator.cpp',
           qt5.preprocess(moc_headers: ['Recorder.h',
                                        'RecordingManager.h',
                                        'MainWindow.h',
                                        'ExecutableValidator.h'],
                          ui_files: 'MainWindow.ui'),
           gui_app: true,
           dependencies: [dependency('qt5', modules: ['Core', 'Gui', 'Widgets', 'Network']),
                          subproject('qhotkey').get_variable('qhotkey_dep'),
                          subproject('keysequence_widget').get_variable('keysequence_widget_dep')])
