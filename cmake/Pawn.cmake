set(PAWN_ROOT "${CMAKE_SOURCE_DIR}/libs/pawn")

if(EXISTS "${PAWN_ROOT}")
    set(PAWN_AMX_SOURCES
        "${PAWN_ROOT}/amx/amx.c"
        "${PAWN_ROOT}/amx/amxcore.c"
        "${PAWN_ROOT}/amx/amxcons.c"
        "${PAWN_ROOT}/amx/amxstring.c"
        "${PAWN_ROOT}/amx/amxfixed.c"
        "${PAWN_ROOT}/amx/amxaux.c"
    )

    add_library(pawn_amx STATIC ${PAWN_AMX_SOURCES})
    add_library(pawn::amx ALIAS pawn_amx)

    target_include_directories(pawn_amx PUBLIC
        $<BUILD_INTERFACE:${PAWN_ROOT}/amx>
        $<BUILD_INTERFACE:${PAWN_ROOT}/include>
    )

    target_compile_definitions(pawn_amx PUBLIC
        # TODO: See if the ARM assembly version of AMX is worth it...
        # AMX_ASM=1
        FIXEDPOINT
        PAWN_CELL_SIZE=32
        HAVE_STDINT_H=1
        AMX_ANSIONLY            # UTF-8 strings
        AMX_NODYNALOAD          # Disable dynamic loading
        AMX_NOPROPLIST          # Reduce unnecessary mallocs
        AMX_TOKENTHREADING
        AMX_NO_OVERLAY          # Overlay might not be needed??
    )

    # Suppress warnings from Pawn source code
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(pawn_amx PRIVATE
            -Wno-unused-parameter
            -Wno-sign-compare
            -Wno-unused-function
        )
    endif()

    # Ensure host Pawn compiler executable is available
    include(cmake/PawnCompiler.cmake)
    p3d_ensure_pawncc()
else()
    message(WARNING "Pawn root directory not found at ${PAWN_ROOT}")
endif()
