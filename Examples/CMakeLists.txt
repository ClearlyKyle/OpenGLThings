## CMAKE to get all source files from the Examples folder
## and to add the directories to the "include"

file (GLOB EXAMPLE_SOURCES 
        ${CMAKE_CURRENT_LIST_DIR}/**/*.c
        ${CMAKE_CURRENT_LIST_DIR}/**/*.h
    )

#foreach (exmaple_source ${EXAMPLE_SOURCES})
#    message(STATUS "EXAMPLE sources : ${exmaple_source}")
#endforeach()

foreach (_headerFile ${Foo_HEADERS})
get_filename_component(_dir ${_headerFile} PATH)
list (APPEND Foo_INCLUDE_DIRS ${_dir})
# message(STATUS "EXAMPLE INCLUDES : ${_dir}")
endforeach()

set (EXAMPLE_DIRECTORIES "")
file(GLOB example_subdirectories ${CMAKE_CURRENT_LIST_DIR}/*)
foreach(dir ${example_subdirectories})
    if(IS_DIRECTORY ${dir})
        #message(STATUS "EXAMPLE directories : ${dir}")
        list (APPEND EXAMPLE_DIRECTORIES ${dir})
    endif()
endforeach(dir)
list (REMOVE_DUPLICATES EXAMPLE_DIRECTORIES)

#add_executable(FooExe ${Foo_SOURCES})
#target_include_directories(FooExe PRIVATE ${Foo_INCLUDE_DIRS})