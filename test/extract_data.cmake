# cmake -DDATA=<data_reader/data> -DOUT=<dir> -P extract_data.cmake
# Unpacks the stage 0 sources into <dir> with the layout data_reader/stage_0 installs, skipping an archive
# already unpacked and not changed since.

set(archives
    3201/3201.zip 3201/files.zip
    3202/3202.zip 3202/files.zip
    3203/3203.zip 3203/files.zip
    Crumbs/Crumbs.tar.gz
)
foreach(archive ${archives})
    get_filename_component(source ${archive} DIRECTORY)
    get_filename_component(name ${archive} NAME)
    set(stamp ${OUT}/${source}/.${name}.unpacked)
    if (NOT EXISTS ${stamp} OR ${DATA}/${archive} IS_NEWER_THAN ${stamp})
        file(MAKE_DIRECTORY ${OUT}/${source})
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xf ${DATA}/${archive}
            WORKING_DIRECTORY ${OUT}/${source}
            RESULT_VARIABLE result
        )
        if (NOT result EQUAL 0)
            message(FATAL_ERROR "failed to unpack ${DATA}/${archive}")
        endif()
        file(TOUCH ${stamp})
    endif()
endforeach()
