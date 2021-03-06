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

    // Configure the camera matrix
    camera = new Camera(m_backingWidth, m_backingHeight, glm::vec3(0.0,0.0,2.415));
    camera->updateMatrix(45.0f, 0.1f, 100.0f);

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
    faceDetectObj = faceDetect.getFaceObject(m_pDataY.get(), m_width, m_height, m_cameraFacing);

    // Create a mesh object from the face detect
    if (faceDetectObj.detected){
		yuvImgTextures.push_back(Texture(faceDetect.m_face_mask_image, "transparency", 3, m_width, m_height,1));
		m_faceMask.loadTextures(&yuvImgTextures);
		m_faceMask.loadFaceDetectObj(faceDetectObj, &faceDetect);
    }
//
//	/*******************************************************/
//
	m_cameraImage.loadTextures(&yuvImgTextures);
	m_cameraImage.initCameraImage();
//
//	/*******************************************************/
//
	if (faceDetectObj.detected) {
		m_hairObject.updateHairObject(&m_faceMask);
	}

	/*******************************************************/
    // Draw all objects
    m_cameraImage.drawCameraImage(*camera);
	if (faceDetectObj.detected) {
		m_hairObject.drawHairObject(*camera);
		glEnable(GL_BLEND);
		m_faceMask.drawMaskImage(*camera);
		glDisable(GL_BLEND);
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

void GLVideoRendererYUV420::setParameters(std::vector<std::string> params)
{
	m_params = params;
}

std::vector<std::string> GLVideoRendererYUV420::getParameters()
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
		// Load configs
		config.LoadConfig(internalFilePaths[0],m_params[0]);
		config.params.hair_obj = internalFilePaths[3];

		// Initialize image objects
		m_cameraImage.loadImageAspectRatio(float(m_width)/float(m_height));
		m_cameraImage.loadShaders(shaderProgramImg);
		m_cameraImage.setCameraFacing(m_cameraFacing);
		m_faceMask.loadImageAspectRatio(float(m_width)/float(m_height));
		m_faceMask.loadShaders(shaderProgramMask);
		m_faceMask.setCameraFacing(m_cameraFacing);
		m_hairObject.loadImageAspectRatio(float(m_width)/float(m_height));
		m_hairObject.loadShaders(shaderProgramModel);

        unsigned char* hairimage;
        int file_size;
        AAssetDir* assetDirImg = AAssetManager_openDir(assetManager, "hair");
        AAsset* asset = AAssetManager_open(assetManager, config.params.hair_texture.c_str(), AASSET_MODE_UNKNOWN);
        file_size = AAsset_getLength(asset);
        hairimage = (unsigned char*) malloc (sizeof(unsigned char)*file_size);
        AAsset_read(asset,hairimage,file_size);
        AAsset_close(asset);
        AAssetDir_close(assetDirImg);

        // Initialize texture
        hairTextures.clear();
        hairTextures.push_back(Texture(hairimage,file_size, "diffuse", 0));
        m_hairObject.loadTextures(&hairTextures);

        // Initialize hair object
		m_hairObject.loadTextures(&hairTextures);
		glm::vec3 hair_color;

		float r = float(std::stoul(m_params[1].substr(2,2), nullptr, 16))/255.0;
		float g = float(std::stoul(m_params[1].substr(4,2), nullptr, 16))/255.0;
		float b = float(std::stoul(m_params[1].substr(6,2), nullptr, 16))/255.0;
		hair_color = glm::vec3(r,g,b);

		m_hairObject.initHairObject(&config, hair_color);

		isProgramChanged = false;
	}
    return 1;

}