# cmake -DDATA=<fixture data> -DOUT=<dir> -DWORD=<word> -P added_word_data.cmake
# Copies the fixture data into <dir> with one more dictionary word.

file(REMOVE_RECURSE ${OUT})
file(COPY ${DATA}/ DESTINATION ${OUT})

set(words ${OUT}/3201/files/SINGLE.TXT)
file(READ ${words} text)
if (text MATCHES "(^|\n)${WORD}\r?\n")
    message(FATAL_ERROR "${words} already has ${WORD}")
endif()
file(APPEND ${words} "${WORD}\n")
