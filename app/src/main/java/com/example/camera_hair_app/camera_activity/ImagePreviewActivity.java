package com.example.camera_hair_app.camera_activity;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.PersistableBundle;
import android.util.Log;
import android.view.TextureView;
import android.widget.Button;
import android.widget.ImageView;

import androidx.annotation.Nullable;
import androidx.fragment.app.FragmentActivity;

import com.example.camera_hair_app.R;

import java.io.FileInputStream;

public class ImagePreviewActivity extends FragmentActivity {

    private static final String TAG = ImagePreviewActivity.class.toString();

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
    }
}
