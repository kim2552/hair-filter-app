package com.example.camera_hair_app.camera_render;

import android.content.Context;
import android.graphics.Bitmap;
import android.opengl.GLException;
import android.opengl.GLSurfaceView;
import android.util.Log;
import org.json.simple.*;
import org.json.simple.parser.*;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.nio.IntBuffer;
import java.util.concurrent.CountDownLatch;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLVideoRenderer extends VideoRenderer implements GLSurfaceView.Renderer {

    private static final String TAG = GLVideoRenderer.class.toString();

    private GLSurfaceView mGLSurface;

    private Bitmap snapshotBitmap;
    CountDownLatch snapshotLatch;
    public boolean saveSnapshotBitmap = false;

    InputStream is;
    FileOutputStream os;

    File mCascadeFile;
    File mLandmarkFile;
    File mConfigFile;
    File mHairObjFile;
    File mHairTextFile;
    String[] internalFilePaths = new String[5];

    JSONParser parser = new JSONParser();
    Object jsonObj;


    public GLVideoRenderer() { create(Type.GL_YUV420_FILTER.getValue()); }

    public void init(GLSurfaceView glSurface, Context context){
        try {
            InputStream inptstrm = context.getResources().getAssets().open("configs.json");
            Reader targetReader = new InputStreamReader(inptstrm);
            jsonObj = parser.parse(targetReader);
            JSONObject jsonObject = (JSONObject)jsonObj;
            JSONObject configsObject = (JSONObject)jsonObject;

            JSONObject selectedHairObj = (JSONObject)((JSONObject)configsObject.get("hairs")).get("0");
            String selectedHairObjPath = (String) selectedHairObj.get("path");

            JSONObject selectedHairText = (JSONObject)((JSONObject)configsObject.get("textures")).get("0");
            String selectedHairTextPath = (String)selectedHairText.get("path");

            File dir = context.getDir("data", context.MODE_PRIVATE);
            mConfigFile = new File(dir,"configs.json");
            mCascadeFile = new File(dir, "face_frontal.xml");
            mLandmarkFile = new File(dir, "face_landmark.dat");
            mHairObjFile = new File(dir, "hair.obj");
            mHairTextFile = new File(dir, "hair_texture.png");

            createInternalFiles(context, mConfigFile, "configs.json", 0);
            createInternalFiles(context, mCascadeFile, "data/lbpcascade_frontalface_improved.xml", 1);
            createInternalFiles(context, mLandmarkFile, "data/shape_predictor_81_face_landmarks.dat", 2);
            createInternalFiles(context, mHairObjFile, selectedHairObjPath, 3);
            createInternalFiles(context, mHairTextFile, selectedHairTextPath, 4);

            //Pass AssetManager to JNI
            setAssetManager(context.getResources().getAssets());
            setInternalFiles(internalFilePaths);
        } catch(Exception e) {
            e.printStackTrace();
        }
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

    public void setVideoParameters(int params, Context context){
        String hairObjParams = String.valueOf(params);

        JSONObject jsonObject = (JSONObject)jsonObj;
        JSONObject configsObject = (JSONObject)jsonObject;

        JSONObject selectedHairObj = (JSONObject)((JSONObject)configsObject.get("hairs")).get(hairObjParams);
        String selectedHairObjPath = (String) selectedHairObj.get("path");

        JSONObject selectedHairText = (JSONObject)((JSONObject)configsObject.get("textures")).get("0");
        String selectedHairTextPath = (String)selectedHairText.get("path");

        createInternalFiles(context, mHairObjFile, selectedHairObjPath, 3);
        createInternalFiles(context, mHairTextFile, selectedHairTextPath, 4);

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

    public Bitmap getSnapshotBitmap() throws InterruptedException {
        snapshotLatch = new CountDownLatch(1);
        saveSnapshotBitmap = true;
        snapshotLatch.await();
        return snapshotBitmap;
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        render();
        if(saveSnapshotBitmap){
            snapshotBitmap = createBitmapFromGLSurface(0,0, mGLSurface.getWidth(), mGLSurface.getHeight(), gl);
            saveSnapshotBitmap = false;
            snapshotLatch.countDown();
        }
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        init(null, width, height);
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {

    }

    private Bitmap createBitmapFromGLSurface(int x, int y, int w, int h, GL10 gl)
            throws OutOfMemoryError {
        int bitmapBuffer[] = new int[w * h];
        int bitmapSource[] = new int[w * h];
        IntBuffer intBuffer = IntBuffer.wrap(bitmapBuffer);
        intBuffer.position(0);

        try {
            gl.glReadPixels(x, y, w, h, GL10.GL_RGBA, GL10.GL_UNSIGNED_BYTE, intBuffer);
            int offset1, offset2;
            for (int i = 0; i < h; i++) {
                offset1 = i * w;
                offset2 = (h - i - 1) * w;
                for (int j = 0; j < w; j++) {
                    int texturePixel = bitmapBuffer[offset1 + j];
                    int blue = (texturePixel >> 16) & 0xff;
                    int red = (texturePixel << 16) & 0x00ff0000;
                    int pixel = (texturePixel & 0xff00ff00) | red | blue;
                    bitmapSource[offset2 + j] = pixel;
                }
            }
        } catch (GLException e) {
            return null;
        }

        return Bitmap.createBitmap(bitmapSource, w, h, Bitmap.Config.ARGB_8888);
    }
}
