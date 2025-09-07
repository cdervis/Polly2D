file(GLOB opengl_header_files CONFIGURE_DEPENDS "*.hpp")
file(GLOB opengl_source_files CONFIGURE_DEPENDS "*.cpp")

target_sources(Polly PRIVATE
    ${opengl_header_files}
    ${opengl_source_files}
    Resources/SpriteBatchOpenGL.vert
    Resources/PolyOpenGL.vert
    Resources/MeshOpenGL.vert
)

target_compile_definitions(Polly PRIVATE -Dpolly_have_gfx_opengl=1)

source_group("Private\\Graphics" FILES ${opengl_header_files} ${opengl_source_files})

