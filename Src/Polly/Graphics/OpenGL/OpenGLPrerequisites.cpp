#include "Polly/Graphics/OpenGL/OpenGLPrerequisites.hpp"

#include "Polly/Algorithm.hpp"
#include "Polly/Error.hpp"
#include "Polly/Format.hpp"
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
} // namespace Polly
