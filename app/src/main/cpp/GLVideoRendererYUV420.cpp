#include "GLVideoRendererYUV420.h"
#include "GLShaders.h"
#include "CommonUtils.h"
#include "Log.h"

// Vertices for a full screen quad.
static const float kVertices[8] = {
  -1.f, 1.f,
  -1.f, -1.f,
  1.f, 1.f,
  1.f, -1.f
};

// Texture coordinates for mapping entire texture.
static const float kTextureCoords[8] = {
  0, 0,
  0, 1,
  1, 0,
  1, 1
};

// Vertices coordinates
Vertex imgVerts[] =
		{ //               COORDINATES           /            NORMALS          /           COLORS         /       TEXCOORDS         //
				Vertex{glm::vec3(-1.0f, 1.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
				Vertex{glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
				Vertex{glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
				Vertex{glm::vec3(1.0f, 1.0f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
		};

// Indices for vertices order
static const GLuint imgInds[] =
		{
				0, 1, 2,
				0, 2, 3
		};

// Vertices coordinates
static const Vertex pointVerts[4] =
		{ //               COORDINATES           /            NORMALS          /           COLORS         /       TEXCOORDS         //
				Vertex{glm::vec3(-0.5f, 0.5f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
				Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
				Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
				Vertex{glm::vec3(0.5f, 0.5f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
		};

// Indices for vertices order
static const GLuint pointInds[6] =
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
}

GLVideoRendererYUV420::~GLVideoRendererYUV420()
{
	deleteTextures();
	delete_program(m_program);
}

void GLVideoRendererYUV420::init(ANativeWindow* window, size_t width, size_t height)
{
    m_backingWidth = width;
    m_backingHeight = height;

    char* pointvert = (char*)NULL;
    char* pointfrag = (char*)NULL;
    char* imagevert = (char*)NULL;
    char* imagefrag = (char*)NULL;

	AAssetDir* assetDir = AAssetManager_openDir(assetManager, "shaders");
	const char* filename = (const char*)NULL;
	while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL) {
		if(strcmp(filename, "point.vert") == 0){
			AAsset* asset = AAssetManager_open(assetManager, "shaders/point.vert", AASSET_MODE_UNKNOWN);
			long size = AAsset_getLength(asset);
			pointvert = (char*) malloc (sizeof(char)*size);
			AAsset_read(asset,pointvert,size);
			AAsset_close(asset);
		}
		if(strcmp(filename, "point.frag") == 0){
			AAsset* asset = AAssetManager_open(assetManager, "shaders/point.frag", AASSET_MODE_UNKNOWN);
			long size = AAsset_getLength(asset);
			pointfrag = (char*) malloc (sizeof(char)*size);
			AAsset_read(asset,pointfrag,size);
			AAsset_close(asset);
		}
		if(strcmp(filename, "image.vert") == 0){
			AAsset* asset = AAssetManager_open(assetManager, "shaders/image.vert", AASSET_MODE_UNKNOWN);
			long size = AAsset_getLength(asset);
			imagevert = (char*) malloc (sizeof(char)*size);
			AAsset_read(asset,imagevert,size);
			AAsset_close(asset);
		}
		if(strcmp(filename, "image.frag") == 0){
			AAsset* asset = AAssetManager_open(assetManager, "shaders/image.frag", AASSET_MODE_UNKNOWN);
			long size = AAsset_getLength(asset);
			imagefrag = (char*) malloc (sizeof(char)*size);
			AAsset_read(asset,imagefrag,size);
			AAsset_close(asset);
		}
	}
	AAssetDir_close(assetDir);

    //TODO::Move this somewhere else
    shaderProgramPoint = new Shader(pointvert, pointfrag);
	shaderProgramImg = new Shader(imagevert, imagefrag);
    camera = new Camera(m_backingWidth, m_backingHeight, glm::vec3(0.0,0.0,2.5));

	unsigned char* redpng;
	int file_size;
	AAssetDir* assetDirImg = AAssetManager_openDir(assetManager, "colors");
	while ((filename = AAssetDir_getNextFileName(assetDirImg)) != NULL) {
		if(strcmp(filename, "red.png") == 0){
			AAsset* asset = AAssetManager_open(assetManager, "colors/red.png", AASSET_MODE_UNKNOWN);
			file_size = AAsset_getLength(asset);
			redpng = (unsigned char*) malloc (sizeof(unsigned char)*file_size);
			AAsset_read(asset,redpng,file_size);
			AAsset_close(asset);
		}

	}
	AAssetDir_close(assetDirImg);

    pointTextures.push_back(Texture(redpng,file_size,"diffuse",0));

	// Store mesh data in vectors for the mesh
	std::vector <Vertex> pVerts(pointVerts, pointVerts + sizeof(pointVerts) / sizeof(Vertex));
	std::vector <GLuint> pInds(pointInds, pointInds + sizeof(pointInds) / sizeof(GLuint));

    pointMesh = new Mesh(pVerts,pInds,pointTextures);

	// Activate shader for Face Mask and configure the model matrix
	shaderProgramPoint->Activate();
	pointModel = glm::mat4(1.0f);
//	pointModel = glm::scale(pointModel, glm::vec3(1.0/20.0, 1.0/20.0, 1.0/20.0));
	glm::mat4 translation = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, -0.5));
	pointModel = translation * pointModel;

	// Activate shader for Image and configure the model matrix
	shaderProgramImg->Activate();
	glm::mat4 imgModel = glm::mat4(1.0f);
	imgModel = glm::rotate(imgModel, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));	// Flip the image
	imgModel = glm::rotate(imgModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));	// Flip the image
	for (size_t i = 0; i < 4; i++) {		// Height stays as 2.0, Width needs to change based on aspect ratio
		imgVerts[i].position.x *= (float)m_backingWidth / (float)m_backingHeight;
	}
	glm::mat4 tr = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, -0.0));
	imgModel = tr * imgModel;
}

void GLVideoRendererYUV420::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Image texture data
	imgTextures.clear();	//TODO::Convert YUV to RGB image and pass it to imgTextures
	imgTextures.push_back(Texture(m_pDataY.get(),"diffuse", 0, m_backingWidth, m_backingHeight, 1));						// Texture object deletes imgBytes afterwards
	// Store mesh data in vectors for the mesh
	std::vector <Vertex> iVerts(imgVerts, imgVerts + sizeof(imgVerts) / sizeof(Vertex));
	std::vector <GLuint> iInds(imgInds, imgInds + sizeof(imgInds) / sizeof(GLuint));
	// Create image mesh
	Mesh imgMesh(iVerts, iInds, imgTextures);

	imgMesh.Draw(*shaderProgramImg, *camera, imgModel);		// Draw the image
	pointMesh->Draw(*shaderProgramPoint, *camera, pointModel);

    if (!updateTextures() || !useProgram()) return;

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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
    GLsizei widthY = (GLsizei)m_width;
    GLsizei heightY = (GLsizei)m_height;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &m_textureIdY);
	glBindTexture(GL_TEXTURE_2D, m_textureIdY);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, widthY, heightY, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);

    if (!m_textureIdY)
    {
        check_gl_error("Create Y texture");
        return false;
    }

	GLsizei widthU = (GLsizei)m_width / 2;
    GLsizei heightU = (GLsizei)m_height / 2;

	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &m_textureIdU);
	glBindTexture(GL_TEXTURE_2D, m_textureIdU);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, widthU, heightU, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);

    if (!m_textureIdU)
    {
        check_gl_error("Create U texture");
        return false;
    }

    GLsizei widthV = (GLsizei)m_width / 2;
    GLsizei heightV = (GLsizei)m_height / 2;

	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1, &m_textureIdV);
	glBindTexture(GL_TEXTURE_2D, m_textureIdV);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, widthV, heightV, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);

    if (!m_textureIdV)
    {
        check_gl_error("Create V texture");
        return false;
    }

	return true;
}

bool GLVideoRendererYUV420::updateTextures()
{
	if (!m_textureIdY && !m_textureIdU && !m_textureIdV && !createTextures()) return false;

	if (isDirty)
    {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textureIdY);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (GLsizei)m_width, (GLsizei)m_height, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, m_pDataY.get());

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_textureIdU);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (GLsizei)m_width / 2, (GLsizei)m_height / 2, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, m_pDataU);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_textureIdV);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (GLsizei)m_width / 2, (GLsizei)m_height / 2, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, m_pDataV);

        isDirty = false;

        return true;
	}

	return false;
}

void GLVideoRendererYUV420::deleteTextures()
{
	if (m_textureIdY)
    {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &m_textureIdY);

		m_textureIdY = 0;
	}

	if (m_textureIdU)
    {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &m_textureIdU);

		m_textureIdU = 0;
	}

	if (m_textureIdV)
    {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &m_textureIdV);

		m_textureIdV = 0;
	}
}

int GLVideoRendererYUV420::createProgram(const char *pVertexSource, const char *pFragmentSource)
{
	m_program = create_program(pVertexSource, pFragmentSource, m_vertexShader, m_pixelShader);

	if (!m_program)
    {
        check_gl_error("Create program");
		LOGE("Could not create program.");
        return 0;
	}

	m_vertexPos = (GLuint)glGetAttribLocation(m_program, "position");
	m_uniformProjection = glGetUniformLocation(m_program, "projection");
    m_uniformRotation = glGetUniformLocation(m_program, "rotation");
    m_uniformScale = glGetUniformLocation(m_program, "scale");
	m_textureYLoc = glGetUniformLocation(m_program, "s_textureY");
	m_textureULoc = glGetUniformLocation(m_program, "s_textureU");
	m_textureVLoc = glGetUniformLocation(m_program, "s_textureV");
	m_textureSize = glGetUniformLocation(m_program, "texSize");
	m_textureLoc = (GLuint)glGetAttribLocation(m_program, "texcoord");

	return m_program;
}

GLuint GLVideoRendererYUV420::useProgram()
{
	if (!m_program && !createProgram(kVertexShader, kFragmentShader))
    {
		LOGE("Could not use program.");
		return 0;
	}

	if (isProgramChanged)
    {
		glUseProgram(m_program);

        check_gl_error("Use program.");

		glVertexAttribPointer(m_vertexPos, 2, GL_FLOAT, GL_FALSE, 0, kVertices);
		glEnableVertexAttribArray(m_vertexPos);

		float targetAspectRatio = (float)m_width / (float)m_height;

		GLfloat projection[16];
        mat4f_load_ortho(-1.0f, 1.0f, -targetAspectRatio, targetAspectRatio, -1.0f, 1.0f, projection);
		glUniformMatrix4fv(m_uniformProjection, 1, GL_FALSE, projection);

        GLfloat rotationZ[16];
        mat4f_load_rotation_z(m_rotation, rotationZ);
        glUniformMatrix4fv(m_uniformRotation, 1, 0, &rotationZ[0]);

        float scaleFactor = aspect_ratio_correction(false, m_backingWidth, m_backingHeight, m_width, m_height);

        GLfloat scale[16];
        mat4f_load_scale(scaleFactor, scaleFactor, 1.0f, scale);
        glUniformMatrix4fv(m_uniformScale, 1, 0, &scale[0]);

		glUniform1i(m_textureYLoc, 0);
		glUniform1i(m_textureULoc, 1);
		glUniform1i(m_textureVLoc, 2);
		glVertexAttribPointer(m_textureLoc, 2, GL_FLOAT, GL_FALSE, 0, kTextureCoords);
		glEnableVertexAttribArray(m_textureLoc);

		if (m_textureSize >= 0) {
			GLfloat size[2];
			size[0] = m_width;
			size[1] = m_height;
			glUniform2fv(m_textureSize, 1, &size[0]);
		}

		isProgramChanged = false;
	}

	return m_program;
}
