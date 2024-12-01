set(LIBRARY_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/member_management.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../bookManagement/book_management.c")

set(LIBRARY_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/member_management.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../bookManagement/book_management.h")
set(LIBRARY_INCLUDES "./" "${CMAKE_BINARY_DIR}/configured_files/include")

add_library("LibMemberManagement" STATIC ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})
target_include_directories("LibMemberManagement" PUBLIC ${LIBRARY_INCLUDES})

if(${ENABLE_WARNINGS})
    target_set_warnings(
        TARGET
        "LibMemberManagement"
        ENABLE
        ${ENABLE_WARNINGS}
        AS_ERRORS
        ${ENABLE_WARNINGS_AS_ERRORS})
endif()

if(${ENABLE_LTO})
    target_enable_lto(
        TARGET
        "LibMemberManagement"
        ENABLE
        ON)
endif()

if(${ENABLE_CLANG_TIDY})
    add_clang_tidy_to_target("LibMemberManagement")
endif()
