package com.example.camera_hair_app.camera_capture;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.ImageFormat;
import android.graphics.PixelFormat;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.ImageReader;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Size;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.core.content.ContextCompat;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

import com.example.camera_hair_app.libs.*;

public class VideoCameraPreview extends SurfaceView implements SurfaceHolder.Callback {
    private static final String TAG = VideoCameraPreview.class.toString();

    private VideoCapture mVideoCapture;
    private Context mContext;
    private CameraCaptureSession mCaptureSession;
    private CameraDevice mCameraDevice;
    private String mCameraId;
    private Integer mSelectedCameraFacing;
    private Handler mBackgroundHandler;
    private HandlerThread mBackgroundThread;
    private ImageReader mImageReader;
    private Semaphore mCameraOpenCloseLock = new Semaphore(1);
    private Integer mSensorOrientation;
    private List<Size> mOutputSizes = new ArrayList<>();
    private Size mPreviewSize;

    public VideoCameraPreview(Context context){
        super(context);

        mSelectedCameraFacing = CameraCharacteristics.LENS_FACING_FRONT;  // Default Front Facing

        mContext = context;
        mVideoCapture = new VideoCapture((PreviewFrameHandler) context);
        getHolder().addCallback(this);
        setVisibility(View.GONE);
    }

    public void init(int w, int h) {
        mPreviewSize = getOptimalPreviewSize(w,h);
    }

    /**
     * This is called immediately after the surface is first created.
     * Implementations of this should start up whatever rendering code
     * they desire.  Note that only one thread can ever draw into
     * a {@link Surface}, so you should not draw into the Surface here
     * if your normal rendering will be in another thread.
     *
     * @param holder The SurfaceHolder whose surface is being created.
     */
    public void surfaceCreated(SurfaceHolder holder){
        Log.i(TAG,"surfaceCreated");
    }

    /**
     * This is called immediately before a surface is being destroyed. After
     * returning from this call, you should no longer try to access this
     * surface.  If you have a rendering thread that directly accesses
     * the surface, you must ensure that thread is no longer touching the
     * Surface before returning from this function.
     *
     * @param holder The SurfaceHolder whose surface is being destroyed.
     */
    public void surfaceDestroyed(SurfaceHolder holder){

    }

    /**
     * This is called immediately after any structural changes (format or
     * size) have been made to the surface.  You should at this point update
     * the imagery in the surface.  This method is always called at least
     * once, after {@link #surfaceCreated}.
     *
     * @param holder The SurfaceHolder whose surface has changed.
     * @param format The new {@link PixelFormat} of the surface.
     * @param w The new width of the surface.
     * @param h The new height of the surface.
     */
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h){
        if(null == mImageReader && null != mPreviewSize) {
            mImageReader = ImageReader.newInstance(mPreviewSize.getWidth(), mPreviewSize.getHeight(), ImageFormat.YUV_420_888, 2);
            mImageReader.setOnImageAvailableListener(mVideoCapture, mBackgroundHandler);
            openCamera(mCameraId);
        }
    }

    public Integer getSensorOrientation() { return mSensorOrientation; }

    public List<Size> getOutputSizes() { return mOutputSizes; }

    public void startCamera(){
        startBackgroundThread();
        setVisibility(View.VISIBLE);
    }

    public void stopCamera(){
        mImageReader.setOnImageAvailableListener(null,null);
        closeCamera();
        stopBackgroundThread();
        setVisibility(View.GONE);
    }

    public void changeSize(Size size){
        mPreviewSize = size;

        stopCamera();
        startCamera();
    }

    /**
     * Opens the camera specified by {@link VideoCameraPreview#}.
     */
    public void openCamera(String cameraId){
        if(ContextCompat.checkSelfPermission(mContext, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED){
            return;
        }

        Log.i(TAG,"openCamera");

        CameraManager manager = (CameraManager) mContext.getSystemService(Context.CAMERA_SERVICE);
        try{
            if(!mCameraOpenCloseLock.tryAcquire(2500, TimeUnit.MILLISECONDS)){
                throw new RuntimeException("Time out waiting to lock camera opening.");
            }
            manager.openCamera(cameraId, mStateCallback, mBackgroundHandler);
        } catch (CameraAccessException e){
            Log.e(TAG, "Cannot access the camera." + e.toString());
        } catch (InterruptedException e){
            throw new RuntimeException("Interrupted while trying to lock camera opening.", e);
        }
    }

    /**
     * Closes the current {@link CameraDevice}.
     */
    public void closeCamera(){
        try{
            mCameraOpenCloseLock.acquire();
            if(null!=mCaptureSession){
                mCaptureSession.close();
                mCaptureSession = null;
            }
            if(null!=mCameraDevice){
                mCameraDevice.close();
                mCameraDevice = null;
            }
            if(null!=mImageReader){
                mImageReader.close();
                mImageReader = null;
            }
        } catch (InterruptedException e){
            throw new RuntimeException("Interrupted while trying to lock camera closing.", e);
        } finally {
            mCameraOpenCloseLock.release();
        }

        Log.i(TAG, "closeCamera");
    }

    /**
     * Starts a background thread and its {@link Handler}.
     */
    public void startBackgroundThread(){
        mBackgroundThread = new HandlerThread("CameraBackground");
        mBackgroundThread.start();
        mBackgroundHandler = new Handler(mBackgroundThread.getLooper());
    }

    /**
     * Stops the background thread and its {@link Handler}.
     */
    public void stopBackgroundThread(){
        mBackgroundThread.quitSafely();
        try{
            mBackgroundThread.join();
            mBackgroundThread = null;
            mBackgroundHandler = null;
        } catch (InterruptedException e){
            e.printStackTrace();
        }
    }

    /**
     * Creates the CaptureRequest.Builder based on the device specific camera.
     * @return CaptureRequest.Builder
     */
    private CaptureRequest createCaptureRequest(){
        if(null == mCameraDevice) return null;
        try{
            CaptureRequest.Builder builder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_RECORD);
            builder.addTarget(mImageReader.getSurface());
            return builder.build();
        } catch (CameraAccessException e){
            Log.e(TAG, e.getMessage());
            return null;
        }
    }

    /**
     * {@link CameraDevice.StateCallback} is called when {@link CameraDevice} changes its state.
     */
    private final CameraDevice.StateCallback mStateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(@NonNull CameraDevice cameraDevice) {
            mCameraOpenCloseLock.release();
            mCameraDevice = cameraDevice;
            createCaptureSession(); // We start camera preview here.
        }

        @Override
        public void onDisconnected(@NonNull CameraDevice cameraDevice) {
            mCameraOpenCloseLock.release();
            cameraDevice.close();
            mCameraDevice = null;
        }

        @Override
        public void onError(@NonNull CameraDevice cameraDevice, int error) {
            mCameraOpenCloseLock.release();
            cameraDevice.close();
            mCameraDevice = null;
        }
    };

    private void createCaptureSession() {
        try{
            if(null == mCameraDevice || null == mImageReader) return;
            mCameraDevice.createCaptureSession(Collections.singletonList(mImageReader.getSurface()), sessionStateCallback, mBackgroundHandler); //TODO::Replace deprecated functionality
        } catch (CameraAccessException e){
            Log.e(TAG, "createCaptureSession "+e.toString());
        }
    }

    private CameraCaptureSession.StateCallback sessionStateCallback = new CameraCaptureSession.StateCallback() {
        @Override
        public void onConfigured(@NonNull CameraCaptureSession session) {
            mCaptureSession = session;
            try{
                CaptureRequest captureRequest = createCaptureRequest();
                if(captureRequest != null){
                    session.setRepeatingRequest(captureRequest, null, mBackgroundHandler);
                }else{
                    Log.e(TAG, "captureRequest is null");
                }
            } catch (CameraAccessException e){
                Log.e(TAG, "onConfigured "+ e.toString());
            }
        }

        @Override
        public void onConfigureFailed(@NonNull CameraCaptureSession session) {
            Log.e(TAG, "onConfigureFailed");
        }
    };

    public Size getOptimalPreviewSize(int w, int h){
        CameraManager manager = (CameraManager) mContext.getSystemService(Context.CAMERA_SERVICE);
        try{
            for (String cameraId : manager.getCameraIdList()){
                CameraCharacteristics characteristics = manager.getCameraCharacteristics(cameraId);

                Integer facing = characteristics.get(CameraCharacteristics.LENS_FACING);

                if(mSelectedCameraFacing == facing) {
                    StreamConfigurationMap streamConfigs = characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
                    if (streamConfigs != null) {
                        mOutputSizes = Arrays.asList(streamConfigs.getOutputSizes(SurfaceTexture.class));
                    }
                    mSensorOrientation = characteristics.get(CameraCharacteristics.SENSOR_ORIENTATION);

                    mCameraId = cameraId;
                }
            }
        } catch (CameraAccessException e){
            Log.e(TAG, "Cannot access the camera." + e.toString());
        }

        // Use a very small tolerance because we want an exact match.
        final double ASPECT_TOLERANCE = 0.1;
        double targetRatio = (double) 4 / 3;    //We always want aspect ratio of 4:3    //(double) w / h;
        if (mOutputSizes == null)
            return null;

        Size optimalSize = null;

        // Start with max value and refine as we iterate over available preview sizes. This is the
        // minimum difference between view and camera height.
        double minDiff = Double.MAX_VALUE;

        // Try to find a preview size that matches aspect ratio and the target view size.
        // Iterate over all available sizes and pick the largest size that can fit in the view and
        // still maintain the aspect ratio.
        for (Size size : mOutputSizes) {
            double ratio = (double) size.getWidth() / size.getHeight();
            if (Math.abs(ratio - targetRatio) > ASPECT_TOLERANCE)
                continue;
            if (Math.abs(size.getHeight() - h) < minDiff) {
                optimalSize = size;
                minDiff = Math.abs(size.getHeight() - h);
            }
        }

        // Cannot find preview size that matches the aspect ratio, ignore the requirement
        if (optimalSize == null) {
            minDiff = Double.MAX_VALUE;
            for (Size size : mOutputSizes) {
                if (Math.abs(size.getHeight() - h) < minDiff) {
                    optimalSize = size;
                    minDiff = Math.abs(size.getHeight() - h);
                }
            }
        }

        return optimalSize;
    }
}
