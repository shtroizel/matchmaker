# cmake -DDATA=<fixture data> -DOUT=<dir> -P moved_text_data.cmake
# Copies the fixture data into <dir> with the text of two Crumbs posts trading places: the same words, at
# other locations.

file(REMOVE_RECURSE ${OUT})
file(COPY ${DATA}/ DESTINATION ${OUT})

set(posts ${OUT}/Crumbs/text/0001-0002)
file(READ ${posts} text)
string(REPLACE "\nNight.\n" "\n<>\n" moved "${text}")
string(REPLACE "\nNoon.\n" "\nNight.\n" moved "${moved}")
string(REPLACE "\n<>\n" "\nNoon.\n" moved "${moved}")
if (moved STREQUAL text)
    message(FATAL_ERROR "no Night. and Noon. posts to trade in ${posts}")
endif()
file(WRITE ${posts} "${moved}")
