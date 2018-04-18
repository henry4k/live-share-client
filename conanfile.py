from conans import ConanFile
from shutil import move

class LiveShareClient(ConanFile):
    name = 'live-share-client'
    requires = 'Qt/[~=5]@henry4k/testing'
    generators = 'pkg_config'

    def imports(self):
        move('Qt.pc', 'qt5.pc') # Hack hack hack
