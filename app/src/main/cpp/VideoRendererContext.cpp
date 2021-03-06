#include "VideoRendererContext.h"
#include "Log.h"

VideoRendererContext::jni_fields_t VideoRendererContext::jni_fields = { 0L };

VideoRendererContext::VideoRendererContext(int type)
{
    m_internalFilePaths = {};
    m_pVideoRenderer = VideoRenderer::create(type);
}

VideoRendererContext::~VideoRendererContext()
{

}

void VideoRendererContext::init(ANativeWindow* window, size_t width, size_t height)
{
    m_pVideoRenderer->init(window, width, height);
}

void VideoRendererContext::render()
{
	m_pVideoRenderer->render();
}

void VideoRendererContext::draw(uint8_t *buffer, size_t length, size_t width, size_t height, int rotation, int camera_facing)
{
    m_pVideoRenderer->draw(buffer, length, width, height, rotation, camera_facing);
}

void VideoRendererContext::setAssetManager(AAssetManager* mgr)
{
    m_assetManager = mgr;
    m_pVideoRenderer->setAssetManager(m_assetManager);
}

AAssetManager* VideoRendererContext::getAssetManager()
{
    return m_assetManager;
}

void VideoRendererContext::setInternalFilePaths(std::vector<std::string> file_paths)
{
    m_internalFilePaths = file_paths;
    m_pVideoRenderer->setInternalFilePaths(m_internalFilePaths);
}

void VideoRendererContext::setParameters(std::vector<std::string> params)
{
    m_pVideoRenderer->setParameters(params);
}

uint32_t VideoRendererContext::getParameters()
{
    return 0;   //TODO::Not implemented
}

void VideoRendererContext::createContext(JNIEnv *env, jobject obj, jint type)
{
    VideoRendererContext* context = new VideoRendererContext(type);

    storeContext(env, obj, context);
}

void VideoRendererContext::storeContext(JNIEnv *env, jobject obj, VideoRendererContext *context)
{
    // Get a reference to this object's class
    jclass cls = env->GetObjectClass(obj);

    if (NULL == cls)
    {
        LOGE("Could not find com/media/camera/preview/render/VideoRenderer.");
        return;
    }

    // Get the Field ID of the "mNativeContext" variable
    jni_fields.context = env->GetFieldID(cls, "mNativeContext", "J");
    if (NULL == jni_fields.context)
    {
        LOGE("Could not find mNativeContext.");
        return;
    }

    env->SetLongField(obj, jni_fields.context, (jlong)context);
}

void VideoRendererContext::deleteContext(JNIEnv *env, jobject obj)
{
    if (NULL == jni_fields.context)
    {
        LOGE("Could not find mNativeContext.");
        return;
    }

    VideoRendererContext* context = reinterpret_cast<VideoRendererContext*>(env->GetLongField(obj, jni_fields.context));

    if (context) delete context;

    env->SetLongField(obj, jni_fields.context, 0L);
}

VideoRendererContext* VideoRendererContext::getContext(JNIEnv *env, jobject obj)
{
    if (NULL == jni_fields.context)
    {
        LOGE("Could not find mNativeContext.");
        return NULL;
    }

    VideoRendererContext* context = reinterpret_cast<VideoRendererContext*>(env->GetLongField(obj, jni_fields.context));

    return context;
}
