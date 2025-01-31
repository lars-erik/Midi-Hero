function(embed_csv OUTPUT_HEADER)
    set(FILES_AND_NAMES ${ARGN})

    set(HEADER_CONTENT "#pragma once\n\n")

    foreach(FILE_AND_NAME ${FILES_AND_NAMES})

        string(REPLACE "|" ";" FILE_AND_NAME "${FILE_AND_NAME}")

        list(GET FILE_AND_NAME 0 INPUT_FILE)
        list(GET FILE_AND_NAME 1 VAR_NAME)

        file(READ "${INPUT_FILE}" FILE_CONTENT)

        # Escape special characters for embedding
        string(REPLACE "\\" "\\\\" FILE_CONTENT "${FILE_CONTENT}")
        string(REPLACE "\"" "\\\"" FILE_CONTENT "${FILE_CONTENT}")
        string(REPLACE "\n" "\\n\"\n\"" FILE_CONTENT "${FILE_CONTENT}")

        # Append to header content
        set(HEADER_CONTENT "${HEADER_CONTENT}static const char ${VAR_NAME}[] =\n\"${FILE_CONTENT}\";\n\n")

    endforeach()

    # Write the final header file
    file(WRITE "${OUTPUT_HEADER}" "${HEADER_CONTENT}")

endfunction()
