function(polly_log message)
    message(STATUS "[Polly] ${message}")
endfunction()

function(polly_warn message)
    message(WARNING "[Polly] ${message}")
endfunction()

function(polly_fatal_error message)
    message(FATAL_ERROR "[Polly] ${message}")
endfunction()

