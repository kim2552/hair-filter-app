#ifndef _H_VIDEO_RENDER_JNI_
#define _H_VIDEO_RENDER_JNI_

#include <jni.h>
#include <assert.h>
#include <android/asset_manager_jni.h>
#include <vector>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#define JCMCPRV(rettype, name)                                             \
  rettype JNIEXPORT JNICALL Java_com_example_camera_1hair_1app_camera_1render_VideoRenderer_##name

static AAssetManager* asset_manager;
std::vector<std::string> internal_file_paths;
std::vector<std::string> internal_obj_params;
const char* cascade_file_path;

JCMCPRV(void, create)(JNIEnv * env, jobject obj, jint type);
JCMCPRV(void, destroy)(JNIEnv * env, jobject obj);
JCMCPRV(void, init)(JNIEnv * env, jobject obj, jobject surface, jint width, jint height);
JCMCPRV(void, render)(JNIEnv * env, jobject obj);
JCMCPRV(void, draw)(JNIEnv * env, jobject obj, jbyteArray data, jint width, jint height, jint rotation, jint camera_facing);
JCMCPRV(void, setAssetManager)(JNIEnv * env, jobject obj, jobject asset_manager);
JCMCPRV(void, setInternalFiles)(JNIEnv * env, jobject obj, jobjectArray filepaths);
JCMCPRV(void, setParameters)(JNIEnv * env, jobject obj, jobjectArray params);
JCMCPRV(jint, getParameters)(JNIEnv * env, jobject obj);

#ifdef __cplusplus
}
#endif

#endif // _H_VIDEO_RENDER_JNI_
