deps = []
moc_headers = ['keysequence_listwidget_plugin.h',
               'keysequence_widget_plugin.h',
               'keysequence_widget_plugin_collection.h']
sources = ['keysequence_listwidget_plugin.cpp',
           'keysequence_widget_plugin.cpp',
           'keysequence_widget_plugin_collection.cpp']

deps += dependency('qt5', modules: ['Core', 'Widgets', 'Designer'])
deps += keysequence_widget_dep

designer_plugin = shared_module(
    'keysequence_widget',
    sources,
    qt5.preprocess(moc_headers: moc_headers),
    dependencies: deps)

# vim: set softtabstop=4 shiftwidth=4:
