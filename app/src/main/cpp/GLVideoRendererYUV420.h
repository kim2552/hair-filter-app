#ifndef _GL_VIDEO_RENDERER_YUV_H_
#define _GL_VIDEO_RENDERER_YUV_H_

#include "VideoRenderer.h"
#include "GLUtils.h"
#include <opencv2/core.hpp>

#include "render/Camera.h"
#include "render/Mesh.h"
#include "face/FaceDetect.h"
#include "face/FaceMesh.h"
#include "render/Model.h"
#include "render/ModelObj.h"
#include "3rdparty/json/json.hpp"

using json = nlohmann::json;

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
	virtual void setInternalFilePaths(std::vector<std::string> file_paths) override;
    virtual uint32_t getParameters() override;
	virtual bool createTextures() override;
	virtual bool updateTextures() override;
	virtual void deleteTextures() override;
	virtual int createPrograms() override;

protected:
	virtual GLuint useProgram();

	GLuint m_program;
	GLuint m_vertexShader;
	GLuint m_pixelShader;

private:
	bool shaderProgramsCreated;

	std::unique_ptr<uint8_t[]> m_pDataY;
    uint8_t * m_pDataU;
    uint8_t * m_pDataV;
	std::unique_ptr<uint8_t[]> m_pDataA;	// Alpha channel

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

	Shader* shaderProgramImg;		// Shader program for image drawing
	Shader* shaderProgramMask;		// Shader program for image mask
	Shader* shaderProgramPoint;		// Shader program for misc point drawing
	Shader* shaderProgramModel;

	ModelObj* hairObj;

	Mesh* imgMesh;
	glm::mat4 imgModel;
    std::vector<Texture> yuvImgTextures;

	FaceDetect faceDetect;
	std::vector<FaceDetectObj> faceObjs;
	std::vector<Mesh> faceDetectMeshes;
	glm::mat4 faceDetectModel;

	std::vector<Texture> hairTextures;

	// Saved model parameters	TODO::INITIALIZE IN CONSTRUCTOR
	json configsJSON;
	std::string selected_hair_texture;
	std::string selected_hair_obj;
	float saved_ratio_width;
	float saved_ratio_height;
	float saved_scale_z;
	float saved_pitch;
	float saved_yaw;
	float saved_roll;
	float saved_topheadx;
	float saved_topheady;
	float saved_topheadz;
	int saved_front_vert_index;
};

#endif //_GL_VIDEO_RENDERER_YUV_H_
