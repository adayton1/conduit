# Copyright (c) Lawrence Livermore National Security, LLC and other Conduit
# Project developers. See top-level LICENSE AND COPYRIGHT files for dates and
# other details. No copyright assignment is required to contribute to Conduit.

################################################################
# if BLT_SOURCE_DIR is not set - use "blt" as default
################################################################
if(NOT BLT_SOURCE_DIR)
    set(BLT_SOURCE_DIR "blt")
endif()

################################################################
# if not set, prefer c++14 lang standard
################################################################
if(NOT BLT_CXX_STD)
    set(BLT_CXX_STD "c++14" CACHE STRING "")
endif()

################################################################
# if not set, prefer folder grouped targets
################################################################
if(NOT ENABLE_FOLDERS)
    set(ENABLE_FOLDERS TRUE CACHE STRING "")
endif()

################################################################
# init blt using BLT_SOURCE_DIR
################################################################
include(${BLT_SOURCE_DIR}/SetupBLT.cmake)

if(ENABLE_MPI)
    set(CONDUIT_MPI_USES_CMAKE_TARGETS OFF CACHE BOOL "")

    # on some platforms (mostly cray systems) folks skip mpi
    # detection in BLT by setting ENABLE_FIND_MPI = OFF
    # in these cases, we need to set MPI_FOUND = TRUE,
    # since the rest of our cmake logic to include MPI uses MPI_FOUND
    if(NOT ENABLE_FIND_MPI)
        set(MPI_FOUND ON CACHE BOOL "")
    endif()

    # if we are using BLT's enable mpi, then we must
    # make sure the MPI targets exist
    if(ENABLE_FIND_MPI)
        if(TARGET MPI::MPI_CXX)
            set(CONDUIT_USE_CMAKE_MPI_TARGETS TRUE CACHE BOOL "")
            message(STATUS "Using MPI CMake imported target: MPI::MPI_CXX")
            # use cmake mpi targets directly
            set(conduit_blt_mpi_deps MPI::MPI_CXX CACHE STRING "")
        else()
            message(FATAL_ERROR "Cannot use CMake imported targets for MPI."
                                "(ENABLE_MPI == ON, but "
                                "MPI::MPI_CXX CMake target is missing.)")
        endif()
    else()
        set(CONDUIT_USE_CMAKE_MPI_TARGETS FALSE CACHE BOOL "")
        # compiler will handle them implicitly
        set(conduit_blt_mpi_deps "" CACHE STRING "")
    endif()

    # In some cases (mpich?) -fallow-argument-mismatch will be
    # reported as a needed MPI flag for fortran.
    # BLT fuses all MPI compiler flags into one big bunch.
    # (It does not differentiate between C and fortran flags)
    #
    # -fallow-argument-mismatch is a fortran compiler flag that makes clang
    # very unhappy, and this will cause blt's mpi smoke test to fail to build
    # with clang.
    #
    # Conduit does not use mpi fortran, so we strip this flag if it exists.
    #
    # blt's mpi target is called "mpi"
    if(TARGET mpi)
    # check and strip interface compile opts
        get_target_property(_mpi_iface_compile_opts mpi INTERFACE_COMPILE_OPTIONS)
        if(_mpi_iface_compile_opts)
            list(REMOVE_ITEM _mpi_iface_compile_opts "-fallow-argument-mismatch")
            list(REMOVE_ITEM _mpi_iface_compile_opts "-fallow-invalid-boz")
            set_target_properties(mpi PROPERTIES INTERFACE_COMPILE_OPTIONS "${_mpi_iface_compile_opts}")
        endif()
    endif()

endif()

if(ENABLE_OPENMP)
    if(TARGET OpenMP::OpenMP_CXX)
        set(CONDUIT_USE_CMAKE_OPENMP_TARGETS TRUE CACHE BOOL "")
        message(STATUS "Using OpenMP CMake imported target: OpenMP::OpenMP_CXX")
        # use cmake openmp targets directly
        set(conduit_blt_openmp_deps OpenMP::OpenMP_CXX CACHE STRING "")
    else()
        message(FATAL_ERROR "Cannot use CMake imported targets for OpenMP."
                            "(ENABLE_OPENMP == ON, but "
                            "OpenMP::OpenMP_CXX CMake target is missing.)")
    endif()
endif()


################################################################
# apply folders to a few ungrouped blt targets
################################################################

###############################################
# group main blt docs targets into docs folder
###############################################
blt_set_target_folder( TARGET docs FOLDER docs)

if(TARGET sphinx_docs)
    blt_set_target_folder( TARGET sphinx_docs FOLDER docs)
endif()

if(TARGET doxygen_docs)
    blt_set_target_folder( TARGET doxygen_docs FOLDER docs)
endif()

####################################################
# group top level blt health checks into blt folder
####################################################
if(TARGET check)
    blt_set_target_folder( TARGET check FOLDER blt)
endif()

if(TARGET style)
    blt_set_target_folder( TARGET style FOLDER blt)
endif()


