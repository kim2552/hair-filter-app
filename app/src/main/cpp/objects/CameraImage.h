#pragma once
#ifndef CAMERA_IMAGE_H
#define CAMERA_IMAGE_H

#include <glm/glm.hpp>

#include "render/Model.h"
#include "render/Texture.h"

class CameraImage
{
public:
	void loadImageAspectRatio(float imageAspectRatio);
	void loadTextures(std::vector<Texture>* textureCameraImage);
	void loadShaders(Shader* shaderCameraImage);
	void initCameraImage();
	void drawCameraImage(Camera camera);
	void setCameraFacing(int facing);

	RenderObject m_cameraImage;
private:
	float m_imageAspectRatio;
	int m_cameraFacing = 0;
};

#endif