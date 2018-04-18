properties([
    buildDiscarder(logRotator(numToKeepStr: '4')),
    parameters([
        choice(name: 'buildType',
               choices: 'plain\ndebug\ndebugoptimized\nrelease\nminsize',
               description: 'Defines general compiler flags.')
    ])
])

def createCacheDir() {
    def cacheDir = "${env.HOME}/.crosscompile-cache"
    sh "mkdir -p '${cacheDir}'"
    return cacheDir
}

def withDockerImage(imageName, closure) {
    node('4bf288b3ce') {
        def cacheDir = createCacheDir()
        def args = "--mount 'type=bind,source=${cacheDir},destination=/cache'"
        docker.image(imageName).inside(args, closure)
    }
}

def buildStage(config) {
    withDockerImage('henry4k/crossbuild:1.0') {
        try {
            stage("Configure ${config.name}") {
                unstash 'source'
                sh 'mkdir build'
                sh 'mkdir dist'
                sh "conan remote add sogilis 'https://api.bintray.com/conan/sogilis/testing'"
                sh "conan install --profile ${config.triple} "+
                                 '--build=outdated '+
                                 '--install-folder=$PWD/build '+
                                 '$PWD/source'
                sh 'PKG_CONFIG_PATH=$PWD/build '+
                   'OSXCROSS_PKG_CONFIG_PATH=$PWD/build '+
                   'meson --libdir= '+
                         '--bindir= '+
                         '--datadir=resources '+
                         "--buildtype ${params.buildType} "+
                         '--prefix $PWD/dist '+
                         '--warnlevel 3 '+
                         "--cross-file ${config.triple} "+
                         'build '+
                         '$PWD/source'
            }

            stage("Build ${config.name}") {
                sh 'ninja -C build'
            }

            stage("Test ${config.name}") {
                sh 'ninja -C build test'
            }

            stage("Package ${config.name}") {
                sh 'ninja -C build install'
                dir('dist') {
                    stash name: config.name, includes: '**'
                }
            }
        }
        finally {
            sh 'rm -rf build dist source'
        }
    }
}

def buildConfigs = [[name: 'windows-x64', triple: 'x86_64-w64-mingw32']/*,
                    [name: 'apple-x64',   triple: 'x86_64-apple-darwin14']*/]

node {
    try {
        stage('Checkout') {
            dir('source') {
                checkout scm
            }
            stash name: 'source', includes: 'source/'
        }

        stage('Build') {
            def stages = [:]
            for (int i = 0; i < buildConfigs.size(); i++) {
                def config = buildConfigs[i]
                stages[config.name] = {
                    buildStage(config)
                }
            }

            stages.failFast = true
            parallel stages
        }

        stage('Check Logs') {
            step([$class: 'WarningsPublisher',
                consoleParsers: [[
                    parserName: 'GNU compiler (gcc)'
                ]],
                unstableTotalAll: '0',
                usePreviousBuildAsReference: true
            ])
        }

        stage('Package') {
            dir('dist') {
                for (int i = 0; i < buildConfigs.size(); i++) {
                    def config = buildConfigs[i]
                    dir(config.name) {
                        unstash config.name
                    }
                }
                archiveArtifacts artifacts: '**', fingerprint: true
            }
        }
    }
    finally {
        sh 'rm -rf source dist'
    }
}

// vim: set filetype=groovy:
