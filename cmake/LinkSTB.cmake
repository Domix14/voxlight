include(FetchContent)

macro (LinkSTB TARGET ACCESS)
    FetchContent_Declare(
        stb GIT_REPOSITORY https://github.com/nothings/stb
        GIT_TAG b42009b3b9d4ca35bc703f5310eedc74f584be58 EXCLUDE_FROM_ALL FIND_PACKAGE_ARGS
    )

    FetchContent_GetProperties(stb)

    if (NOT stb_POPULATED)
        FetchContent_Populate(stb)
    endif ()

    add_library(stb INTERFACE)
    target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

    target_link_libraries(${TARGET} ${ACCESS} stb)
    # target_include_directories(${TARGET} ${ACCESS} ${stb_SOURCE_DIR})
endmacro ()
