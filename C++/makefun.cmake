macro(add_module mod)
    unset(modsource)
    unset(modhead)
    file(GLOB_RECURSE modsource ${mod}/src/*)
    file(GLOB_RECURSE modhead ${mod}/include/*)
    list(APPEND sourcelist ${modsource})
    list(APPEND headlist ${modhead})
    include_directories(${mod}/include)
endmacro(add_module)

macro(add_dir_source dir)
    unset(modsource)
    unset(modhead)
    file(GLOB_RESOURSE modsource ${dir}/*.cpp ${dir}/*.c ${dir}/*.cc)
    list(APPEND sourcelist ${modsource})
    file(GLOB_RESOURSE modhead ${dir}/*.h)
    list(APPEND headlist ${modhead})
    foreach(file ${headlist})
        get_filename_component(_path_ ${file} PATH)
        include_directories(${_path_})
    endforeach()
endmacro(add_dir_source)

macro(add_source_file file)
    list(APPEND sourcelist ${file})
endmacro(add_source_file)



# ??????????
macro(source_by_group)
    set(root_dir ${SourceDir})
    source_group(TREE ${root_dir} FILES ${headlist})
    source_group(TREE ${root_dir} FILES ${sourcelist})
endmacro(source_by_group)


macro(build_lib)
    source_by_group()
    add_library(${PROJECT_NAME} STATIC ${headlist} ${sourcelist})
endmacro(build_lib)

macro(build_exe)
    source_by_group()
    add_executable(${PROJECT_NAME} ${headlist} ${sourcelist})
    # ?????????
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endmacro(build_exe)