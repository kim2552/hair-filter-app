#pragma once
#ifndef HAIR_OBJ_H
#define HAIR_OBJ_H

#include <glm/glm.hpp>

#include "render/Model.h"
#include "render/ModelObj.h"
#include "objects/FaceMask.h"
#include "config/AppConfig.h"

class HairObject
{
public:
	void loadImageAspectRatio(float imageAspectRatio);
	void loadTextures(std::vector<Texture>* textureCameraImage);
	void loadShaders(Shader* shaderCameraImage);
	void initHairObject(AppConfig* appConfig, glm::vec3 hair_color);
	void changeHairColor(glm::vec3 color);
	void updateHairObject(FaceMask* faceMask, bool modify_size = false);
	void drawHairObject(Camera camera);

	RenderObject m_hairRenderObj;
	ModelObj* m_hairModelObj;
private:
	float m_imageAspectRatio;
};

#endif