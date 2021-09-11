package com.example.camera_hair_app.camera_render;

import android.content.Context;
import android.content.res.AssetManager;
import android.content.res.Resources;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.Log;

import com.example.camera_hair_app.camera_activity.GLViewActivity;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLVideoRenderer extends VideoRenderer implements GLSurfaceView.Renderer {

    private static final String TAG = GLVideoRenderer.class.toString();

    private GLSurfaceView mGLSurface;

    InputStream is;
    FileOutputStream os;

    File mCascadeFile;
    File mLandmarkFile;
    String[] internalFilePaths = new String[2];


    public GLVideoRenderer() { create(Type.GL_YUV420_FILTER.getValue()); }

    public void init(GLSurfaceView glSurface, Context context){
        File dir = context.getDir("data", context.MODE_PRIVATE);
        mCascadeFile = new File(dir, "face_frontal.xml");
        mLandmarkFile = new File(dir, "face_landmark.xml");

        createInternalFiles(context, mCascadeFile, "data/haarcascade_frontalface_default.xml", 0);
        createInternalFiles(context, mLandmarkFile, "data/face_landmark_model.dat", 1);
        //Pass AssetManager to JNI
        setAssetManager(context.getResources().getAssets());
        setInternalFiles(internalFilePaths);

        mGLSurface = glSurface;

        //Create an OpenGL ES 3 context
        mGLSurface.setEGLContextClientVersion(3);
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

    private void createInternalFiles(Context context, File fileObj, String filepath, int index){
        try {
            is = context.getResources().getAssets().open(filepath);

            FileOutputStream os;
            os = new FileOutputStream(fileObj);

            byte[] buffer = new byte[4096];
            int bytesRead;
            while ((bytesRead = is.read(buffer)) != -1) {
                os.write(buffer, 0, bytesRead);
            }

            is.close();
            os.close();

            internalFilePaths[index] = fileObj.getAbsolutePath();
        } catch (IOException e) {
            Log.i(TAG, filepath+" not found");
        }
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
