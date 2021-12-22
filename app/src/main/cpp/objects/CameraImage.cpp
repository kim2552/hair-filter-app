#include "CameraImage.h"

void CameraImage::loadImageAspectRatio(float imageAspectRatio)
{
	m_imageAspectRatio = imageAspectRatio;
}

void CameraImage::loadTextures(std::vector<Texture>* textureCameraImage)
{
	// Texture for face mask points
	m_cameraImage.textures = textureCameraImage;
}

void CameraImage::loadShaders(Shader* shaderCameraImage)
{
	m_cameraImage.shader = shaderCameraImage;
}

void CameraImage::drawCameraImage(Camera camera)
{
	m_cameraImage.mesh->Draw(*m_cameraImage.shader, camera, m_cameraImage.model);
}

void CameraImage::initCameraImage()
{
	std::vector<Vertex> cameraImageVertices{
		Vertex{glm::vec3(-1.0f * m_imageAspectRatio, 1.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(-1.0f * m_imageAspectRatio, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
		Vertex{glm::vec3(1.0f * m_imageAspectRatio, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
		Vertex{glm::vec3(1.0f * m_imageAspectRatio, 1.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
	};
	std::vector<GLuint> cameraImageIndices{
		0, 1, 2,
		0, 2, 3
	};

	// Create image mesh
	m_cameraImage.mesh = new Mesh(cameraImageVertices, cameraImageIndices, *m_cameraImage.textures);

	// Activate shader for Image and configure the model matrix
	m_cameraImage.shader->Activate();
	glm::mat4 cameraImageModel = glm::mat4(1.0f);
	if(m_cameraFacing == 0){    // front-facing
		cameraImageModel = glm::rotate(cameraImageModel, -1.57079633f, glm::vec3(0.0f, 0.0f, 1.0f));	// Flip the image 90
	}else{
		cameraImageModel = glm::rotate(cameraImageModel, 1.57079633f, glm::vec3(0.0f, 0.0f, 1.0f));	// Flip the image 90
	}
	m_cameraImage.model = cameraImageModel;
}

void CameraImage::setCameraFacing(int facing)
{
	m_cameraFacing = facing;
}