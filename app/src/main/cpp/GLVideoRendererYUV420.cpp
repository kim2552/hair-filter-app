#include "GLVideoRendererYUV420.h"
#include "GLShaders.h"
#include "CommonUtils.h"
#include "Log.h"

// Preview Image Vertices coordinates
Vertex imgVerts[] =
		{ //               COORDINATES           /            NORMALS          /           COLORS         /       TEXCOORDS         //
				Vertex{glm::vec3(-1.0f, 0.75f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
				Vertex{glm::vec3(-1.0f, -0.75f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
				Vertex{glm::vec3(1.0f, -0.75f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
				Vertex{glm::vec3(1.0f, 0.75f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
		};

// Preview Image Indices for vertices order
static const GLuint imgInds[] =
		{
				0, 1, 2,
				0, 2, 3
		};

GLVideoRendererYUV420::GLVideoRendererYUV420()
    : m_program(0)
	, m_vertexShader(0)
	, m_pixelShader(0)
	, m_pDataY(nullptr)
	, m_pDataU(nullptr)
	, m_pDataV(nullptr)
    , m_length(0)
	, m_sizeY(0)
	, m_sizeU(0)
	, m_sizeV(0)
	, m_textureIdY(0)
	, m_textureIdU(0)
	, m_textureIdV(0)
	, m_vertexPos(0)
    , m_textureLoc(0)
	, m_textureYLoc(0)
	, m_textureULoc(0)
	, m_textureVLoc(0)
	, m_textureSize(0)
	, m_uniformProjection(0)
    , m_uniformRotation(0)
    , m_uniformScale(0)
    , m_cameraFacing(0)
{
	isProgramChanged = true;
	shaderProgramsCreated = false;
}

GLVideoRendererYUV420::~GLVideoRendererYUV420()
{
	deleteTextures();
	delete_program(m_program);
}

// GLVideoRenderer Initialization - occurs on application surface changed!
void GLVideoRendererYUV420::init(ANativeWindow* window, size_t width, size_t height)
{
    m_backingWidth = width;
    m_backingHeight = height;

    const char* filename = (const char*)NULL;
    unsigned char* hairimage;
    int file_size;
    AAssetDir* assetDirImg = AAssetManager_openDir(assetManager, "hair");
    while ((filename = AAssetDir_getNextFileName(assetDirImg)) != NULL) {
        if(strcmp(filename, "dark_blonde_2.jpg") == 0){
            AAsset* asset = AAssetManager_open(assetManager, "hair/dark_blonde_2.jpg", AASSET_MODE_UNKNOWN);
            file_size = AAsset_getLength(asset);
            hairimage = (unsigned char*) malloc (sizeof(unsigned char)*file_size);
            AAsset_read(asset,hairimage,file_size);
            AAsset_close(asset);
        }
    }
    AAssetDir_close(assetDirImg);

    // Initialize texture
    hairTextures.push_back(Texture(hairimage, "diffuse", 0, 894, 894, 3));
	glEnable(GL_DEPTH_TEST);
}

// GLVideoRenderer render - occurs every new camera image
void GLVideoRendererYUV420::render()
{
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Update textures and create shader programs
	if (!updateTextures() || !useProgram()) return;

	// Detect face(s) and get landmark points
    std::vector<std::vector<cv::Point2f>> faces;
    faces = faceDetect.getFaceLandmarks(m_pDataY.get(), m_width, m_height);

    // Create a mesh object from the face detect
    for(auto fdMesh : faceDetectMeshes) fdMesh.Delete();
    faceDetectMeshes.clear();
    for (size_t i = 0; i < faces.size(); i++)							// Loop through all the faces
    {
        faceDetectMeshes.push_back(faceDetect.genFaceMesh(faces[i]));
    }

    // Create FaceMesh models from all faces detected
	for(auto fmMesh : faceMeshMeshes) fmMesh.Delete();
    faceMeshMeshes.clear();
	faceMeshModel.clear();
	faceModels.clear();
	for (size_t i = 0; i < faces.size(); i++)						// Loop through all the faces
	{
		eos::core::LandmarkCollection<Eigen::Vector2f> landmarkCollection = faceMesh.processLandmarks(
				faces[i]);
		FaceMeshObj faceMeshObj = faceMesh.getFaceMeshObj(landmarkCollection, RESIZED_IMAGE_WIDTH,
														  RESIZED_IMAGE_HEIGHT);
		faceMeshMeshes.push_back(faceMesh.genFaceMesh(faceMeshObj));
		faceMeshModel.push_back(faceMesh.genFaceModel(faceMeshObj, m_cameraFacing));

		FaceProperties props = faceMesh.genProperties(faceMeshObj, faceMeshModel[i]);

		Model faceModel(faceMeshMeshes);
		faceModel.topHeadCoord = props.topHeadCoord;
		faceModel.faceWidth = props.faceWidth;
		faceModel.faceHeight = props.faceHeight;

		faceModel.UpdateModel(faceMeshModel[i]);
		faceModels.push_back(faceModel);
	}

    std::vector<ModelObj> hairObjs;

    for (auto fom : faceModels)
    {
        // Initialize model object
        std::string filename = internalFilePaths[7];
        ModelObj hairBob(filename, hairTextures);

        // Transfer face mesh data to hair object
        hairBob.model->topHeadCoord = fom.topHeadCoord;
        hairBob.model->faceWidth = fom.faceWidth;
        hairBob.model->faceHeight = fom.faceHeight;

        // Activate shader for Object and configure the model matrix
        glm::mat4 hairObjectModel = glm::mat4(1.0f);

        // Calculate the scale of the hair object
        float goldenRatioWidth = 1.7723179;				// Width ratio value based on developer preference			// TODO::Store values in json file
        float goldenRatioHeight = 1.8758706;			// Height ratio value based on developer preference
        float goldenZScale = 1.0f / 25.0f;				// Constant Z scale value based on developer preference
        float objectWidth = glm::length(hairBob.model->originalBb.max.x - hairBob.model->originalBb.min.x);
        float objectHeight = glm::length(hairBob.model->originalBb.max.y - hairBob.model->originalBb.min.y);
        float scaleMultWidth = goldenRatioWidth * hairBob.model->faceWidth / objectWidth;
        float scaleMultHeight = goldenRatioHeight * hairBob.model->faceHeight / objectHeight;
        hairObjectModel = glm::scale(hairObjectModel, glm::vec3(scaleMultWidth, scaleMultHeight, goldenZScale));	// TODO::Find calculation for Z component

        // Rotate object to match face direction
        hairObjectModel = glm::rotate(hairObjectModel, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        hairObjectModel = glm::rotate(hairObjectModel, glm::radians(fom.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        hairObjectModel = glm::rotate(hairObjectModel, glm::radians(fom.yaw), glm::vec3(0.0f, -1.0f, 0.0f));
        hairObjectModel = glm::rotate(hairObjectModel, glm::radians(fom.roll), glm::vec3(0.0f, 0.0f, -1.0f));

        hairBob.model->UpdateModel(hairObjectModel);			// Updates the position and bounding box of the scaled, rotated object

        float goldenDiffX = -0.063447;		// Value obtained from fixedVertex distance from topHeadCoord	#TODO::Save values in a json file and retrieve it
        float goldenDiffY = 0.021071;		// Value obtained from fixedVertex distance from topHeadCoord
        float goldenDiffZ = 0.007910;		// Value obtained from fixedVertex distance from topHeadCoord

        float transX = (goldenDiffX + hairBob.model->topHeadCoord.x) - hairBob.model->fixedVertex.x;
        float transY = (goldenDiffY + hairBob.model->topHeadCoord.y) - hairBob.model->fixedVertex.y;
        float transZ = (goldenDiffZ + hairBob.model->topHeadCoord.z) - hairBob.model->fixedVertex.z;

        glm::mat4 translation = glm::translate(glm::mat4(1.f), glm::vec3(transX, transY, transZ));
        hairObjectModel = translation * hairObjectModel;

//        shaderProgramObj.Activate();
//        glUniform4f(glGetUniformLocation(shaderProgramObj.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
//        glUniform3f(glGetUniformLocation(shaderProgramObj.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        hairBob.model->UpdateModel(hairObjectModel);																// Update object model
        hairObjs.push_back(hairBob);
    }

	// Create image mesh for camera preview
	std::vector <Vertex> yuvImgV(imgVerts, imgVerts + sizeof(imgVerts) / sizeof(Vertex));
	std::vector <GLuint> yuvImgI(imgInds, imgInds + sizeof(imgInds) / sizeof(GLuint));
	imgMesh = new Mesh (yuvImgV, yuvImgI, yuvImgTextures);

    // Draw all meshes
	imgMesh->Draw(*shaderProgramImg, *camera, imgModel);		// Draw the image
//	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	for (size_t i = 0; i < faceDetectMeshes.size(); i++)
	{
		faceDetectMeshes[i].Draw(*shaderProgramPoint, *camera, faceDetectModel, GL_LINE_STRIP);
	}
	for(size_t i=0;i<faceModels.size();i++)
	{
		faceModels[i].Draw(*shaderProgramPoint,*camera);
	}
//	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    for (size_t i = 0; i < hairObjs.size(); i++)
    {
        hairObjs[i].model->Draw(*shaderProgramImg, *camera);					// Draw the object
    }
}

// Reads data from src to dst mirrored
void copyoverdata(uint8_t* dst, const uint8_t* src, size_t width, size_t height, size_t n)
{
	size_t i;
	dst += n-width;

	for (int h = 0; h < height; h++)
	{
		memcpy(dst, src, width);

		src += width;
		dst -= width;
	}
}

// Gets frame information and stores YUV data
void GLVideoRendererYUV420::updateFrame(const video_frame& frame, int camera_facing)
{
	m_sizeY = frame.width * frame.height;
	m_sizeU = frame.width * frame.height / 4;
	m_sizeV = frame.width * frame.height / 4;

	if (m_pDataY == nullptr || m_width != frame.width || m_height != frame.height || m_cameraFacing != camera_facing)
	{
		m_cameraFacing = camera_facing;
		m_pDataY = std::make_unique<uint8_t[]>(m_sizeY + m_sizeU + m_sizeV);
		m_pDataU = m_pDataY.get() + m_sizeY;
		m_pDataV = m_pDataU + m_sizeU;
		isProgramChanged = true;
	}

	m_width = frame.width;
	m_height = frame.height;

	if (m_width == frame.stride_y)
	{
		if(m_cameraFacing == 0){	// front facing
			copyoverdata(m_pDataY.get(), frame.y, m_width, m_height, m_sizeY);
		}else{
			memcpy(m_pDataY.get(), frame.y, m_sizeY);
		}
	}
	else
	{
		uint8_t* pSrcY = frame.y;
		uint8_t* pDstY;
		if(m_cameraFacing == 0){	// front facing
			pDstY = m_pDataY.get() + m_sizeY - m_width;
		}else{
			pDstY = m_pDataY.get();
		}


		for (int h = 0; h < m_height; h++)
		{
			memcpy(pDstY, pSrcY, m_width);

			pSrcY += frame.stride_y;
			if(m_cameraFacing == 0){	// front facing
				pDstY -= m_width;
			}else{
				pDstY += m_width;
			}

		}
	}

	if (m_width / 2 == frame.stride_uv)
	{
		if(m_cameraFacing == 0){	// front facing
			copyoverdata(m_pDataU, frame.u, m_width/2, m_height/2, m_sizeU);
			copyoverdata(m_pDataV, frame.v, m_width/2, m_height/2, m_sizeV);
		}else{
			memcpy(m_pDataU, frame.u, m_sizeU);
			memcpy(m_pDataV, frame.v, m_sizeV);
		}
	}
	else
	{
		uint8_t* pSrcU = frame.u;
		uint8_t* pSrcV = frame.v;
		uint8_t *pDstU;
		uint8_t *pDstV;
		if(m_cameraFacing == 0) {    // front facing
			pDstU = m_pDataU + m_sizeU - (m_width / 2);
			pDstV = m_pDataV + m_sizeV - (m_width / 2);
		}else{
			pDstU = m_pDataU;
			pDstV = m_pDataV;
		}

		for (int h = 0; h < m_height / 2; h++)
		{
			memcpy(pDstU, pSrcU, m_width / 2);
			memcpy(pDstV, pSrcV, m_width / 2);

			if(m_cameraFacing == 0) {    // front facing
				pDstU -= m_width / 2;
				pDstV -= m_width / 2;
			}else{
				pDstU += m_width / 2;
				pDstV += m_width / 2;
			}

			pSrcU += frame.stride_uv;
			pSrcV += frame.stride_uv;
		}
	}

	isDirty = true;
}

// GLVideoRenderer draw - occurs every preview frame callback!
void GLVideoRendererYUV420::draw(uint8_t *buffer, size_t length, size_t width, size_t height, int rotation, int camera_facing)
{
    m_length = length;
    m_rotation = rotation;

	video_frame frame;
	frame.width = width;
	frame.height = height;
	frame.stride_y = width;
	frame.stride_uv = width / 2;
	frame.y = buffer;
	frame.u = buffer + width * height;
	frame.v = buffer + width * height * 5 / 4;

	updateFrame(frame, camera_facing);
}

void GLVideoRendererYUV420::setAssetManager(AAssetManager *mgr)
{
	assetManager = mgr;
}

AAssetManager * GLVideoRendererYUV420::getAssetManager() {
	return assetManager;
}

void GLVideoRendererYUV420::setInternalFilePaths(std::vector<std::string> file_paths)
{
    internalFilePaths = file_paths;

    faceDetect.init(internalFilePaths);
    faceMesh.init(internalFilePaths);
}

void GLVideoRendererYUV420::setParameters(uint32_t params)
{
	m_params = params;
}

uint32_t GLVideoRendererYUV420::getParameters()
{
    return m_params;
}

bool GLVideoRendererYUV420::createTextures()
{
	if(isDirty){
		GLsizei widthY = (GLsizei)m_width;
		GLsizei heightY = (GLsizei)m_height;

		GLsizei widthUV = (GLsizei)m_width / 2;
		GLsizei heightUV = (GLsizei)m_height / 2;

		if(yuvImgTextures.size() > 0){			// Garbage collection
			for(auto txt : yuvImgTextures){
				txt.Delete();
			}
		}
		yuvImgTextures.clear();

		yuvImgTextures.push_back(Texture(m_pDataY.get(),"s_textureY",0,widthY,heightY,1));		// Y texture
		yuvImgTextures.push_back(Texture(m_pDataU,"s_textureU",1,widthUV,heightUV,1));	// U texture
		yuvImgTextures.push_back(Texture(m_pDataV,"s_textureV",2,widthUV,heightUV,1));	// V texture
	}

	return true;
}

bool GLVideoRendererYUV420::updateTextures()
{
	if (!createTextures()) return false;

	if (isDirty)
    {
        isDirty = false;
        return true;
	}

	return false;
}

void GLVideoRendererYUV420::deleteTextures()
{
	if(yuvImgTextures.size() > 0){			// Garbage collection
		for(auto txt : yuvImgTextures){
			txt.Delete();
		}
	}
	yuvImgTextures.clear();
}

int GLVideoRendererYUV420::createPrograms()
{
	// Shaders have already been created
	if(!shaderProgramsCreated){
		shaderProgramImg = new Shader(imageVertexShader, imageFragmentShader);
        shaderProgramPoint = new Shader(pointVertexShader, pointFragShader);
		shaderProgramsCreated = true;
	}

	// Check if shader programs exist
	if (!shaderProgramImg->ID || !shaderProgramPoint->ID)
    {
        check_gl_error("Create programs");
		LOGE("Could not create programs.");
        return 0;
	}

	return 1;
}

GLuint GLVideoRendererYUV420::useProgram()
{
	// Create shader programs
	if (!createPrograms())
    {
		LOGE("Could not use programs.");
		return 0;
	}

	// Check if the program has changed
	if (isProgramChanged)
    {
		// Configure the camera matrix
		camera = new Camera(m_backingWidth, m_backingHeight, glm::vec3(0.0,0.0,2.415));
		camera->updateMatrix(45.0f, 0.1f, 100.0f);

		// Configure image model matrix
		imgModel = glm::mat4(1.0f);
		if(m_cameraFacing == 0){    // front-facing
            imgModel = glm::rotate(imgModel, -1.57079633f, glm::vec3(0.0f, 0.0f, 1.0f));	// Flip the image 90
		}else{
            imgModel = glm::rotate(imgModel, 1.57079633f, glm::vec3(0.0f, 0.0f, 1.0f));	// Flip the image 90
		}

		// Configure face detection model matrix
		faceDetectModel = faceDetect.genFaceModel(m_cameraFacing);

		isProgramChanged = false;
	}

	return 1;
}
