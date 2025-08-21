file(GLOB metal_header_files CONFIGURE_DEPENDS "*.hpp")
file(GLOB metal_source_files CONFIGURE_DEPENDS "*.cpp")
file(GLOB metal_objc_files CONFIGURE_DEPENDS "*.mm")

target_sources(Polly PRIVATE
        FILE_SET HEADERS
        FILES ${metal_header_files}
        PRIVATE ${metal_source_files} ${metal_objc_files}
)

target_compile_definitions(Polly PRIVATE
    -Dpolly_have_gfx_metal=1
    -DMETALCPP_SYMBOL_VISIBILITY_HIDDEN
)

target_include_directories(Polly PRIVATE Bindings)

target_link_libraries(Polly PRIVATE "-framework Metal")

target_sources(Polly PRIVATE Resources/AllShaders.metal)

