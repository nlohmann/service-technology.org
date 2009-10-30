# generate parser with Bison

# search BISON
MACRO(FIND_BISON)
    IF(NOT BISON_EXECUTABLE)
        FIND_PROGRAM(BISON_EXECUTABLE bison)
        IF (NOT BISON_EXECUTABLE)
          MESSAGE(FATAL_ERROR "bison not found - aborting")
        ENDIF (NOT BISON_EXECUTABLE)
    ENDIF(NOT BISON_EXECUTABLE)
ENDMACRO(FIND_BISON)

MACRO(ADD_BISON_FILES _sources )
    FIND_BISON()

    FOREACH (_current_FILE ${ARGN})
      GET_FILENAME_COMPONENT(_in ${_current_FILE} ABSOLUTE)
      GET_FILENAME_COMPONENT(_basename ${_current_FILE} NAME_WE)

      SET(_out ${CMAKE_CURRENT_BINARY_DIR}/${_basename}.cc)
      SET(_header ${CMAKE_CURRENT_BINARY_DIR}/${_basename}.h)

      ADD_CUSTOM_COMMAND(
         OUTPUT ${_out} ${_header}
         COMMAND ${BISON_EXECUTABLE}
         ARGS
         -y -o ${_out} --defines=${_header}
         ${_in}
         DEPENDS ${_in}
      )

      SET(${_sources} ${${_sources}} ${_out})
      SET(${_sources} ${${_sources}} ${_header})
   ENDFOREACH (_current_FILE)
ENDMACRO(ADD_BISON_FILES)
