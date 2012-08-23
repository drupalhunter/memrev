# Copyright 2012 Philip Puryear
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required (VERSION 2.6)
project (libmemrev)

option (MEMREV_USE_VECTOR
        "Use SIMD acceleration if possible." OFF)

if (MEMREV_USE_VECTOR)
    add_definitions(-DUSE_VECTOR)
endif (MEMREV_USE_VECTOR)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_library (memrev memrev.cc)

install (TARGETS memrev DESTINATION lib)
install (FILES memrev.h DESTINATION include)
