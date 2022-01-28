#include "GLVideoRendererYUV420Filter.h"
#include "GLShaders.h"

GLVideoRendererYUV420Filter::GLVideoRendererYUV420Filter()
{
    m_fragmentShader.push_back(kFragmentShader);
    m_fragmentShader.push_back(kFragmentShader1);
    m_fragmentShader.push_back(kFragmentShader2);
    m_fragmentShader.push_back(kFragmentShader3);
    m_fragmentShader.push_back(kFragmentShader4);
    m_fragmentShader.push_back(kFragmentShader5);
    m_fragmentShader.push_back(kFragmentShader6);
    m_fragmentShader.push_back(kFragmentShader7);
    m_fragmentShader.push_back(kFragmentShader8);
    m_fragmentShader.push_back(kFragmentShader9);
    m_fragmentShader.push_back(kFragmentShader10);
    m_fragmentShader.push_back(kFragmentShader11);
    m_fragmentShader.push_back(kFragmentShader12);
}

GLVideoRendererYUV420Filter::~GLVideoRendererYUV420Filter()
{

}

void GLVideoRendererYUV420Filter::setParameters(std::vector<std::string> params)
{
    GLVideoRendererYUV420::setParameters(params);
    isProgramChanged = true;
}

std::vector<std::string> GLVideoRendererYUV420Filter::getParameters()
{
    return m_params;
}

void GLVideoRendererYUV420Filter::render()
{
    if (isProgramChanged)
    {
        delete_program(m_program);
        createPrograms();
    }

    GLVideoRendererYUV420::render();
}
