#include "HairObject.h"

void HairObject::loadImageAspectRatio(float imageAspectRatio)
{
	m_imageAspectRatio = imageAspectRatio;
}

void HairObject::loadTextures(std::vector<Texture>* textureHairObject)
{
	// Texture for face mask points
	m_hairRenderObj.textures = textureHairObject;
}

void HairObject::loadShaders(Shader* shaderHairObject)
{
	m_hairRenderObj.shader = shaderHairObject;
}

void HairObject::drawHairObject(Camera camera)
{
	m_hairModelObj->model->Draw(*m_hairRenderObj.shader, camera);
}

void HairObject::initHairObject(AppConfig* appConfig)
{
	m_hairModelObj = new ModelObj(appConfig->params.hair_obj.c_str(), *m_hairRenderObj.textures);

	// Activate shader for Object and configure the model matrix
	m_hairRenderObj.shader->Activate();
	glm::mat4 hairObjectModel = glm::mat4(1.0f);

	// Load configs for this object
	m_hairModelObj->model->front_head_vertex_index = appConfig->params.front_vert_index;
	m_hairModelObj->model->savedRatioWidth = appConfig->params.ratio_width;				// Width ratio value based on developer preference
	m_hairModelObj->model->savedRatioHeight = appConfig->params.ratio_height;			// Height ratio value based on developer preference
	m_hairModelObj->model->savedRatioLength = appConfig->params.ratio_length;			// Length ratio value based on developer preference
	m_hairModelObj->model->savedRatioWidth = appConfig->params.ratio_width;
	m_hairModelObj->model->savedRatioHeight = appConfig->params.ratio_height;
	m_hairModelObj->model->savedPitch = appConfig->params.pitch;
	m_hairModelObj->model->savedYaw = appConfig->params.yaw;
	m_hairModelObj->model->savedRoll = appConfig->params.roll;
	m_hairModelObj->model->savedTopHeadDist.x = appConfig->params.topheadx;		// Value obtained from fixedVertex distance from topHeadCoord
	m_hairModelObj->model->savedTopHeadDist.y = appConfig->params.topheady;		// Value obtained from fixedVertex distance from topHeadCoord
	m_hairModelObj->model->savedTopHeadDist.z = appConfig->params.topheadz;		// Value obtained from fixedVertex distance from topHeadCoord

	// Take care of all the light related things
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);

	glUniform4f(glGetUniformLocation(m_hairRenderObj.shader->ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(m_hairRenderObj.shader->ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(m_hairRenderObj.shader->ID, "filterColor"), 1, 1, 1);
}

void HairObject::updateHairObject(FaceMask* faceMask, bool modify_size)
{
	if (!modify_size) {
		// Transfer face mesh data to hair object
		m_hairModelObj->model->topHeadCoord = faceMask->m_faceDetectObj.topCoord;
        m_hairModelObj->model->faceWidth = faceMask->m_faceDetectObj.width / sin(glm::radians(90 - abs(faceMask->m_faceDetectObj.yaw)));
        m_hairModelObj->model->faceHeight = faceMask->m_faceDetectObj.height / sin(glm::radians(90 - abs(faceMask->m_faceDetectObj.pitch)));
        m_hairModelObj->model->faceLength = m_hairModelObj->model->faceHeight * 7.0f / 8.0f;
        m_hairModelObj->model->faceLength = m_hairModelObj->model->faceLength / sin(glm::radians(90 - abs(faceMask->m_faceDetectObj.roll)));

		// Rotate object to match face direction
		m_hairModelObj->model->facePitch = faceMask->m_faceDetectObj.pitch;
		m_hairModelObj->model->faceYaw = faceMask->m_faceDetectObj.yaw;
		m_hairModelObj->model->faceRoll = faceMask->m_faceDetectObj.roll;
	}

	glm::mat4 hairObjectModel = glm::mat4(1.0f);

	// Calculate the scale of the hair object
	m_hairModelObj->model->originalModelWidth = glm::length(m_hairModelObj->model->originalBb.max.x - m_hairModelObj->model->originalBb.min.x);
	m_hairModelObj->model->originalModelHeight = glm::length(m_hairModelObj->model->originalBb.max.y - m_hairModelObj->model->originalBb.min.y);
	m_hairModelObj->model->originalModelLength = glm::length(m_hairModelObj->model->originalBb.max.z - m_hairModelObj->model->originalBb.min.z);
	float scaleMultWidth = m_hairModelObj->model->savedRatioWidth * m_hairModelObj->model->faceWidth / m_hairModelObj->model->originalModelWidth;
	float scaleMultHeight = m_hairModelObj->model->savedRatioHeight * m_hairModelObj->model->faceHeight / m_hairModelObj->model->originalModelHeight;
	float scaleMultLength = m_hairModelObj->model->savedRatioLength * m_hairModelObj->model->faceLength / m_hairModelObj->model->originalModelLength;
	hairObjectModel = glm::scale(hairObjectModel, glm::vec3(scaleMultWidth, scaleMultHeight, scaleMultLength));

	if (!modify_size) {
		m_hairModelObj->model->scale.x = m_hairModelObj->model->savedRatioWidth * m_hairModelObj->model->faceWidth / m_hairModelObj->model->originalModelWidth;
		m_hairModelObj->model->scale.y = m_hairModelObj->model->savedRatioHeight * m_hairModelObj->model->faceHeight / m_hairModelObj->model->originalModelHeight;
		m_hairModelObj->model->scale.z = m_hairModelObj->model->savedRatioLength * m_hairModelObj->model->faceLength / m_hairModelObj->model->originalModelLength;
	}

	// Rotate object to match face direction
	hairObjectModel = glm::rotate(hairObjectModel, glm::radians(m_hairModelObj->model->facePitch + m_hairModelObj->model->savedPitch), glm::vec3(1.0f, 0.0f, 0.0f));
	hairObjectModel = glm::rotate(hairObjectModel, glm::radians(-m_hairModelObj->model->faceYaw + m_hairModelObj->model->savedYaw), glm::vec3(0.0f, 1.0f, 0.0f));
	hairObjectModel = glm::rotate(hairObjectModel, glm::radians(m_hairModelObj->model->faceRoll + m_hairModelObj->model->savedRoll), glm::vec3(0.0f, 0.0f, 1.0f));

	// Update the size and rotation of the model first
	m_hairModelObj->model->UpdateModel(hairObjectModel);

	float transX = (m_hairModelObj->model->savedTopHeadDist.x + m_hairModelObj->model->topHeadCoord.x) - m_hairModelObj->model->fixedVertex.x;
	float transY = (m_hairModelObj->model->savedTopHeadDist.y + m_hairModelObj->model->topHeadCoord.y) - m_hairModelObj->model->fixedVertex.y;
	float transZ = (m_hairModelObj->model->savedTopHeadDist.z + m_hairModelObj->model->topHeadCoord.z) - m_hairModelObj->model->fixedVertex.z;

	// Update the position of the model second
	glm::mat4 newtrans = glm::translate(glm::mat4(1.f), glm::vec3(transX, transY, transZ));
	hairObjectModel = newtrans * hairObjectModel;

	m_hairModelObj->model->UpdateModel(hairObjectModel);
}