# Copyright (c) Lawrence Livermore National Security, LLC and other Conduit
# Project developers. See top-level LICENSE AND COPYRIGHT files for dates and
# other details. No copyright assignment is required to contribute to Conduit.

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/conduit_blueprint_python_exports.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/conduit_blueprint_python_exports.h")

# add blueprint submodule
PYTHON_ADD_COMPILED_MODULE(NAME          conduit_blueprint_python
                           DEST_DIR      python-modules
                           PY_MODULE_DIR conduit/blueprint
                           SOURCES       conduit_blueprint_python.cpp
                                         ${CMAKE_CURRENT_BINARY_DIR}/conduit_blueprint_python_exports.h
                           FOLDER        libs/python)

# compiled modules depend on output dir structure created by main module setup
add_dependencies( conduit_blueprint_python conduit_python_py_setup)

# link with the proper libs (beyond python)
target_link_libraries(conduit_blueprint_python conduit conduit_blueprint conduit_python_build)

#############################################################
# blueprint.mcarray
#############################################################

# add mcarray submodule
PYTHON_ADD_COMPILED_MODULE(NAME          conduit_blueprint_mcarray_python
                           DEST_DIR      python-modules
                           PY_MODULE_DIR conduit/blueprint/mcarray
                           SOURCES       conduit_blueprint_mcarray_python.cpp
                                         ${CMAKE_CURRENT_BINARY_DIR}/conduit_blueprint_python_exports.h
                           FOLDER        libs/python)

# compiled modules depend on output dir structure created by main module setup
add_dependencies( conduit_blueprint_mcarray_python conduit_python_py_setup)

# link with the proper libs (beyond python)
target_link_libraries(conduit_blueprint_mcarray_python conduit conduit_blueprint conduit_python_build)

# add mcarray examples submodule
PYTHON_ADD_COMPILED_MODULE(NAME          conduit_blueprint_mcarray_examples_python
                           DEST_DIR      python-modules
                           PY_MODULE_DIR conduit/blueprint/mcarray/examples
                           SOURCES       conduit_blueprint_mcarray_examples_python.cpp
                                         ${CMAKE_CURRENT_BINARY_DIR}/conduit_blueprint_python_exports.h
                           FOLDER        libs/python)

# compiled modules depend on output dir structure created by main module setup
add_dependencies( conduit_blueprint_mcarray_examples_python conduit_python_py_setup)

# link with the proper libs (beyond python)
target_link_libraries(conduit_blueprint_mcarray_examples_python conduit conduit_blueprint conduit_python_build)

#############################################################
# blueprint.mesh
#############################################################

# add mesh submodule
PYTHON_ADD_COMPILED_MODULE(NAME          conduit_blueprint_mesh_python
                           DEST_DIR      python-modules
                           PY_MODULE_DIR conduit/blueprint/mesh
                           SOURCES       conduit_blueprint_mesh_python.cpp
                                         ${CMAKE_CURRENT_BINARY_DIR}/conduit_blueprint_python_exports.h
                           FOLDER        libs/python)

# compiled modules depend on output dir structure created by main module setup
add_dependencies( conduit_blueprint_mesh_python conduit_python_py_setup)

# link with the proper libs (beyond python)
target_link_libraries(conduit_blueprint_mesh_python conduit conduit_blueprint conduit_python_build)


# add mesh examples submodule
PYTHON_ADD_COMPILED_MODULE(NAME          conduit_blueprint_mesh_examples_python
                           DEST_DIR      python-modules
                           PY_MODULE_DIR conduit/blueprint/mesh/examples
                           SOURCES       conduit_blueprint_mesh_examples_python.cpp
                                         ${CMAKE_CURRENT_BINARY_DIR}/conduit_blueprint_python_exports.h
                           FOLDER        libs/python)

# compiled modules depend on output dir structure created by main module setup
add_dependencies( conduit_blueprint_mesh_examples_python conduit_python_py_setup)

# link with the proper libs (beyond python)
target_link_libraries(conduit_blueprint_mesh_examples_python conduit conduit_blueprint conduit_python_build)

#############################################################
# blueprint.table
#############################################################

# add table submodule
PYTHON_ADD_COMPILED_MODULE(NAME          conduit_blueprint_table_python
                           DEST_DIR      python-modules
                           PY_MODULE_DIR conduit/blueprint/table
                           SOURCES       conduit_blueprint_table_python.cpp
                                         ${CMAKE_CURRENT_BINARY_DIR}/conduit_blueprint_python_exports.h
                           FOLDER        libs/python)

# compiled modules depend on output dir structure created by main module setup
add_dependencies( conduit_blueprint_table_python conduit_python_py_setup)

# link with the proper libs (beyond python)
target_link_libraries(conduit_blueprint_table_python conduit conduit_blueprint conduit_python_build)


# add table examples submodule
PYTHON_ADD_COMPILED_MODULE(NAME          conduit_blueprint_table_examples_python
                           DEST_DIR      python-modules
                           PY_MODULE_DIR conduit/blueprint/table/examples
                           SOURCES       conduit_blueprint_table_examples_python.cpp
                                         ${CMAKE_CURRENT_BINARY_DIR}/conduit_blueprint_python_exports.h
                           FOLDER        libs/python)

# compiled modules depend on output dir structure created by main module setup
add_dependencies( conduit_blueprint_table_examples_python conduit_python_py_setup)

# link with the proper libs (beyond python)
target_link_libraries(conduit_blueprint_table_examples_python conduit conduit_blueprint conduit_python_build)
