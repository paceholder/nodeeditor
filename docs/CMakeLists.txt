find_package(Doxygen)

if(DOXYGEN_FOUND)

  # Find all the public headers
  get_target_property(QT_NODES_PUBLIC_HEADER_DIR QtNodes INTERFACE_INCLUDE_DIRECTORIES)

  file(GLOB_RECURSE QT_NODES_PUBLIC_HEADERS ${QT_NODES_PUBLIC_HEADER_DIR}/*.hpp)

  #This will be the main output of our command
  set(DOXYGEN_INDEX_FILE ${CMAKE_CURRENT_BINARY_DIR}/html/index.html)

  set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/src
                        ${PROJECT_SOURCE_DIR}/include
                        ${PROJECT_SOURCE_DIR}/examples/calculator)
  # Making string joined with " "
  list(JOIN DOXYGEN_INPUT_DIR " " DOXYGEN_INPUT_DIR_JOINED)

  set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)

  set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)

  set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
  set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)


  configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)


  file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}) #Doxygen won't create this for us

  add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                     DEPENDS ${QT_NODES_PUBLIC_HEADERS}
                     COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                     MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                     COMMENT "Generating docs"
                     VERBATIM)

  add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})
endif()


#####################################################################################


find_package(Sphinx)

if (Sphinx_FOUND)
  set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
  set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
  set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

  # Only regenerate Sphinx when:
  # - Doxygen has rerun
  # - Our doc files have been updated
  # - The Sphinx config has been updated


  add_custom_command(OUTPUT ${SPHINX_INDEX_FILE}
                     COMMAND ${SPHINX_EXECUTABLE} -b html
                     # Tell Breathe where to find the Doxygen output
                     -Dbreathe_projects.QtNodes=${DOXYGEN_OUTPUT_DIR}/xml
                     ${SPHINX_SOURCE} ${SPHINX_BUILD}
                     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                     # Other docs files you want to track should go here (or in some variable)
                     DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/index.rst
                             ${DOXYGEN_INDEX_FILE}
                     MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
                     COMMENT "Generating documentation with Sphinx")

  # Nice named target so we can run the job easily
  add_custom_target(Sphinx ALL DEPENDS ${SPHINX_INDEX_FILE})

  # Add an install target to install the docs
  include(GNUInstallDirs)
  install(DIRECTORY ${SPHINX_BUILD}
          DESTINATION ${CMAKE_INSTALL_DOCDIR})
endif()
