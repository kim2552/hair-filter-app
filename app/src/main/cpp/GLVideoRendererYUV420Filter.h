#ifndef _GL_VIDEO_RENDERER_YUV_FILTER_H_
#define _GL_VIDEO_RENDERER_YUV_FILTER_H_

#include "GLVideoRendererYUV420.h"
#include <vector>

class GLVideoRendererYUV420Filter : public GLVideoRendererYUV420 {

    enum filters{

    };

public:
    GLVideoRendererYUV420Filter();
    virtual ~GLVideoRendererYUV420Filter();

    virtual void render() override ;
    virtual void setParameters(std::vector<std::string> params) override;
    virtual std::vector<std::string> getParameters() override;

private:
    std::vector<const char *> m_fragmentShader;
};

#endif //_GL_VIDEO_RENDERER_YUV_FILTER_H_
