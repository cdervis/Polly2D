file(GLOB opengl_header_files CONFIGURE_DEPENDS "*.hpp")
file(GLOB opengl_source_files CONFIGURE_DEPENDS "*.cpp")

list(APPEND opengl_header_files "glad.h")
list(APPEND opengl_source_files "glad.c")

target_sources(Polly PRIVATE
    FILE_SET HEADERS
    FILES ${opengl_header_files}
    PRIVATE ${opengl_source_files}
)

target_sources(Polly PRIVATE
    Resources/SpriteBatchOpenGL.vert
    Resources/PolyOpenGL.vert
    Resources/MeshOpenGL.vert
)

target_compile_definitions(Polly PRIVATE -Dpolly_have_gfx_opengl=1)

source_group("Private\\Graphics" FILES ${opengl_header_files} ${opengl_source_files})

