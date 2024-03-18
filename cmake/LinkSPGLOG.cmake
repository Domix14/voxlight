include(FetchContent)

macro (LinkSPGLOG TARGET ACCESS)
    FetchContent_Declare(
        spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG v1.x EXCLUDE_FROM_ALL
                                                                                FIND_PACKAGE_ARGS
    )
    FetchContent_MakeAvailable(spdlog)

    target_link_libraries(${TARGET} ${ACCESS} spdlog)
endmacro ()
