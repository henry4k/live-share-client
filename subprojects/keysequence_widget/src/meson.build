deps = []
inc = include_directories('.')
moc_headers = ['keysequence_widget.h',
               'keysequence_listwidget.h']
sources = ['keysequence_widget.cpp',
           'keysequence_listwidget.cpp']

deps += dependency('qt5', modules: ['Core', 'Widgets'])

keysequence_widget_library = library(
    'keysequence_widget',
    sources,
    qt5.preprocess(moc_headers: moc_headers),
    dependencies: deps,
    include_directories: inc)

keysequence_widget_dep = declare_dependency(
    link_with: keysequence_widget_library,
    include_directories: inc)

# vim: set softtabstop=4 shiftwidth=4:
