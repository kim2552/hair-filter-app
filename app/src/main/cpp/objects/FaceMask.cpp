#include "FaceMask.h"

void FaceMask::loadImageAspectRatio(float imageAspectRatio)
{
	m_imageAspectRatio = imageAspectRatio;
}

void FaceMask::loadTextures(std::vector<Texture>* textureFaceMaskImage)
{
	// Texture for face mask image
	m_faceMaskImage.textures = textureFaceMaskImage;
}

void FaceMask::loadShaders(Shader* shaderFaceMaskImage)
{
	m_faceMaskImage.shader = shaderFaceMaskImage;
}

void FaceMask::drawMaskImage(Camera camera)
{
	m_faceMaskImage.mesh->Draw(*m_faceMaskImage.shader, camera, m_faceMaskImage.model);
}

void FaceMask::loadFaceDetectObj(FaceDetectObj faceDetectObj, FaceDetect* faceDetect)
{
	m_faceDetectObj = faceDetectObj;

	std::vector<Vertex> faceMaskImageVertices{
		Vertex{glm::vec3(-1.0f * m_imageAspectRatio, 1.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(-1.0f * m_imageAspectRatio, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
		Vertex{glm::vec3(1.0f * m_imageAspectRatio, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
		Vertex{glm::vec3(1.0f * m_imageAspectRatio, 1.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
	};
	std::vector<GLuint> faceMaskImageIndices{
		0, 1, 2,
		0, 2, 3
	};

	//------------------------------- Initialize the Face Mask Image Model ----------------------------------//

	// Create image mesh
	m_faceMaskImage.mesh = new Mesh(faceMaskImageVertices, faceMaskImageIndices, *m_faceMaskImage.textures);

	m_faceMaskImage.shader->Activate();
	glm::mat4 faceMaskImageModel = glm::mat4(1.0f);
	if(m_cameraFacing == 0){    // front-facing
		faceMaskImageModel = glm::rotate(faceMaskImageModel, -1.57079633f, glm::vec3(0.0f, 0.0f, 1.0f));	// Flip the image 90
	}else{
		faceMaskImageModel = glm::rotate(faceMaskImageModel, 1.57079633f, glm::vec3(0.0f, 0.0f, 1.0f));	// Flip the image 90
	}
	faceMaskImageModel = glm::scale(faceMaskImageModel, glm::vec3(0.6f, 0.6f, 1.0f));
	glm::mat4 translation_1 = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 0.0f, -1.0f));
	faceMaskImageModel = translation_1 * faceMaskImageModel;

	glm::mat4 testModel = glm::mat4(1.0f);
	testModel = glm::scale(testModel, glm::vec3(0.6f, 0.6f, 1.0f));
//	glm::mat4 trans = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 0.0f, -1.0f));
//	testModel = trans * testModel;

	cv::Point th = m_faceDetectObj.topPoint;
	cv::Point bh = m_faceDetectObj.botPoint;
	cv::Point rc = m_faceDetectObj.rightPoint;
	cv::Point lc = m_faceDetectObj.leftPoint;
	glm::vec4 vth(glm::vec3(0.75 - (th.x * 0.75 / (float)RESIZED_IMAGE_WIDTH * 2), 1 - (th.y / (float)RESIZED_IMAGE_HEIGHT * 2), -1.0f), 1.0f);
	glm::vec4 vbh(glm::vec3(0.75 - (bh.x * 0.75 / (float)RESIZED_IMAGE_WIDTH * 2), 1 - (bh.y / (float)RESIZED_IMAGE_HEIGHT * 2), -1.0f), 1.0f);
	glm::vec4 vrc(glm::vec3(0.75 - (rc.x * 0.75 / (float)RESIZED_IMAGE_WIDTH * 2), 1 - (rc.y / (float)RESIZED_IMAGE_HEIGHT * 2), -1.0f), 1.0f);
	glm::vec4 vlc(glm::vec3(0.75 - (lc.x * 0.75 / (float)RESIZED_IMAGE_WIDTH * 2), 1 - (lc.y / (float)RESIZED_IMAGE_HEIGHT * 2), -1.0f), 1.0f);
	vth = testModel * vth;
	vbh = testModel * vbh;
	vrc = testModel * vrc;
	vlc = testModel * vlc;
	m_faceDetectObj.topCoord = vth;
	m_faceDetectObj.width = glm::length(vrc - vlc);
	m_faceDetectObj.height = glm::length(vth - vbh);
	m_faceMaskImage.model = faceMaskImageModel;
}

void FaceMask::setCameraFacing(int facing)
{
	m_cameraFacing = facing;
}