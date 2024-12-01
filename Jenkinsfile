// Jenkinsfile
node {
    // Tool definitions
    def cmakeHome = tool 'cmake-3.26.0'
    def cppcheckHome = tool 'cppcheck'
    def conanHome = tool 'conan'
    def gcovHome = tool 'gcov'

    // Build configuration
    def buildType = params.BUILD_TYPE ?: 'Release'
    def buildDir = 'build'
    def testDir = 'tests'
    def artifactName = 'myapp'
    def deployDir = '/opt/applications'
    def coverageThreshold = 80
    def sonarProjectKey = 'cpp-project'

    // Utility functions
    def notifyBuild(String buildStatus) {
        buildStatus = buildStatus ?: 'SUCCESS'
        def subject = "Pipeline '${env.JOB_NAME} [${env.BUILD_NUMBER}]' ${buildStatus}"
        def details = """
            Build: ${env.BUILD_URL}
            Build Type: ${buildType}
            Branch: ${env.BRANCH_NAME}
            Status: ${buildStatus}
        """.stripIndent()

        emailext (
            subject: subject,
            body: details,
            recipientProviders: [
                [$class: 'DevelopersRecipientProvider'],
                [$class: 'RequesterRecipientProvider']
            ]
        )
    }

    try {

        stage('Checkout') {
            checkout scm
            if (env.BRANCH_NAME) {
                sh "git checkout ${env.BRANCH_NAME}"
            }
        }

        stage('Setup Build Environment') {
            // Install dependencies using Conan
            sh """
                ${conanHome}/bin/conan install . \
                    --output-folder=${buildDir} \
                    --build=missing \
                    -s build_type=${buildType}
            """

            // Configure CMake
            dir(buildDir) {
                sh """
                    ${cmakeHome}/bin/cmake .. \
                        -DCMAKE_BUILD_TYPE=${buildType} \
                        -DCMAKE_CXX_COMPILER=g++-12 \
                        -DCMAKE_C_COMPILER=gcc-12 \
                        -DENABLE_TESTING=ON \
                        -DENABLE_COVERAGE=ON
                """
            }
        }

        stage('Build') {
            dir(buildDir) {
                sh "make -j\$(nproc)"
            }
        }

        if (params.RUN_TESTS) {
            stage('Unit Tests') {
                try {
                    dir(buildDir) {
                        // Run tests
                        sh "ctest --output-on-failure --verbose"

                        // Generate coverage report
                        sh """
                            ${gcovHome}/bin/gcovr -r .. \
                                --html --html-details \
                                -o coverage_report.html \
                                --exclude-directories='${testDir}'
                        """

                        // Publish test results
                        step([
                            $class: 'XUnitBuilder',
                            thresholds: [
                                [$class: 'FailedThreshold', unstableThreshold: '0'],
                                [$class: 'SkippedThreshold', unstableThreshold: '0']
                            ],
                            tools: [
                                [$class: 'CTestType',
                                 pattern: 'Testing/**/*.xml',
                                 deleteOutputFiles: true,
                                 stopProcessingIfError: true]
                            ]
                        ])

                        // Publish coverage report
                        publishHTML([
                            allowMissing: false,
                            alwaysLinkToLastBuild: true,
                            keepAll: true,
                            reportDir: '.',
                            reportFiles: 'coverage_report.html',
                            reportName: 'Code Coverage Report'
                        ])
                    }
                } catch (Exception e) {
                    currentBuild.result = 'UNSTABLE'
                    error "Test execution failed: ${e.message}"
                }
            }
        }

        if (params.STATIC_ANALYSIS) {
            stage('Static Analysis') {
                parallel (
                    "Cppcheck": {
                        sh """
                            ${cppcheckHome}/bin/cppcheck \
                                --enable=all \
                                --inconclusive \
                                --xml \
                                --output-file=cppcheck_report.xml \
                                .
                        """

                        publishCppcheck(
                            pattern: 'cppcheck_report.xml',
                            ignoreBlankFiles: false
                        )
                    },
                    "SonarQube": {
                        withSonarQubeEnv('SonarQube') {
                            sh """
                                sonar-scanner \
                                    -Dsonar.projectKey=${sonarProjectKey} \
                                    -Dsonar.sources=src \
                                    -Dsonar.tests=${testDir} \
                                    -Dsonar.cpp.file.suffixes=.cpp,.h \
                                    -Dsonar.cpp.cppcheck.reportPath=cppcheck_report.xml \
                                    -Dsonar.coverage.exclusions=**/${testDir}/**
                            """
                        }

                        timeout(time: 10, unit: 'MINUTES') {
                            def qg = waitForQualityGate()
                            if (qg.status != 'OK') {
                                error "Quality gate failed: ${qg.status}"
                            }
                        }
                    }
                )
            }
        }

        stage('Package') {
            dir(buildDir) {
                sh "cpack -G DEB"
                archiveArtifacts(
                    artifacts: "*.deb",
                    fingerprint: true
                )
            }
        }

        stage('Deploy') {
            if (currentBuild.resultIsBetterOrEqualTo('SUCCESS')) {
                def deployScript = "scripts/deploy-${buildType.toLowerCase()}.sh"
                if (fileExists(deployScript)) {
                    sh """
                        chmod +x ${deployScript}
                        ./${deployScript} ${buildDir}/*.deb ${deployDir}
                    """
                } else {
                    error "Deployment script ${deployScript} not found!"
                }
            }
        }

        currentBuild.result = 'SUCCESS'
        notifyBuild('SUCCESS')

    } catch (Exception e) {
        currentBuild.result = 'FAILURE'
        notifyBuild('FAILURE')
        throw e
    } finally {
        // Cleanup
        deleteDir()
    }
}
