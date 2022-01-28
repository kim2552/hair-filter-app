#ifndef _H_VIDEO_RENDERER_CONTEXT_
#define _H_VIDEO_RENDERER_CONTEXT_

#include "VideoRenderer.h"

#include <memory>
#include <jni.h>
#include <android/asset_manager.h>

class VideoRendererContext
{
public:
	struct jni_fields_t
	{
		jfieldID context;
	};

	VideoRendererContext(int type);
	~VideoRendererContext();

    void init(ANativeWindow* window, size_t width, size_t height);
	void render();
	void draw(uint8_t *buffer, size_t length, size_t width, size_t height, int rotation, int camera_facing);
	void setAssetManager(AAssetManager* mgr);
	AAssetManager* getAssetManager();
	void setInternalFilePaths(std::vector<std::string> file_paths);
	void setParameters(std::vector<std::string> params);
	uint32_t getParameters();

	static void createContext(JNIEnv *env, jobject obj, jint type);
	static void storeContext(JNIEnv *env, jobject obj, VideoRendererContext *context);
	static void deleteContext(JNIEnv *env, jobject obj);
	static VideoRendererContext* getContext(JNIEnv *env, jobject obj);

private:
    std::unique_ptr<VideoRenderer> m_pVideoRenderer;

	static jni_fields_t jni_fields;

	AAssetManager* m_assetManager;
	std::vector<std::string> m_internalFilePaths;
	std::vector<std::string> m_internalObjParams;
};

#endif // _H_VIDEO_RENDERER_CONTEXT_
