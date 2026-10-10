find_program(P3D_FFMPEG_EXECUTABLE ffmpeg)

if(NOT P3D_FFMPEG_EXECUTABLE)
    message(FATAL_ERROR "ffmpeg not found, required for media conversion. Install ffmpeg or configure with -DP3D_ENABLE_ASSETS=OFF")
endif()

if(NOT P3D_BLOCKSDS_ROOT)
    set(P3D_BLOCKSDS_ROOT "/opt/blocksds/core")
endif()

find_program(P3D_GRIT_EXECUTABLE grit
    HINTS
    ${P3D_BLOCKSDS_ROOT}/tools/grit
    /opt/blocksds/core/tools/grit
    /opt/wonderful/thirdparty/blocksds/core/tools/grit
)

if(NOT P3D_GRIT_EXECUTABLE)
    message(FATAL_ERROR "grit not found, required for graphics conversion. Set BLOCKSDS/P3D_BLOCKSDS_ROOT or configure with -DP3D_ENABLE_ASSETS=OFF")
endif()

file(GLOB_RECURSE P3D_ASSET_TOOL_INPUTS CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/tools/converters/*
)

function(p3d_add_asset_group group_name)
    set(options)
    set(oneValueArgs COMMENT)
    set(multiValueArgs PATTERNS)
    cmake_parse_arguments(P3D_GROUP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT P3D_GROUP_PATTERNS)
        message(FATAL_ERROR "p3d_add_asset_group(${group_name}) requires PATTERNS")
    endif()

    file(GLOB_RECURSE P3D_GROUP_INPUTS CONFIGURE_DEPENDS ${P3D_GROUP_PATTERNS})
    set(P3D_GROUP_STAMP ${CMAKE_BINARY_DIR}/p3d_assets_${group_name}.stamp)
    set(P3D_GROUP_TARGET p3d_assets_${group_name})

    add_custom_command(
        OUTPUT ${P3D_GROUP_STAMP}
        COMMAND ${CMAKE_COMMAND}
        -DP3D_SOURCE_DIR=${CMAKE_SOURCE_DIR}
        -DP3D_PYTHON_EXECUTABLE=${Python3_EXECUTABLE}
        -DP3D_FFMPEG_EXECUTABLE=${P3D_FFMPEG_EXECUTABLE}
        -DP3D_GRIT_EXECUTABLE=${P3D_GRIT_EXECUTABLE}
        -DP3D_ASSET_GROUP=${group_name}
        -P ${CMAKE_SOURCE_DIR}/cmake/BuildAssets.cmake
        COMMAND ${CMAKE_COMMAND} -E touch ${P3D_GROUP_STAMP}
        DEPENDS
        ${P3D_GROUP_INPUTS}
        ${P3D_ASSET_TOOL_INPUTS}
        ${CMAKE_SOURCE_DIR}/tools/build_asset.py
        ${CMAKE_SOURCE_DIR}/cmake/BuildAssets.cmake
        COMMENT "${P3D_GROUP_COMMENT}"
        VERBATIM
    )

    add_custom_target(${P3D_GROUP_TARGET} DEPENDS ${P3D_GROUP_STAMP})
    set_property(GLOBAL APPEND PROPERTY P3D_ASSET_GROUP_TARGETS ${P3D_GROUP_TARGET})
endfunction()

p3d_add_asset_group(dialogue
    COMMENT "Generating dialogue assets"
    PATTERNS
    ${CMAKE_SOURCE_DIR}/assets/dialogue/*.dlg
    ${CMAKE_SOURCE_DIR}/assets/dialogue/*.build.json
)
p3d_add_asset_group(music
    COMMENT "Generating music assets"
    PATTERNS
    ${CMAKE_SOURCE_DIR}/assets/music/*.mp3
    ${CMAKE_SOURCE_DIR}/assets/music/*.build.json
    ${CMAKE_SOURCE_DIR}/assets/music/**/*.mp3
    ${CMAKE_SOURCE_DIR}/assets/music/**/*.build.json
)
p3d_add_asset_group(video
    COMMENT "Generating video assets"
    PATTERNS
    ${CMAKE_SOURCE_DIR}/assets/video/*.mp4
    ${CMAKE_SOURCE_DIR}/assets/video/*.build.json
)
p3d_add_asset_group(environments
    COMMENT "Generating environment assets"
    PATTERNS
    ${CMAKE_SOURCE_DIR}/assets/environments/*/*.obj
    ${CMAKE_SOURCE_DIR}/assets/environments/*/*.png
    ${CMAKE_SOURCE_DIR}/assets/environments/*/*.mtl
    ${CMAKE_SOURCE_DIR}/assets/environments/*/*.build.json
    ${CMAKE_SOURCE_DIR}/assets/environments/*.build.json
)
p3d_add_asset_group(models
    COMMENT "Generating model assets"
    PATTERNS
    ${CMAKE_SOURCE_DIR}/assets/models/*/*.json
    ${CMAKE_SOURCE_DIR}/assets/models/*/*.png
    ${CMAKE_SOURCE_DIR}/assets/models/*/*.build.json
    ${CMAKE_SOURCE_DIR}/assets/models/*.build.json
)
p3d_add_asset_group(maps
    COMMENT "Generating map assets"
    PATTERNS
    ${CMAKE_SOURCE_DIR}/assets/maps/*.jmap
)
p3d_add_asset_group(graphics
    COMMENT "Generating graphics assets"
    PATTERNS
    ${CMAKE_SOURCE_DIR}/assets/graphics/*.png
    ${CMAKE_SOURCE_DIR}/assets/graphics/*.grit
    ${CMAKE_SOURCE_DIR}/assets/environments/*/*.png
    ${CMAKE_SOURCE_DIR}/assets/environments/*/*.grit
    ${CMAKE_SOURCE_DIR}/assets/models/*/*.png
    ${CMAKE_SOURCE_DIR}/assets/models/*/*.grit
)
p3d_add_asset_group(fonts
    COMMENT "Generating font assets"
    PATTERNS
    ${CMAKE_SOURCE_DIR}/assets/fonts/*.png
    ${CMAKE_SOURCE_DIR}/assets/fonts/*.fnt
    ${CMAKE_SOURCE_DIR}/assets/fonts/*.grit
)

add_dependencies(p3d_assets_graphics p3d_assets_environments p3d_assets_models)

add_custom_target(p3d_environment_db DEPENDS p3d_assets_environments)

add_custom_target(p3d_assets)
add_custom_target(p3d_clean_assets
    COMMAND ${CMAKE_COMMAND}
    -DP3D_SOURCE_DIR=${CMAKE_SOURCE_DIR}
    -P ${CMAKE_SOURCE_DIR}/cmake/CleanAssets.cmake
    COMMENT "Cleaning generated asset outputs"
    VERBATIM
)
get_property(P3D_ASSET_GROUP_TARGETS GLOBAL PROPERTY P3D_ASSET_GROUP_TARGETS)

if(P3D_ASSET_GROUP_TARGETS)
    add_dependencies(p3d_assets ${P3D_ASSET_GROUP_TARGETS})
endif()

add_dependencies(p3d_assets p3d_environment_db)

if(TARGET p3d_game)
    add_dependencies(p3d_game p3d_assets)
endif()

set(P3D_ADDITIONAL_CLEAN)

file(GLOB P3D_CURRENT_DATA_ITEMS LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/data/*/*")
list(FILTER P3D_CURRENT_DATA_ITEMS EXCLUDE REGEX "/\\.gitkeep$")
list(FILTER P3D_CURRENT_DATA_ITEMS EXCLUDE REGEX "^${CMAKE_SOURCE_DIR}/data/save")
list(APPEND P3D_ADDITIONAL_CLEAN ${P3D_CURRENT_DATA_ITEMS})

file(GLOB P3D_CURRENT_DATA_FILES LIST_DIRECTORIES false "${CMAKE_SOURCE_DIR}/data/*")
list(APPEND P3D_ADDITIONAL_CLEAN ${P3D_CURRENT_DATA_FILES})

file(GLOB P3D_ASSET_ENV_DIRS LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/assets/environments/*")
foreach(env_dir IN LISTS P3D_ASSET_ENV_DIRS)
    if(IS_DIRECTORY "${env_dir}")
        get_filename_component(env_name "${env_dir}" NAME)
        list(APPEND P3D_ADDITIONAL_CLEAN "${CMAKE_SOURCE_DIR}/data/environments/${env_name}")
    endif()
endforeach()
list(APPEND P3D_ADDITIONAL_CLEAN "${CMAKE_SOURCE_DIR}/source/data/environmentDb.cpp")

file(GLOB P3D_ASSET_MODEL_DIRS LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/assets/models/*")
foreach(model_dir IN LISTS P3D_ASSET_MODEL_DIRS)
    if(IS_DIRECTORY "${model_dir}")
        get_filename_component(model_name "${model_dir}" NAME)
        list(APPEND P3D_ADDITIONAL_CLEAN
            "${CMAKE_SOURCE_DIR}/data/models/${model_name}"
            "${CMAKE_SOURCE_DIR}/source/models/${model_name}.hpp"
        )
    endif()
endforeach()

file(GLOB P3D_ASSET_GRAPHIC_DIRS LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/assets/graphics/*")
foreach(g_dir IN LISTS P3D_ASSET_GRAPHIC_DIRS)
    if(IS_DIRECTORY "${g_dir}")
        get_filename_component(g_name "${g_dir}" NAME)
        list(APPEND P3D_ADDITIONAL_CLEAN "${CMAKE_SOURCE_DIR}/data/graphics/${g_name}")
    endif()
endforeach()

file(GLOB P3D_ASSET_FONT_DIRS LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/assets/fonts/*")
foreach(f_dir IN LISTS P3D_ASSET_FONT_DIRS)
    if(IS_DIRECTORY "${f_dir}")
        get_filename_component(f_name "${f_dir}" NAME)
        list(APPEND P3D_ADDITIONAL_CLEAN "${CMAKE_SOURCE_DIR}/data/fonts/${f_name}")
    endif()
endforeach()

file(GLOB P3D_ASSET_MUSIC_DIRS LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/assets/music/*")
foreach(m_dir IN LISTS P3D_ASSET_MUSIC_DIRS)
    if(IS_DIRECTORY "${m_dir}")
        get_filename_component(m_name "${m_dir}" NAME)
        list(APPEND P3D_ADDITIONAL_CLEAN "${CMAKE_SOURCE_DIR}/data/music/${m_name}")
    endif()
endforeach()

file(GLOB_RECURSE P3D_ASSET_MP3_FILES "${CMAKE_SOURCE_DIR}/assets/music/*.mp3")
foreach(mp3 IN LISTS P3D_ASSET_MP3_FILES)
    file(RELATIVE_PATH rel "${CMAKE_SOURCE_DIR}/assets/music" "${mp3}")
    string(REGEX REPLACE "\\.mp3$" ".qoa" rel_qoa "${rel}")
    string(REGEX REPLACE "\\.mp3$" ".pcm" rel_pcm "${rel}")
    list(APPEND P3D_ADDITIONAL_CLEAN
        "${CMAKE_SOURCE_DIR}/data/music/${rel_qoa}"
        "${CMAKE_SOURCE_DIR}/data/music/${rel_pcm}"
    )
endforeach()

file(GLOB P3D_ASSET_VIDEO_FILES "${CMAKE_SOURCE_DIR}/assets/video/*.mp4")
foreach(mp4 IN LISTS P3D_ASSET_VIDEO_FILES)
    get_filename_component(stem "${mp4}" NAME_WE)
    list(APPEND P3D_ADDITIONAL_CLEAN "${CMAKE_SOURCE_DIR}/data/video/${stem}.vid")
endforeach()

file(GLOB P3D_ASSET_JMAP_FILES "${CMAKE_SOURCE_DIR}/assets/maps/*.jmap")
foreach(jmap IN LISTS P3D_ASSET_JMAP_FILES)
    get_filename_component(stem "${jmap}" NAME_WE)
    list(APPEND P3D_ADDITIONAL_CLEAN "${CMAKE_SOURCE_DIR}/source/maps/${stem}.hpp")
endforeach()

file(GLOB P3D_ASSET_DLG_FILES "${CMAKE_SOURCE_DIR}/assets/dialogue/*.dlg")
foreach(dlg IN LISTS P3D_ASSET_DLG_FILES)
    get_filename_component(stem "${dlg}" NAME_WE)
    list(APPEND P3D_ADDITIONAL_CLEAN
        "${CMAKE_SOURCE_DIR}/source/dialogue/${stem}_dialogue.cpp"
        "${CMAKE_SOURCE_DIR}/source/dialogue/${stem}_dialogue.h"
        "${CMAKE_SOURCE_DIR}/source/dialogue/${stem}_dialogue.hpp"
    )
endforeach()

list(APPEND P3D_ADDITIONAL_CLEAN
    "${CMAKE_SOURCE_DIR}/sdcard.img"
    "${CMAKE_SOURCE_DIR}/sdcard.img.idx"
)

list(REMOVE_DUPLICATES P3D_ADDITIONAL_CLEAN)
set_property(DIRECTORY "${CMAKE_SOURCE_DIR}" APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${P3D_ADDITIONAL_CLEAN})
set_target_properties(p3d_clean_assets PROPERTIES ADDITIONAL_CLEAN_FILES "${P3D_ADDITIONAL_CLEAN}")
