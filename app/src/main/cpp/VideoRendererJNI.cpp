#include "VideoRendererJNI.h"
#include "VideoRendererContext.h"

#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>

JCMCPRV(void, create)(JNIEnv * env, jobject obj, jint type)
{
    VideoRendererContext::createContext(env, obj, type);
}

JCMCPRV(void, destroy)(JNIEnv * env, jobject obj)
{
    VideoRendererContext::deleteContext(env, obj);
}

JCMCPRV(void, init)(JNIEnv * env, jobject obj, jobject surface, jint width, jint height)
{
    VideoRendererContext* context = VideoRendererContext::getContext(env, obj);

	ANativeWindow *window = surface ? ANativeWindow_fromSurface(env, surface) : nullptr;

    if (context){
    	context->setInternalFilePaths(internal_file_paths);
        context->setAssetManager(asset_manager);
        context->init(window, (size_t)width, (size_t)height);
    }
}

JCMCPRV(void, render)(JNIEnv * env, jobject obj)
{
	VideoRendererContext* context = VideoRendererContext::getContext(env, obj);

	if (context) context->render();
}

JCMCPRV(void, draw)(JNIEnv * env, jobject obj, jbyteArray data, jint width, jint height, jint rotation, jint camera_facing)
{
	jbyte* bufferPtr = env->GetByteArrayElements(data, 0);

	jsize arrayLength = env->GetArrayLength(data);

	VideoRendererContext* context = VideoRendererContext::getContext(env, obj);

	if (context) context->draw((uint8_t *)bufferPtr, (size_t)arrayLength, (size_t)width, (size_t)height, rotation, camera_facing);

	env->ReleaseByteArrayElements(data, bufferPtr, 0);
}

JCMCPRV(void, setAssetManager)(JNIEnv * env, jobject obj, jobject mgr)
{
    asset_manager = AAssetManager_fromJava(env, mgr);
}

JCMCPRV(void, setInternalFiles)(JNIEnv * env, jobject obj, jobjectArray filepaths)
{
	int stringCount = env->GetArrayLength(filepaths);

	for (int i=0; i<stringCount; i++) {
		jstring string = (jstring) (env->GetObjectArrayElement(filepaths, i));
		const char* file_path = env->GetStringUTFChars(string, 0);
		internal_file_paths.push_back(std::string(file_path));
		// Don't forget to call `ReleaseStringUTFChars` when you're done.
//		env->ReleaseStringUTFChars(string, cascade_file_path);
	}
}

JCMCPRV(void, setParameters)(JNIEnv * env, jobject obj, jint params)
{
	VideoRendererContext* context = VideoRendererContext::getContext(env, obj);

	if (context) context->setParameters((uint32_t)params);
}

JCMCPRV(jint, getParameters)(JNIEnv * env, jobject obj)
{
	VideoRendererContext* context = VideoRendererContext::getContext(env, obj);

	if (context) return context->getParameters();

	return 0;
}
