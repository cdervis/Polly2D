#include "Polly/Graphics/OpenGL/OpenGLPrerequisites.hpp"

#include "Polly/Algorithm.hpp"
#include "Polly/Error.hpp"
#include "Polly/Format.hpp"
#include "Polly/Image.hpp"
#include "Polly/List.hpp"

namespace Polly
{
void verifyOpenGLState()
{
    auto error = glGetError();

    if (error != GL_NO_ERROR)
    {
        auto errorList = List<GLenum, 4>();

        do
        {
            errorList.add(error);
            error = glGetError();
        }
        while (error != GL_NO_ERROR);

        if (errorList.size() == 1)
        {
            throw Error(formatString("An OpenGL error has occurred: {}", error));
        }
        else
        {
            throw Error(formatString("Multiple OpenGL errors have occurred: {}", errorList));
        }
    }
}

Maybe<OpenGLFormatTriplet> convertImageFormat(ImageFormat format)
{
    switch (format)
    {
        case ImageFormat::R8Unorm:
            return OpenGLFormatTriplet{
                .internalFormat = GL_R8,
                .baseFormat     = GL_RED,
                .type           = GL_UNSIGNED_BYTE,
            };
        case ImageFormat::R8G8B8A8UNorm:
            return OpenGLFormatTriplet{
                .internalFormat = GL_RGBA8,
                .baseFormat     = GL_RGBA,
                .type           = GL_UNSIGNED_BYTE,
            };
        case ImageFormat::R8G8B8A8Srgb:
            return OpenGLFormatTriplet{
                .internalFormat = GL_SRGB8,
                .baseFormat     = GL_RGBA,
                .type           = GL_UNSIGNED_BYTE,
            };
        case ImageFormat::R32G32B32A32Float:
            return OpenGLFormatTriplet{
                .internalFormat = GL_RGBA32F,
                .baseFormat     = GL_RGBA,
                .type           = GL_FLOAT,
            };
    }

    return none;
}

void setOpenGLObjectLabel([[maybe_unused]] GLuint handleGL, [[maybe_unused]] StringView name)
{
    // TODO
}
} // namespace Polly
