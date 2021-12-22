#pragma once
#ifndef FACE_MASK_H
#define FACE_MASK_H

#include <glm/glm.hpp>

#include "render/Model.h"
#include "render/Texture.h"
#include "face/FaceDetect.h"

class FaceMask
{
public:
	void loadImageAspectRatio(float imageAspectRatio);
	void loadTextures(std::vector<Texture>* textureFaceMaskImage);
	void loadShaders(Shader* shaderFaceMaskImage);
	void loadFaceDetectObj(FaceDetectObj faceDetectObj, FaceDetect* faceDetect);
	void drawMaskImage(Camera camera);
	void setCameraFacing(int facing);

	RenderObject m_faceMaskImage;

	FaceDetectObj m_faceDetectObj;

private:
	float m_imageAspectRatio;
	int m_cameraFacing = 0;
};

#endif