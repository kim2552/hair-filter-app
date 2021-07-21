package com.example.camera_hair_app.camera_activity;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.SparseIntArray;
import android.view.Surface;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.fragment.app.FragmentActivity;
import androidx.recyclerview.widget.RecyclerView;

import com.example.camera_hair_app.camera_capture.PreviewFrameHandler;
import com.example.camera_hair_app.camera_capture.VideoCameraPreview;

public abstract class BaseViewActivity extends FragmentActivity implements PreviewFrameHandler {

    private static final SparseIntArray ORIENTATIONS = new SparseIntArray();
    protected VideoCameraPreview mPreview;
    protected int mParams;

    static {
        ORIENTATIONS.append(Surface.ROTATION_0, 90);
        ORIENTATIONS.append(Surface.ROTATION_90, 0);
        ORIENTATIONS.append(Surface.ROTATION_180, 270);
        ORIENTATIONS.append(Surface.ROTATION_270, 180);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        DisplayMetrics displayMetrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);  //TODO::Replace deprecated functionality

        mPreview = new VideoCameraPreview(this);

        mPreview.init(displayMetrics.widthPixels, displayMetrics.heightPixels);
    }

    protected int getOrientation(){
        int rotation = getWindowManager().getDefaultDisplay().getRotation(); //TODO::Replace deprecated functionality
        return (ORIENTATIONS.get(rotation) + mPreview.getSensorOrientation() + 270) % 360;
    }
}
