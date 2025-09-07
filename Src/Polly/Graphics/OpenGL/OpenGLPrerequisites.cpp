#include "Polly/Graphics/OpenGL/OpenGLPrerequisites.hpp"

#include "Polly/Algorithm.hpp"
#include "Polly/BlendState.hpp"
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

Maybe<GLenum> convertBlend(Blend blend)
{
    switch (blend)
    {
        case Blend::One: return GL_ONE;
        case Blend::Zero: return GL_ZERO;
        case Blend::SrcColor: return GL_SRC_COLOR;
        case Blend::InvSrcColor: return GL_ONE_MINUS_SRC_COLOR;
        case Blend::SrcAlpha: return GL_SRC_ALPHA;
        case Blend::InvSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
        case Blend::DstColor: return GL_DST_COLOR;
        case Blend::InvDstColor: return GL_ONE_MINUS_DST_COLOR;
        case Blend::DstAlpha: return GL_DST_ALPHA;
        case Blend::InvDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
        case Blend::BlendFactor: return GL_CONSTANT_COLOR;
        case Blend::InvBlendFactor: return GL_ONE_MINUS_CONSTANT_COLOR;
        case Blend::SrcAlphaSaturation: return GL_SRC_ALPHA_SATURATE;
    }

    return none;
}

Maybe<GLenum> convertBlendFunction(BlendFunction blendFunction)
{
    switch (blendFunction)
    {
        case BlendFunction::Add: return GL_FUNC_ADD;
        case BlendFunction::Subtract: return GL_FUNC_SUBTRACT;
        case BlendFunction::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
        case BlendFunction::Min: return GL_MIN;
        case BlendFunction::Max: return GL_MAX;
    }

    return none;
}
} // namespace Polly
