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
	, m_pDataA(nullptr)
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
        if(strcmp(filename, "blonde.png") == 0){
            AAsset* asset = AAssetManager_open(assetManager, "hair/blonde.png", AASSET_MODE_UNKNOWN);
            file_size = AAsset_getLength(asset);
            hairimage = (unsigned char*) malloc (sizeof(unsigned char)*file_size);
            AAsset_read(asset,hairimage,file_size);
            AAsset_close(asset);
        }
    }
    AAssetDir_close(assetDirImg);

    // Initialize texture
    hairTextures.push_back(Texture(hairimage,file_size, "diffuse", 0));
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// GLVideoRenderer render - occurs every new camera image
void GLVideoRendererYUV420::render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Update textures and create shader programs
	if (!updateTextures() || !useProgram()) return;

	// Detect face(s) and get landmark points
    faceObjs = faceDetect.getFaceLandmarks(m_pDataY.get(), m_width, m_height, m_cameraFacing);

    // Create a mesh object from the face detect
    for(auto fdMesh : faceDetectMeshes) fdMesh.Delete();
    faceDetectMeshes.clear();
    for (size_t i = 0; i < faceObjs.size(); i++)							// Loop through all the faces
    {
        faceDetectMeshes.push_back(faceDetect.genFaceMesh(faceObjs[i].shape));
    }

	/*******************************************************/
	/*********** TODO::Create class and optimize ***********/

    // Store the mask image
    yuvImgTextures.push_back(Texture(faceDetect.face_mask_image, "transparency", 3, m_width, m_height,1));

	// Store mesh data in vectors for the mesh
	std::vector <Vertex> mVerts(imgVerts, imgVerts + sizeof(imgVerts) / sizeof(Vertex));
	std::vector <GLuint> mInds(imgInds, imgInds + sizeof(imgInds) / sizeof(GLuint));
	// Create image mesh
	Mesh maskMesh(mVerts, mInds, yuvImgTextures);

	glm::mat4 maskModel = glm::mat4(1.0f);
	if(m_cameraFacing == 0){    // front-facing
		maskModel = glm::rotate(maskModel, -1.57079633f, glm::vec3(0.0f, 0.0f, 1.0f));	// Flip the image 90
	}else{
		maskModel = glm::rotate(maskModel, 1.57079633f, glm::vec3(0.0f, 0.0f, 1.0f));	// Flip the image 90
	}
	maskModel = glm::scale(maskModel, glm::vec3(0.6f, 0.6f, 1.0f));
	glm::mat4 translation = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, -0.0075f, -1.0f));
	maskModel = translation * maskModel;

	glm::mat4 testModel = glm::mat4(1.0f);
	testModel = glm::scale(testModel, glm::vec3(0.6f, 0.6f, 1.0f));
	glm::mat4 trans = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 0.0f, -1.0f));
	testModel = trans * testModel;

	for (size_t i = 0; i < faceObjs.size(); i++)							// Loop through all the faces
	{
		cv::Point th = faceObjs[i].topHeadPoint;
		cv::Point bh = faceObjs[i].botHeadPoint;
		cv::Point rc = faceObjs[i].rightHeadPoint;
		cv::Point lc = faceObjs[i].leftHeadPoint;
		glm::vec4 vth(glm::vec3(0.75 - (th.x * IMAGE_ASPECT_RATIO / (float)RESIZED_IMAGE_WIDTH * 2), 1 - (th.y / (float)RESIZED_IMAGE_HEIGHT * 2), 1.0f), 1.0f);
		glm::vec4 vbh(glm::vec3(0.75 - (bh.x * IMAGE_ASPECT_RATIO / (float)RESIZED_IMAGE_WIDTH * 2), 1 - (bh.y / (float)RESIZED_IMAGE_HEIGHT * 2), 1.0f), 1.0f);
		glm::vec4 vrc(glm::vec3(0.75 - (rc.x * IMAGE_ASPECT_RATIO / (float)RESIZED_IMAGE_WIDTH * 2), 1 - (rc.y / (float)RESIZED_IMAGE_HEIGHT * 2), 1.0f), 1.0f);
		glm::vec4 vlc(glm::vec3(0.75 - (lc.x * IMAGE_ASPECT_RATIO / (float)RESIZED_IMAGE_WIDTH * 2), 1 - (lc.y / (float)RESIZED_IMAGE_HEIGHT * 2), 1.0f), 1.0f);
		vth = testModel * vth;
		vbh = testModel * vbh;
		vrc = testModel * vrc;
		vlc = testModel * vlc;
		faceObjs[i].topHeadCoord = vth;
		faceObjs[i].width = glm::length(vrc - vlc);
		faceObjs[i].height = glm::length(vth - vbh);
	}

	/*******************************************************/

	// Create image mesh for camera preview
	std::vector <Vertex> yuvImgV(imgVerts, imgVerts + sizeof(imgVerts) / sizeof(Vertex));
	std::vector <GLuint> yuvImgI(imgInds, imgInds + sizeof(imgInds) / sizeof(GLuint));
	imgMesh = new Mesh (yuvImgV, yuvImgI, yuvImgTextures);

	/*******************************************************/
	/*********** TODO::Create class and optimize ***********/
    std::vector<ModelObj> hairObjs;

    for (auto fom : faceObjs)
    {
        hairObj->model->savedIdx = saved_front_vert_index;
        // Transfer face mesh data to hair object
        hairObj->model->topHeadCoord = fom.topHeadCoord;
        hairObj->model->faceWidth = fom.width;
        hairObj->model->faceHeight = fom.height;

        // Activate shader for Object and configure the model matrix
        glm::mat4 hairObjectModel = glm::mat4(1.0f);

        // Calculate the scale of the hair object
		hairObj->model->savedRatioWidth = saved_ratio_width;				// Width ratio value based on developer preference
		hairObj->model->savedRatioHeight = saved_ratio_height;			// Height ratio value based on developer preference
		hairObj->model->savedScaleZ = saved_scale_z;				// Constant Z scale value based on developer preference
		hairObj->model->originalModelWidth = glm::length(hairObj->model->originalBb.max.x - hairObj->model->originalBb.min.x);
		hairObj->model->originalModelHeight = glm::length(hairObj->model->originalBb.max.y - hairObj->model->originalBb.min.y);
		float scaleMultWidth = hairObj->model->savedRatioWidth * hairObj->model->faceWidth / hairObj->model->originalModelWidth;
		float scaleMultHeight = hairObj->model->savedRatioHeight * hairObj->model->faceHeight / hairObj->model->originalModelHeight;
		hairObjectModel = glm::scale(hairObjectModel, glm::vec3(scaleMultWidth, scaleMultHeight, hairObj->model->savedScaleZ));

        // Rotate object to match face direction
        hairObj->model->facePitch = fom.pitch;
        hairObj->model->faceYaw = fom.yaw;
        hairObj->model->faceRoll = fom.roll;
        hairObj->model->savedPitch = saved_pitch;
        hairObj->model->savedYaw = saved_yaw;
        hairObj->model->savedRoll = saved_roll;
		hairObjectModel = glm::rotate(hairObjectModel, glm::radians(hairObj->model->facePitch + hairObj->model->savedPitch), glm::vec3(1.0f, 0.0f, 0.0f));
		hairObjectModel = glm::rotate(hairObjectModel, glm::radians(hairObj->model->faceYaw + hairObj->model->savedYaw), glm::vec3(0.0f, 1.0f, 0.0f));
		hairObjectModel = glm::rotate(hairObjectModel, glm::radians(hairObj->model->faceRoll + hairObj->model->savedRoll), glm::vec3(0.0f, 0.0f, 1.0f));

        hairObj->model->UpdateModel(hairObjectModel);			// Updates the position and bounding box of the scaled, rotated object
		hairObj->model->savedTopHeadDist.x = saved_topheadx;		// Value obtained from fixedVertex distance from topHeadCoord
		hairObj->model->savedTopHeadDist.y = saved_topheady;		// Value obtained from fixedVertex distance from topHeadCoord
		hairObj->model->savedTopHeadDist.z = saved_topheadz;		// Value obtained from fixedVertex distance from topHeadCoord

        float transX = (hairObj->model->savedTopHeadDist.x + hairObj->model->topHeadCoord.x) - hairObj->model->fixedVertex.x;
        float transY = (hairObj->model->savedTopHeadDist.y + hairObj->model->topHeadCoord.y) - hairObj->model->fixedVertex.y;
        float transZ = (hairObj->model->savedTopHeadDist.z + hairObj->model->topHeadCoord.z) - hairObj->model->fixedVertex.z;

        glm::mat4 translation = glm::translate(glm::mat4(1.f), glm::vec3(transX, transY, transZ));
        hairObjectModel = translation * hairObjectModel;

		// Take care of all the light related things
		glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
        glUniform4f(glGetUniformLocation(shaderProgramModel->ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
        glUniform3f(glGetUniformLocation(shaderProgramModel->ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        hairObj->model->UpdateModel(hairObjectModel);																// Update object model
        hairObjs.push_back(*hairObj);
    }

	/*******************************************************/
    // Draw all meshes
	imgMesh->Draw(*shaderProgramImg, *camera, imgModel);
//	for (size_t i = 0; i < faceDetectMeshes.size(); i++)
//	{
//		faceDetectMeshes[i].Draw(*shaderProgramPoint, *camera, faceDetectModel);
//	}
	glEnable(GL_BLEND);
    for (size_t i = 0; i < hairObjs.size(); i++)
    {
        hairObjs[i].model->Draw(*shaderProgramModel, *camera);					// Draw the object
    }
	maskMesh.Draw(*shaderProgramMask, *camera, maskModel);
	glDisable(GL_BLEND);
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
		m_pDataA = std::make_unique<uint8_t[]>(m_sizeY);
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
		shaderProgramMask = new Shader(imageVertexShader, imageMaskFragmentShader);
        shaderProgramPoint = new Shader(pointVertexShader, pointFragShader);
        shaderProgramModel = new Shader(modelVertexShader, modelFragmentShader);
		shaderProgramsCreated = true;
	}

	// Check if shader programs exist
	if (!shaderProgramImg->ID || !shaderProgramPoint->ID || !shaderProgramModel->ID || !shaderProgramMask->ID)
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
		std::string configstext = get_file_contents(internalFilePaths[0].c_str());
		json configsJSON = json::parse(configstext);
		std::string hair_obj = std::to_string(m_params);
		saved_ratio_width = configsJSON["hairs"][hair_obj]["transformations"]["ratio_width"];
		saved_ratio_height = configsJSON["hairs"][hair_obj]["transformations"]["ratio_height"];
		saved_scale_z = configsJSON["hairs"][hair_obj]["transformations"]["scale_z"];
		saved_pitch = configsJSON["hairs"][hair_obj]["transformations"]["pitch"];
		saved_yaw = configsJSON["hairs"][hair_obj]["transformations"]["yaw"];
		saved_roll = configsJSON["hairs"][hair_obj]["transformations"]["roll"];
		saved_topheadx = configsJSON["hairs"][hair_obj]["transformations"]["topheadx"];
		saved_topheady = configsJSON["hairs"][hair_obj]["transformations"]["topheady"];
		saved_topheadz = configsJSON["hairs"][hair_obj]["transformations"]["topheadz"];
		saved_front_vert_index = configsJSON["hairs"][hair_obj]["transformations"]["front_vert_index"];

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

        // Initialize model object
        std::string filename = internalFilePaths[3];
        hairObj = new ModelObj(filename, hairTextures);

		isProgramChanged = false;
	}

	return 1;
}
