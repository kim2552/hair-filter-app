package com.example.camera_hair_app.camera_activity;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Environment;
import android.os.PersistableBundle;
import android.util.Log;
import android.view.TextureView;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.core.app.ActivityCompat;
import androidx.fragment.app.FragmentActivity;

import com.example.camera_hair_app.R;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;

public class ImagePreviewActivity extends FragmentActivity {

    private static final String TAG = ImagePreviewActivity.class.toString();
    private static final int REQUEST_STORAGE_PERMISSION = 1;
    private static final String[] STORAGE_PERMISSIONS = {
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.i(TAG,"onCreate");
        setContentView(R.layout.activity_image_preview);

        ImageView imageView = findViewById(R.id.previewImageView);

        Bitmap bmp = null;
        String filename = getIntent().getStringExtra("image");
        try {
            FileInputStream is = this.openFileInput(filename);
            bmp = BitmapFactory.decodeStream(is);
            is.close();
        } catch (Exception e) {
            e.printStackTrace();
        }

        imageView.setImageBitmap(bmp);

        Button btnBack = (Button) findViewById(R.id.previewBackButton);
        btnBack.setOnClickListener(v -> {
            Intent intent = new Intent(this, GLViewActivity.class);
            startActivity(intent);
        });

        Button btnSave = (Button) findViewById(R.id.previewSaveButton);
        Bitmap finalBmp = bmp;
        btnSave.setOnClickListener(v -> {
            if(!hasPermissionsGranted(STORAGE_PERMISSIONS)){
                requestCameraPermission();
            }
            storeImage(finalBmp);
        });
    }

    private void storeImage(Bitmap image) {
        if(!hasPermissionsGranted(STORAGE_PERMISSIONS)){
            return;
        }
        File pictureFile = getOutputMediaFile();
        if (pictureFile == null) {
            Log.d(TAG,
                    "Error creating media file, check storage permissions: ");// e.getMessage());
            return;
        }
        try {
            FileOutputStream fos = new FileOutputStream(pictureFile);
            image.compress(Bitmap.CompressFormat.JPEG, 90, fos);
            fos.close();

            Context context = getApplicationContext();
            CharSequence text = "Image saved: " + pictureFile.getAbsolutePath();
            int duration = Toast.LENGTH_SHORT;

            Toast toast = Toast.makeText(context, text, duration);
            toast.show();

        } catch (FileNotFoundException e) {
            Log.d(TAG, "File not found: " + e.getMessage());
        } catch (IOException e) {
            Log.d(TAG, "Error accessing file: " + e.getMessage());
        }
    }

    /** Create a File for saving an image or video */
    private  File getOutputMediaFile(){
        // To be safe, you should check that the SDCard is mounted
        // using Environment.getExternalStorageState() before doing this.
        File mediaStorageDir = Environment.getExternalStoragePublicDirectory(
                Environment.DIRECTORY_PICTURES + "/" + getString(R.string.app_name));

        // This location works best if you want the created images to be shared
        // between applications and persist after your app has been uninstalled.

        // Create the storage directory if it does not exist
        if (! mediaStorageDir.exists()){
            if (! mediaStorageDir.mkdirs()){
                return null;
            }
        }
        // Create a media file name
        String timeStamp = new SimpleDateFormat("ddMMyyyy_HHmm").format(new Date());
        File mediaFile;
        String mImageName="MI_"+ timeStamp +".jpg";
        mediaFile = new File(mediaStorageDir.getPath() + File.separator + mImageName);
        return mediaFile;
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
            requestPermissions(STORAGE_PERMISSIONS, REQUEST_STORAGE_PERMISSION);
        }
    }
}
