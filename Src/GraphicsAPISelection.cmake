# Graphics API selection happens here.
# Currently, we're using the optimal and "most native" API on each platform, which is:
# - Metal on Apple platforms
# - Direct3D 11 on Windows
# - OpenGL >= 3 on everything else
#
# The Vulkan backend is currently experimental and incomplete. This is fine, since we don't need
# anything special that Vulkan offers that OpenGL doesn't have.
# Vulkan work will resume once all current backends are fully stable.
#
# In the future, Polly is going to support graphics API selection at runtime, meaning
# that games can be configured to use one or another, after they've been built.

if (APPLE)
    set(polly_have_gfx_metal TRUE)
elseif (WIN32)
    set(polly_have_gfx_d3d11 TRUE)
else()
    set(polly_have_gfx_opengl TRUE)
endif ()
