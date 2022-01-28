package com.example.camera_hair_app.camera_activity;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;

import com.example.camera_hair_app.R;
import com.example.camera_hair_app.camera_render.GLVideoRenderer;
import com.example.camera_hair_app.libs.AspectFrameLayout;

import java.io.FileOutputStream;
import java.io.IOException;
import yuku.ambilwarna.AmbilWarnaDialog;

public class GLViewActivity extends BaseViewActivity implements ActivityCompat.OnRequestPermissionsResultCallback {

    private static final String TAG = GLViewActivity.class.toString();
    private static final int REQUEST_CAMERA_PERMISSION = 1;
    private static final String[] CAMERA_PERMISSIONS = {
            Manifest.permission.CAMERA
    };

    private GLVideoRenderer mVideoRenderer;

    private View mColorPreviewBtn;

    private Context appContext;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG,"onCreate");
        setContentView(R.layout.activity_glview);

        appContext = this.getApplicationContext();

        // set the default color to 0 as it is black
        mDefaultHairColor = 0;
        mParams[0] = "0";
        mParams[1] = "ffffffff";

        GLSurfaceView glSurfaceView = findViewById(R.id.gl_surface_view);
        mVideoRenderer = new GLVideoRenderer();
        mVideoRenderer.init(glSurfaceView, appContext);
        mVideoRenderer.setVideoParameters(mParams, appContext);

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

        Button btnSelectHair = (Button) findViewById(R.id.select_object);
        btnSelectHair.setOnClickListener(v -> {
            try{
                Intent intent = new Intent(this, SelectHairActivity.class);
                startActivityForResult(intent, SELECT_HAIR_ACTIVITY_REQUEST_CODE);
            }catch (Exception e){
                e.printStackTrace();
            }
        });

        mColorPreviewBtn = (View) findViewById(R.id.change_color);
        mColorPreviewBtn.setOnClickListener(v -> {
            openColorPickerDialogue();
            Log.i(TAG,Integer.toString(mDefaultHairColor));
        });
    }

    // This method is called when the second activity finishes
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        // check that it is the SecondActivity with an OK result
        if (requestCode == SELECT_HAIR_ACTIVITY_REQUEST_CODE) {
            if (resultCode == RESULT_OK) {

                // get String data from Intent
                String objectSelection = data.getStringExtra(Intent.EXTRA_TEXT);

                mParams[0] = objectSelection;
                mParams[1] = "ffffffff";                    //white
                mColorPreviewBtn.setBackgroundColor(-1);    //white
                mVideoRenderer.setVideoParameters(mParams, appContext);
            }
        }
    }

    // the dialog functionality is handled separately
    // using openColorPickerDialog this is triggered as
    // soon as the user clicks on the Pick Color button And
    // the AmbilWarnaDialog has 2 methods to be overridden
    // those are onCancel and onOk which handle the "Cancel"
    // and "OK" button of color picker dialog
    public void openColorPickerDialogue() {

        // the AmbilWarnaDialog callback needs 3 parameters
        // one is the context, second is default color,
        final AmbilWarnaDialog colorPickerDialogue = new AmbilWarnaDialog(this, mDefaultHairColor,
                new AmbilWarnaDialog.OnAmbilWarnaListener() {
                    @Override
                    public void onCancel(AmbilWarnaDialog dialog) {
                        // leave this function body as
                        // blank, as the dialog
                        // automatically closes when
                        // clicked on cancel button
                    }

                    @Override
                    public void onOk(AmbilWarnaDialog dialog, int color) {
                        // change the mDefaultColor to
                        // change the GFG text color as
                        // it is returned when the OK
                        // button is clicked from the
                        // color picker dialog
                        mDefaultHairColor = color;
                        String hexstr = Integer.toHexString(mDefaultHairColor);
                        mParams[1] = hexstr;
                        mVideoRenderer.setVideoParameters(mParams, appContext);
                        mColorPreviewBtn.setBackgroundColor(mDefaultHairColor);
                    }
                });
        colorPickerDialogue.show();
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
