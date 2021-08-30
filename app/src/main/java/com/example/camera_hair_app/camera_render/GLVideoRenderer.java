package com.example.camera_hair_app.camera_render;

import android.content.res.AssetManager;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLVideoRenderer extends VideoRenderer implements GLSurfaceView.Renderer {

    private GLSurfaceView mGLSurface;

    public GLVideoRenderer() { create(Type.GL_YUV420_FILTER.getValue()); }

    public void init(GLSurfaceView glSurface, AssetManager mgr){
        //Pass AssetManager to JNI
        setAssetManager(mgr);

        mGLSurface = glSurface;

        //Create an OpenGL ES 2 context
        mGLSurface.setEGLContextClientVersion(2);
        mGLSurface.setRenderer(this);
        mGLSurface.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    public void requestRender() {
        if (mGLSurface != null){
            mGLSurface.requestRender();
        }
    }

    public void destroyRender() { destroy(); }

    public void drawVideoFrame(byte[] data, int width, int height, int rotation, int camera_facing){
        draw(data, width, height, rotation, camera_facing);
    }

    public void setVideoParameters(int params){
        setParameters(params);
    }

    public int getVideoParamaters(){
        return getParameters();
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        render();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        init(null, width, height);
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {

    }
}
