package com.shz.gifapplication;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.widget.ImageView;

import java.io.File;
/**
 * Created by shz on 2017/11/7.
 */
public class MainActivity
        extends AppCompatActivity
{

    ImageView mImageView;
    Bitmap bitmap;
    GifHandler gifHandler;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mImageView = (ImageView) findViewById(R.id.gifImageView);

        File file =new File(Environment.getExternalStorageDirectory(), "demo.gif");
        gifHandler =GifHandler.load(file.getAbsolutePath());
        //得到gif的width,height,生成Bitmap;
        int width=gifHandler.getWidth(gifHandler.getGifPoint());
        int height=gifHandler.getHeight(gifHandler.getGifPoint());
        bitmap= Bitmap.createBitmap(width,height, Bitmap.Config.ARGB_8888);
        int nextTime=  gifHandler.updateFrame(gifHandler.getGifPoint(),bitmap);
        handler.sendEmptyMessageDelayed(1,nextTime);
    }



    private Handler handler=new Handler(){
        @Override
        public void handleMessage(Message msg) {
            int nextTime=gifHandler.updateFrame(gifHandler.getGifPoint(),bitmap);
            handler.sendEmptyMessageDelayed(1,nextTime);
            mImageView.setImageBitmap(bitmap);
        }
    };
}
