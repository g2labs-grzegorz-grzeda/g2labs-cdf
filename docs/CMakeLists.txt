# MIT License
#
# Copyright (c) 2023 Grzegorz Grzęda
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
cmake_minimum_required(VERSION 3.25)
enable_testing()

set(PROJECT_DOCUMENTATION_NAME ${CMAKE_PROJECT_NAME}-describe)

set(DOCUMENTATION_TEMPLATE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(DOCUMENTATION_OUTPUT_DIR ${CMAKE_BINARY_DIR}/documentation)

set(OUTPUT_DIRECTORY ${DOCUMENTATION_OUTPUT_DIR})

configure_file(${DOCUMENTATION_TEMPLATE_DIR}/Doxyfile.in ${DOCUMENTATION_OUTPUT_DIR}/Doxyfile @ONLY)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/src/style DESTINATION ${DOCUMENTATION_OUTPUT_DIR})

add_custom_target(${PROJECT_DOCUMENTATION_NAME}
    DEPENDS ${CMAKE_PROJECT_NAME} ${DOCUMENTATION_OUTPUT_DIR}/Doxyfile
    COMMAND doxygen ${DOCUMENTATION_OUTPUT_DIR}/Doxyfile
)

add_custom_command(
    TARGET ${PROJECT_DOCUMENTATION_NAME}
    PRE_BUILD
    COMMAND cp -R "${DOCUMENTATION_TEMPLATE_DIR}/style" "${DOCUMENTATION_OUTPUT_DIR}/style"
)