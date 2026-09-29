set(P3D_SOURCE_DIR "${P3D_SOURCE_DIR}" CACHE PATH "Project source directory")

if(NOT P3D_SOURCE_DIR)
    message(FATAL_ERROR "P3D_SOURCE_DIR must be provided")
endif()

file(GLOB P3D_DATA_ENTRIES LIST_DIRECTORIES true "${P3D_SOURCE_DIR}/data/*")
foreach(data_entry IN LISTS P3D_DATA_ENTRIES)
    if(IS_DIRECTORY "${data_entry}")
        get_filename_component(dir_name "${data_entry}" NAME)
        if(NOT dir_name STREQUAL "save")
            file(GLOB P3D_SUB_ENTRIES LIST_DIRECTORIES true "${data_entry}/*")
            list(FILTER P3D_SUB_ENTRIES EXCLUDE REGEX "/\\.gitkeep$")
            if(P3D_SUB_ENTRIES)
                file(REMOVE_RECURSE ${P3D_SUB_ENTRIES})
            endif()
        endif()
    else()
        file(REMOVE "${data_entry}")
    endif()
endforeach()

file(GLOB P3D_MAP_OUTPUTS "${P3D_SOURCE_DIR}/source/maps/*.hpp")
file(GLOB P3D_MODEL_OUTPUTS "${P3D_SOURCE_DIR}/source/models/*.hpp")
file(GLOB P3D_DIALOGUE_OUTPUTS
    "${P3D_SOURCE_DIR}/source/dialogue/*_dialogue.cpp"
    "${P3D_SOURCE_DIR}/source/dialogue/*_dialogue.h"
    "${P3D_SOURCE_DIR}/source/dialogue/*_dialogue.hpp"
)

set(P3D_SOURCE_OUTPUTS
    ${P3D_MAP_OUTPUTS}
    ${P3D_MODEL_OUTPUTS}
    ${P3D_DIALOGUE_OUTPUTS}
)
if(P3D_SOURCE_OUTPUTS)
    file(REMOVE ${P3D_SOURCE_OUTPUTS})
endif()

file(GLOB P3D_ASSET_STAMPS
    "${P3D_SOURCE_DIR}/out/build/*/p3d_assets_*.stamp"
    "${CMAKE_BINARY_DIR}/p3d_assets_*.stamp"
)
if(P3D_ASSET_STAMPS)
    file(REMOVE ${P3D_ASSET_STAMPS})
endif()

if(EXISTS "${P3D_SOURCE_DIR}/sdcard.img")
    file(REMOVE "${P3D_SOURCE_DIR}/sdcard.img")
endif()
if(EXISTS "${P3D_SOURCE_DIR}/sdcard.img.idx")
    file(REMOVE "${P3D_SOURCE_DIR}/sdcard.img.idx")
endif()

if(EXISTS "${P3D_SOURCE_DIR}/build")
    file(REMOVE_RECURSE "${P3D_SOURCE_DIR}/build")
endif()

message(STATUS "Generated asset outputs cleaned")
