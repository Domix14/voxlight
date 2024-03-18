include(FetchContent)

macro (LinkENTT TARGET ACCESS)
    FetchContent_Declare(
        entt GIT_REPOSITORY https://github.com/skypjack/entt GIT_TAG v3.12.x EXCLUDE_FROM_ALL
                                                                             FIND_PACKAGE_ARGS
    )
    FetchContent_MakeAvailable(entt)

    target_link_libraries(${TARGET} ${ACCESS} EnTT)
endmacro ()
