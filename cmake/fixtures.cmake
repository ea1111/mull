if (APPLE AND CMAKE_OSX_SYSROOT)
  set (SYSROOT -isysroot ${CMAKE_OSX_SYSROOT})
else()
  set (SYSROOT)
endif()

# returns 'path' - ${CMAKE_SOURCE_DIR}
function(_fixture_prefix prefix path out)
  string(REPLACE "${prefix}/lab/" "" replaced "${path}")
  file(TO_CMAKE_PATH "${replaced}" replaced)
  string(REPLACE "/" "_" replaced ${replaced})
  set(${out} ${replaced} PARENT_SCOPE)
endfunction()

function(compile_fixture)
  set (prefix local)
  set (optionArguments )
  set (singleValueArguments INPUT OUTPUT_EXTENSION)
  set (multipleValueArguments FLAGS)

  cmake_parse_arguments(${prefix}
    "${optionArguments}"
    "${singleValueArguments}"
    "${multipleValueArguments}"
    ${ARGN}
  )

  get_filename_component(filename ${local_INPUT} NAME_WE)
  get_filename_component(full_path ${local_INPUT} DIRECTORY)
  _fixture_prefix(${CMAKE_SOURCE_DIR} ${full_path} path_prefix)

  set(fixture ${CMAKE_CURRENT_BINARY_DIR}/${filename}.${local_OUTPUT_EXTENSION})

  add_custom_command(OUTPUT ${fixture}
    COMMAND ${MULL_CC} ${SYSROOT} ${local_FLAGS} ${local_INPUT} -o ${fixture}
    DEPENDS ${local_INPUT}
  )
  set (dependency build-${path_prefix}-${filename}-${local_OUTPUT_EXTENSION}-fixture)

if (TARGET clang)
  set (clang_dependencies clang)
endif()
if (TARGET cxx)
  set (clang_dependencies ${clang_dependencies} cxx)
endif()
if (TARGET cxxabi)
  set (clang_dependencies ${clang_dependencies} cxxabi)
endif()

  add_custom_target(${dependency} DEPENDS ${fixture} ${clang_dependencies} ${local_INPUT})
  add_fixture(${fixture})
  add_fixture_dependency(${dependency})
endfunction()

function(generate_fixture_factory fixture_factory)
  get_property(fixtures GLOBAL PROPERTY TEST_FIXTURES)

  set (HEADER
"// !!! Autogenerated file !!!

namespace mull {
namespace fixtures {
")

  set (FIXTURE_FACTORY ${CMAKE_CURRENT_BINARY_DIR}/FixturePaths.h)
  file(WRITE ${FIXTURE_FACTORY} ${HEADER})

  foreach(fixture ${fixtures})
    get_filename_component(filename ${fixture} NAME_WE)
    get_filename_component(extension ${fixture} EXT)
    string(REPLACE "." "" extension "${extension}")
    get_filename_component(full_path ${fixture} DIRECTORY)

    string(FIND ${full_path} ${CMAKE_BINARY_DIR} binary_dir_index)
    string(FIND ${full_path} ${CMAKE_SOURCE_DIR} source_dir_index)

    if (NOT binary_dir_index EQUAL -1)
      _fixture_prefix(${CMAKE_BINARY_DIR} ${full_path} path_prefix)
    elseif(NOT source_dir_index EQUAL -1)
      _fixture_prefix(${CMAKE_SOURCE_DIR} ${full_path} path_prefix)
    else()
      message(FATAL_ERROR "Unexpected fixture path: ${full_path}")
    endif()

    file(APPEND ${FIXTURE_FACTORY}
"
static const char *${path_prefix}_${filename}_${extension}_path() {
  return \"${fixture}\";
}
")

  endforeach()

  file(APPEND ${FIXTURE_FACTORY} "
} // namespace fixtures
} // namespace mull
")

  set (${fixture_factory} ${FIXTURE_FACTORY} PARENT_SCOPE)
endfunction()
