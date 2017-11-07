package com.shz.gifapplication;

import android.graphics.Bitmap;
import android.util.Log;

/**
 * Created by shz on 2017/11/7.
 */

public class GifHandler {
    private static final String TAG = "GifHandler";
    //代表C层的地址；
    private long gifPoint;

    public long getGifPoint() {
        return gifPoint;
    }

    public GifHandler(long gifPoint) {
        this.gifPoint = gifPoint;
    }

    // Used to load the 'native-lib' library on application startup.
    static {
        try {
            System.loadLibrary("native-lib");
        } catch (Exception e) {
            Log.e(TAG, "static initializer: "+e.toString());


        }
    }

    //    public static native int getWidth(gifPoint);
    public static native long loadGif(byte[] path);

    public static native int getWidth(long gifHandler);

    public static native int getHeight(long gifHandler);

    public static native int getNextTime(long gifHandler);

    public static native int updateFrame(long gifHandler, Bitmap bitmap);

    public static GifHandler load(String path) {
        long gifHander = loadGif(path.getBytes());
        GifHandler gifHandler = new GifHandler(gifHander);
        return gifHandler;
    }
}
