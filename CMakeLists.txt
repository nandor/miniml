# This file is part of the miniml project.
# Licensing information can be found in the LICENSE file.
# (C) Nandor Licker. All rights reserved.

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

ADD_DEFINITIONS(-std=gnu++1y -g)

ADD_EXECUTABLE(mldump
  dump.cpp
  miniml/BytecodeFile.cpp
  miniml/Context.cpp
  miniml/Heap.cpp
  miniml/Stream.cpp
  miniml/Value.cpp
)
