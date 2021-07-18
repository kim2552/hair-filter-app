package com.example.camera_hair_app.camera_render;

import android.view.Surface;

public abstract class VideoRenderer {
    protected enum Type{
        GL_YUV420(0), GL_YUV420_FILTER(1);

        private int mValue;

        Type(int value){ mValue = value; }

        public int getValue() { return mValue; }
    }
    private long mNativeContext;

    protected native void create(int type);
    protected native void destroy();
    protected native void init(Surface surface, int width, int height);
    protected native void render();
    protected native void draw(byte[] data, int width, int height, int rotation);
    protected native void setParameters(int params);
    protected native int getParameters();

    static {
        System.loadLibrary("media-lib");
    }
}
