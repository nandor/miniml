# This file is part of the miniml project.
# Licensing information can be found in the LICENSE file.
# (C) Nandor Licker. All rights reserved.
CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

ADD_DEFINITIONS(-Wall -Werror -Wextra)
ADD_DEFINITIONS(-std=gnu++1y -g)

# Compile the interpreter
SET(INTERP ${CMAKE_CURRENT_BINARY_DIR}/Interpreter.s)
ADD_CUSTOM_COMMAND(
    OUTPUT
      ${INTERP}
    COMMAND
      runhaskell Interpreter.hs x86 ${INTERP}
    WORKING_DIRECTORY
      ${CMAKE_CURRENT_SOURCE_DIR}/miniml/Interp
    DEPENDS
      miniml/Interp/Codegen.hs
      miniml/Interp/Interpreter.hs
)

# miniml
ADD_LIBRARY(miniml STATIC
  ${INTERP}
  miniml/BytecodeFile.cpp
  miniml/Context.cpp
  miniml/Heap.cpp
  miniml/Interpreter.cpp
  miniml/Stream.cpp
  miniml/Value.cpp
)

# runtime
ADD_LIBRARY(minirt SHARED
  minirt/Array.cpp
  minirt/Double.cpp
  minirt/Compare.cpp
  minirt/GC.cpp
  minirt/Ints.cpp
  minirt/IO.cpp
  minirt/Object.cpp
  minirt/String.cpp
  minirt/System.cpp
)
TARGET_LINK_LIBRARIES(minirt
  miniml
)

# mldump
ADD_EXECUTABLE(mldump
  dump.cpp
)
TARGET_LINK_LIBRARIES(mldump
  miniml
  minirt
)

# interp
ADD_EXECUTABLE(mlinterp
  interp.cpp
)
TARGET_LINK_LIBRARIES(mlinterp
  miniml
  minirt
)
