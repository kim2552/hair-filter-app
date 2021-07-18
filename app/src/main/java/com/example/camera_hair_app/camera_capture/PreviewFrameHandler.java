package com.example.camera_hair_app.camera_capture;

public interface PreviewFrameHandler {
    void onPreviewFrame(byte[] data, int width, int height);
}
