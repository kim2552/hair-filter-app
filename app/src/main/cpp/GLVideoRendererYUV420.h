#ifndef _GL_VIDEO_RENDERER_YUV_H_
#define _GL_VIDEO_RENDERER_YUV_H_

#include "VideoRenderer.h"
#include "GLUtils.h"
#include <opencv2/core.hpp>

#include "render/Camera.h"
#include "render/Mesh.h"

class GLVideoRendererYUV420 : public VideoRenderer
{
public:
	GLVideoRendererYUV420();
	virtual ~GLVideoRendererYUV420();

    virtual void init(ANativeWindow* window, size_t width, size_t height) override;
	virtual void render() override;
	virtual void updateFrame(const video_frame& frame, int camera_facing) override;
	virtual void draw(uint8_t *buffer, size_t length, size_t width, size_t height, int rotation, int camera_facing) override;
	virtual void setAssetManager(AAssetManager* mgr) override;
	virtual AAssetManager* getAssetManager() override;
	virtual void setParameters(uint32_t params) override;
    virtual uint32_t getParameters() override;
	virtual bool createTextures() override;
	virtual bool updateTextures() override;
	virtual void deleteTextures() override;
	virtual int createProgram(const char *pVertexSource, const char *pFragmentSource) override;

protected:
	virtual GLuint useProgram();

	GLuint m_program;
	GLuint m_vertexShader;
	GLuint m_pixelShader;
private:
	std::unique_ptr<uint8_t[]> m_pDataY;

	std::unique_ptr<uint8_t[]> m_pRgbImage;

    uint8_t * m_pDataU;
    uint8_t * m_pDataV;

    size_t m_length;
	size_t m_sizeY;
    size_t m_sizeU;
    size_t m_sizeV;

	GLuint m_textureIdY;
	GLuint m_textureIdU;
	GLuint m_textureIdV;

	GLuint m_vertexPos;
    GLuint m_textureLoc;
	GLint m_textureYLoc;
	GLint m_textureULoc;
	GLint m_textureVLoc;
	GLint m_textureSize;
	GLint m_uniformProjection;
	GLint m_uniformRotation;
    GLint m_uniformScale;

    GLuint m_cameraFacing;

	Camera* camera;

	std::vector<Texture> yuvImgTextures;

	std::vector<Texture> pointTextures;
	Mesh* pointMesh;
	Shader* shaderProgramPoint;
	glm::mat4 pointModel;

	std::vector<Texture> imgTextures;
	Mesh* imgMesh;
	Shader* shaderProgramImg;
	glm::mat4 imgModel;
};

#endif //_GL_VIDEO_RENDERER_YUV_H_
