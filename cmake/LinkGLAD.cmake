include(FetchContent)

macro (LinkGLAD TARGET ACCESS)
    FetchContent_Declare(
        glad GIT_REPOSITORY https://github.com/Dav1dde/glad GIT_TAG v2.0.4 EXCLUDE_FROM_ALL
                                                                           FIND_PACKAGE_ARGS
    )

    FetchContent_GetProperties(glad)

    if (NOT glad_POPULATED)
        FetchContent_Populate(glad)

        # This excludes glad from being rebuilt when ALL_BUILD is built it will only be built when a
        # target is built that has a dependency on glad
        add_subdirectory(${glad_SOURCE_DIR}/cmake ${glad_BINARY_DIR} EXCLUDE_FROM_ALL)

        # Set the target's folders
        glad_add_library(glad STATIC API gl:compatibility=4.6)
        # glad_add_library(glad REPRODUCIBLE MX API gl:core=3.3)
        # set_target_properties(glad_gl_core_33 PROPERTIES FOLDER ${PROJECT_NAME}/thirdparty)
        # set_target_properties(glad_gl_core_33-generate-files PROPERTIES FOLDER
        # ${PROJECT_NAME}/thirdparty)
    endif ()

    target_include_directories(${TARGET} ${ACCESS} ${glad_SOURCE_DIR}/include)
    target_link_libraries(${TARGET} ${ACCESS} glad)

    add_dependencies(${TARGET} glad)
endmacro ()
