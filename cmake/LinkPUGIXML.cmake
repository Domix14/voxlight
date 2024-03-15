include(FetchContent)

macro (LinkPUGIXML TARGET ACCESS)
    FetchContent_Declare(pugixml GIT_REPOSITORY https://github.com/zeux/pugixml.git GIT_TAG v1.14)

    FetchContent_MakeAvailable(pugixml)

    target_link_libraries(${TARGET} ${ACCESS} pugixml::pugixml)
endmacro ()
