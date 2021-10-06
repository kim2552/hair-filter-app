package com.example.camera_hair_app.camera_activity;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;

import com.example.camera_hair_app.R;
import com.example.camera_hair_app.camera_render.GLVideoRenderer;
import com.example.camera_hair_app.libs.AspectFrameLayout;

import java.io.FileOutputStream;
import java.io.IOException;

public class GLViewActivity extends BaseViewActivity implements ActivityCompat.OnRequestPermissionsResultCallback {

    private static final String TAG = GLViewActivity.class.toString();
    private static final int REQUEST_CAMERA_PERMISSION = 1;
    private static final String[] CAMERA_PERMISSIONS = {
            Manifest.permission.CAMERA
    };

    private GLVideoRenderer mVideoRenderer;
    private int mFilter = 0;

    private Context appContext;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG,"onCreate");
        setContentView(R.layout.activity_glview);

        appContext = this.getApplicationContext();

        GLSurfaceView glSurfaceView = findViewById(R.id.gl_surface_view);
        mVideoRenderer = new GLVideoRenderer();
        mVideoRenderer.init(glSurfaceView, appContext);

        AspectFrameLayout layout = (AspectFrameLayout) findViewById(R.id.preview);
        layout.setAspectRatio((double) 3/4);
        layout.addView(mPreview);

        Button btnCaptureCamera = (Button) findViewById(R.id.capture_camera);
        btnCaptureCamera.setOnClickListener(v -> {
            try {
                // Get bitmap
                Bitmap snapshot = mVideoRenderer.getSnapshotBitmap();

                // Write file
                String filename = "bitmap.png";
                FileOutputStream stream = this.openFileOutput(filename, Context.MODE_PRIVATE);
                snapshot.compress(Bitmap.CompressFormat.PNG, 100, stream);

                // Cleanup
                stream.close();
                snapshot.recycle();

                // Pop intent
                Intent intent = new Intent(this, ImagePreviewActivity.class);
                intent.putExtra("image", filename);
                startActivity(intent);
            } catch (InterruptedException | IOException e) {
                e.printStackTrace();
            }
        });

        Button btnFlipCamera = (Button) findViewById(R.id.flip_camera);
        btnFlipCamera.setOnClickListener(v -> {
            mPreview.setCameraFacing(displayMetrics.widthPixels, displayMetrics.heightPixels);
        });

        Button btnFilterLeft = (Button) findViewById(R.id.filter_left);
        btnFilterLeft.setOnClickListener(v -> {
            mParams = mVideoRenderer.getVideoParamaters();
            mParams -= 1;
            if(mParams < 0){
                mParams = 0;
            }
            mVideoRenderer.setVideoParameters(mParams, appContext);
        });

        Button btnFilterRight = (Button) findViewById(R.id.filter_right);
        btnFilterRight.setOnClickListener(v -> {
            mParams = mVideoRenderer.getVideoParamaters();
            mParams += 1;
            if(mParams > 1){
                mParams = 1;
            }
            mVideoRenderer.setVideoParameters(mParams, appContext);
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mVideoRenderer.destroyRender();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if(!hasPermissionsGranted(CAMERA_PERMISSIONS)){
            requestCameraPermission();
        }else{
            mPreview.startCamera();
        }
    }

    @Override
    protected void onPause() {
        if(hasPermissionsGranted(CAMERA_PERMISSIONS)){
            mPreview.stopCamera();
        }
        super.onPause();
    }

    @Override
    public void onPreviewFrame(byte[] data, int width, int height, int camera_facing) {
        mVideoRenderer.drawVideoFrame(data, width, height, getOrientation(), camera_facing);
        mVideoRenderer.requestRender();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if(requestCode == REQUEST_CAMERA_PERMISSION){
            if(grantResults.length == CAMERA_PERMISSIONS.length){
                for(int result : grantResults){
                    if(result != PackageManager.PERMISSION_GRANTED){
                        //TODO::Create error dialog stating permission required
                    }else{
                        mPreview.startCamera();
                    }
                }
            }
        }else{
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }
    }

    /**
     * Checks to see see if the given permissions have been granted
     * @param permissions
     * @return
     */
    private boolean hasPermissionsGranted(String[] permissions){
        for(String permission : permissions){
            if(ActivityCompat.checkSelfPermission(this, permission) != PackageManager.PERMISSION_GRANTED){
                return false;
            }
        }
        return true;
    }

    private void requestCameraPermission(){
        if(shouldShowRequestPermissionRationale(Manifest.permission.CAMERA)){
            //TODO::Create confirmation dialog for permissions
        }else{
            requestPermissions(CAMERA_PERMISSIONS, REQUEST_CAMERA_PERMISSION);
        }
    }

}
