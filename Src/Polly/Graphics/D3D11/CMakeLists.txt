file(GLOB d3d11_header_files CONFIGURE_DEPENDS "*.hpp")
file(GLOB d3d11_source_files CONFIGURE_DEPENDS "*.cpp")

target_sources(Polly PRIVATE
	FILE_SET HEADERS
	FILES ${d3d11_header_files}
	PRIVATE ${d3d11_source_files}
)

target_link_libraries(Polly PRIVATE d3d11 dxgi dxguid)

target_compile_definitions(Polly PRIVATE -Dpolly_have_gfx_d3d11)

target_sources(Polly PRIVATE
	Resources/AllShaders.hlsl
)
