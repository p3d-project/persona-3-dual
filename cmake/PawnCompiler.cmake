# Automatically discovers or builds the host Pawn compiler (pawncc) and disassembler (pawndisasm).

option(P3D_REBUILD_PAWNCC "Force rebuild of host Pawn compiler tools" OFF)

function(p3d_ensure_pawncc)
    set(PAWN_ROOT "${CMAKE_SOURCE_DIR}/libs/pawn")
    set(PAWN_BIN_DIR "${PAWN_ROOT}/bin")
    set(PAWNCC_BUILD_DIR "${CMAKE_SOURCE_DIR}/out/build/pawn-compiler-${CMAKE_HOST_SYSTEM_NAME}")

    # Check if a force rebuild was requested
    if(P3D_REBUILD_PAWNCC)
        message(STATUS "P3D_REBUILD_PAWNCC is ON. Rebuilding host Pawn compiler...")
        unset(P3D_PAWNCC_EXECUTABLE CACHE)
        unset(P3D_PAWNCC_EXECUTABLE)
    endif()

    # If already set in cache, verify that the binary still exists and can execute on this OS
    if(P3D_PAWNCC_EXECUTABLE AND EXISTS "${P3D_PAWNCC_EXECUTABLE}")
        execute_process(
            COMMAND "${P3D_PAWNCC_EXECUTABLE}"
            RESULT_VARIABLE _cached_res
            OUTPUT_VARIABLE _cached_out
            ERROR_VARIABLE _cached_out
        )
        if(_cached_out MATCHES "Pawn compiler")
            return()
        endif()
        unset(P3D_PAWNCC_EXECUTABLE CACHE)
        unset(P3D_PAWNCC_EXECUTABLE)
    endif()

    if(NOT P3D_REBUILD_PAWNCC)
        set(_candidate_hints
            /usr/local/bin
            /opt/pawn/bin
            "${PAWN_BIN_DIR}"
            "${PAWNCC_BUILD_DIR}"
            "${PAWNCC_BUILD_DIR}/Release"
        )

        find_program(_FOUND_PAWNCC pawncc
            HINTS ${_candidate_hints}
        )

        if(_FOUND_PAWNCC)
            execute_process(
                COMMAND "${_FOUND_PAWNCC}"
                RESULT_VARIABLE _probe_res
                OUTPUT_VARIABLE _probe_out
                ERROR_VARIABLE _probe_out
            )
            if(_probe_out MATCHES "Pawn compiler")
                set(P3D_PAWNCC_EXECUTABLE "${_FOUND_PAWNCC}" CACHE FILEPATH "Path to host Pawn compiler" FORCE)
                message(STATUS "Found host Pawn compiler: ${P3D_PAWNCC_EXECUTABLE}")
                return()
            endif()
            unset(_FOUND_PAWNCC CACHE)
        endif()
    endif()

    # Auto-build compiler
    if(NOT EXISTS "${PAWN_ROOT}/compiler/CMakeLists.txt")
        message(FATAL_ERROR "Pawn compiler source not found at ${PAWN_ROOT}/compiler.")
    endif()

    message(STATUS "Host Pawn compiler (pawncc) not found or incompatible. Building automatically using host toolchain...")

    if(P3D_REBUILD_PAWNCC AND EXISTS "${PAWNCC_BUILD_DIR}")
        file(REMOVE_RECURSE "${PAWNCC_BUILD_DIR}")
    endif()

    file(MAKE_DIRECTORY "${PAWNCC_BUILD_DIR}")
    file(MAKE_DIRECTORY "${PAWN_BIN_DIR}")

    # Configure with host toolchain
    execute_process(
        COMMAND ${CMAKE_COMMAND}
            -S "${PAWN_ROOT}/compiler"
            -B "${PAWNCC_BUILD_DIR}"
            -DCMAKE_BUILD_TYPE=Release
            -DCMAKE_TOOLCHAIN_FILE=
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        RESULT_VARIABLE _config_res
        OUTPUT_VARIABLE _config_out
        ERROR_VARIABLE _config_out
    )
    if(NOT _config_res EQUAL 0)
        message(FATAL_ERROR "Failed to configure host Pawn compiler:\n${_config_out}")
    endif()

    # Build pawncc and pawndisasm
    execute_process(
        COMMAND ${CMAKE_COMMAND}
            --build "${PAWNCC_BUILD_DIR}"
            --config Release
            --target pawncc pawndisasm
            --parallel
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        RESULT_VARIABLE _build_res
        OUTPUT_VARIABLE _build_out
        ERROR_VARIABLE _build_out
    )
    if(NOT _build_res EQUAL 0)
        message(FATAL_ERROR "Failed to build host Pawn compiler:\n${_build_out}")
    endif()

    # Locate the built binaries
    find_program(_BUILT_PAWNCC pawncc
        PATHS
        "${PAWNCC_BUILD_DIR}"
        "${PAWNCC_BUILD_DIR}/Release"
        NO_DEFAULT_PATH
    )
    find_program(_BUILT_PAWNDISASM pawndisasm
        PATHS
        "${PAWNCC_BUILD_DIR}"
        "${PAWNCC_BUILD_DIR}/Release"
        NO_DEFAULT_PATH
    )

    if(NOT _BUILT_PAWNCC)
        message(FATAL_ERROR "Could not locate built pawncc executable in ${PAWNCC_BUILD_DIR}")
    endif()

    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_BUILT_PAWNCC}" "${PAWN_BIN_DIR}/pawncc${CMAKE_EXECUTABLE_SUFFIX}")
    if(_BUILT_PAWNDISASM)
        execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_BUILT_PAWNDISASM}" "${PAWN_BIN_DIR}/pawndisasm${CMAKE_EXECUTABLE_SUFFIX}")
    endif()

    if(UNIX)
        execute_process(COMMAND chmod +x "${PAWN_BIN_DIR}/pawncc" ERROR_QUIET)
        if(_BUILT_PAWNDISASM)
            execute_process(COMMAND chmod +x "${PAWN_BIN_DIR}/pawndisasm" ERROR_QUIET)
        endif()
    endif()

    set(P3D_PAWNCC_EXECUTABLE "${PAWN_BIN_DIR}/pawncc${CMAKE_EXECUTABLE_SUFFIX}" CACHE FILEPATH "Path to host Pawn compiler" FORCE)
    set(P3D_REBUILD_PAWNCC OFF CACHE BOOL "Force rebuild of host Pawn compiler tools" FORCE)
    message(STATUS "Host Pawn compiler successfully built: ${P3D_PAWNCC_EXECUTABLE}")
endfunction()
