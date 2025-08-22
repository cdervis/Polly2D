function(polly_download_dep_file name url expected_hash)
    set(filename ${polly_deps_headers_dir}/${name})
    set(checksum "")

    if (EXISTS ${filename})
        file(SHA256 ${filename} checksum)
    endif ()
    
    set(checksum "SHA256=${checksum}")

    if (NOT checksum STREQUAL ${expected_hash})
        polly_log("Fetching ${name}")
        file(DOWNLOAD ${url} ${polly_deps_headers_dir}/${name} EXPECTED_HASH ${expected_hash})
    endif ()
endfunction()
