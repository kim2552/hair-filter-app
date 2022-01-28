package com.example.camera_hair_app.camera_activity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import androidx.annotation.Nullable;
import androidx.fragment.app.FragmentActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.example.camera_hair_app.R;

import java.lang.reflect.Array;
import java.util.ArrayList;

public class SelectHairActivity extends FragmentActivity {

    private static final String TAG = "SelectHairActivity";

    //vars
    private ArrayList<String> mNames = new ArrayList<>();
    private ArrayList<String> mImageUrls = new ArrayList<>();
    private ArrayList<String> mObjectIds = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_select_hair);
        Log.d(TAG, "onCreate: started.");

        initImageBitmaps();
        initRecyclerView();
    }

    private void initImageBitmaps(){
        Log.d(TAG, "initImageBitmaps: preparing bitmaps.");

        mImageUrls.add("https://media.istockphoto.com/photos/making-hairstory-everyday-with-gorgeous-hair-picture-id1055099140?k=20&m=1055099140&s=612x612&w=0&h=XhmNge5-6wiIlGb0ZwsSmPfLwrl0h2S_2UtmmJdFbdw=");
        mNames.add("Short Ponytail");
        mObjectIds.add("0");

        mImageUrls.add("https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcQ-UU_WPRDxnSyVCfGKGJNmLdiVN6-4n8YGOw&usqp=CAU");
        mNames.add("Long with bangs");
        mObjectIds.add("1");

        mImageUrls.add("https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcQ-UU_WPRDxnSyVCfGKGJNmLdiVN6-4n8YGOw&usqp=CAU");
        mNames.add("Long and wavy with a bun");
        mObjectIds.add("2");

        mImageUrls.add("https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcQ-UU_WPRDxnSyVCfGKGJNmLdiVN6-4n8YGOw&usqp=CAU");
        mNames.add("Long and wavy");
        mObjectIds.add("3");

        mImageUrls.add("https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcQ-UU_WPRDxnSyVCfGKGJNmLdiVN6-4n8YGOw&usqp=CAU");
        mNames.add("Long and wavy with bangs");
        mObjectIds.add("4");

        mImageUrls.add("https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcQ-UU_WPRDxnSyVCfGKGJNmLdiVN6-4n8YGOw&usqp=CAU");
        mNames.add("Medium length with bangs");
        mObjectIds.add("5");

        mImageUrls.add("https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcQ-UU_WPRDxnSyVCfGKGJNmLdiVN6-4n8YGOw&usqp=CAU");
        mNames.add("Long with bangs");
        mObjectIds.add("6");

        mImageUrls.add("https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcQ-UU_WPRDxnSyVCfGKGJNmLdiVN6-4n8YGOw&usqp=CAU");
        mNames.add("Short with bangs");
        mObjectIds.add("7");
    }

    private void initRecyclerView(){
        Log.d(TAG, "initRecyclerView: init recyclerview.");
        RecyclerView recyclerView = findViewById(R.id.recyclerview);
        RecyclerViewAdapter adapter = new RecyclerViewAdapter(this, mNames, mImageUrls, mObjectIds, new RecyclerViewAdapter.OnClickListener() {
            @Override
            public void onClick(String item) {
                Intent intent = new Intent();
                intent.putExtra(Intent.EXTRA_TEXT, item);
                setResult(RESULT_OK, intent);
                finish();
            }
        });
        recyclerView.setAdapter(adapter);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
    }
}
