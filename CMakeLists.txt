# This file is part of the miniml project.
# Licensing information can be found in the LICENSE file.
# (C) Nandor Licker. All rights reserved.
CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

ADD_DEFINITIONS(-std=gnu++1y -g)

# miniml
ADD_LIBRARY(miniml
  miniml/BytecodeFile.cpp
  miniml/Context.cpp
  miniml/Heap.cpp
  miniml/Interpreter.cpp
  miniml/Stream.cpp
  miniml/Value.cpp
)

# mldump
ADD_EXECUTABLE(mldump
  dump.cpp
)
TARGET_LINK_LIBRARIES(mldump
  miniml
)

# interp
ADD_EXECUTABLE(mlinterp
  interp.cpp
  miniml/Runtime.cpp
)
TARGET_LINK_LIBRARIES(mlinterp
  miniml
)
