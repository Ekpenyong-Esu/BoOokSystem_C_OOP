add_executable("UnitTestBookManage" "test_book_management.c")
target_link_libraries("UnitTestBookManage" PUBLIC "LibBookManagement")
target_link_libraries("UnitTestBookManage" PRIVATE unity)

add_executable("UnitTestBar" "test_bar.c")
target_link_libraries("UnitTestBar" PUBLIC "LibBar")
target_link_libraries("UnitTestBar" PRIVATE unity)

add_test(NAME "RunUnitTestBookManage" COMMAND "UnitTestBookManage")
add_test(NAME "RunUnitTestBar" COMMAND "UnitTestBar")

if(${ENABLE_WARNINGS})
    target_set_warnings(
        TARGET
        "UnitTestBookManage"
        ENABLE
        ${ENABLE_WARNINGS}
        AS_ERRORS
        ${ENABLE_WARNINGS_AS_ERRORS})
    target_set_warnings(
        TARGET
        "UnitTestBar"
        ENABLE
        ${ENABLE_WARNINGS}
        AS_ERRORS
        ${ENABLE_WARNINGS_AS_ERRORS})
endif()

if(ENABLE_COVERAGE)
    set(COVERAGE_MAIN "coverage")
    set(COVERAGE_EXCLUDES
        "${PROJECT_SOURCE_DIR}/app/*"
        "${PROJECT_SOURCE_DIR}/cmake/*"
        "${PROJECT_SOURCE_DIR}/docs/*"
        "${PROJECT_SOURCE_DIR}/external/*"
        "${PROJECT_SOURCE_DIR}/tests/*"
        "${PROJECT_SOURCE_DIR}/build/*"
        "/usr/include/*")
    set(COVERAGE_EXTRA_FLAGS)
    set(COVERAGE_DEPENDENCIES "UnitTestBookManage" "UnitTestBar")

    setup_target_for_coverage_gcovr_html(
        NAME
        ${COVERAGE_MAIN}
        EXECUTABLE
        ctest
        DEPENDENCIES
        ${COVERAGE_DEPENDENCIES}
        BASE_DIRECTORY
        ${CMAKE_SOURCE_DIR})
endif()
