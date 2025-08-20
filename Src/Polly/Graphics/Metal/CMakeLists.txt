set(metal_files
    MetalCBufferAllocator.cpp
    MetalCBufferAllocator.hpp
    MetalConversion.hpp
    MetalConversion.cpp
    MetalGraphicsDevice.cpp
    MetalGraphicsDevice.hpp
    MetalImage.cpp
    MetalImage.hpp
    MetalPsoCache.cpp
    MetalPsoCache.hpp
    MetalSamplerStateCache.cpp
    MetalSamplerStateCache.hpp
    MetalUserShader.cpp
    MetalUserShader.hpp
    MetalWindow.cpp
    MetalWindow.hpp
    MetalHelper.hpp
    MetalHelper.mm
)

target_sources(Polly PRIVATE ${metal_files})

target_compile_definitions(Polly PRIVATE
    -Dpolly_have_gfx_metal=1
    -DMETALCPP_SYMBOL_VISIBILITY_HIDDEN
)

target_include_directories(Polly PRIVATE Bindings)

target_link_libraries(Polly PRIVATE "-framework Metal")

target_sources(Polly PRIVATE Resources/AllShaders.metal)

