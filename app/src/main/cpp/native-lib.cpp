#include <jni.h>
#include <string>
#include <stdlib.h>
#include <android/bitmap.h>
#include "dgif_lib.h"
#include "gif_lib.h"

extern "C" {

typedef struct GifBean {
//有多少帧
    int total_frame;
//当前帧
    int current_frame;
//时间集合 各帧播放时间不一样
    int frame_duration;//todo
    int *delays;
    int total_time;//todo
};
GifFileType *giffiletype=(GifFileType *)giffletype;
GifBean *gifbean=(GifBean) GifFileType.userdata
AndroidBitmapInfo INFO;
AndroidBitmap_getInfo();//env.bitmap &info
//锁定bitmap
void *pixels;
JNIEXPORT void JNICALL
Java_com_example_xiaoke_gifplayer_GifHandler_drawFrame(JNIEnv *env, jclass type,GifFileType *giffletype,GifFileType *info,int  *pixels, int current_frame){

//解锁画布
//AndroidBitmap_lockPixels(env,bitmap,&pixels);//env.bitmap,&pixels
//索引+1
gifbean->current_frame+=1;
if(gifbean->current_frame >= gifbean->total_frame-1)
    gifbean->current_frame =0;
//return gifbean->delays[gifbean->current_frame]; //返回下一帧时间 todo
    GifByteType gifByteType ;
    int *px = (int *) pixels; //整幅图片的首地址
    int *line;//每一行的首地址
    int pointpixels;//在saveimage里面的索引rasterbits
    ColorMapObject *colorMapObject  ;  //根据颜色列表压缩
//拿到当前帧信息
    SavedImages savedImages  = giffiletype->SavedImages[gifbean->current_frame];//todo
GifImageDesc frameinfo= savedImages.imagedesc;
// x y 偏移量,图像宽度 高度
//info.stride;//每一行top列
        //遍历像素数组信息rastebits  绘制bitmap
for (int y=frameinfo.Top;y<frameinfo.Top+frameinfo.Height;++y){ //遍历行
        line=px;

//遍历列
for (int x=frameinfo.Left;x<frameinfo.Left+frameinfo.Width;++x){
//具体一个像素的索引
    pointpixels=(y-frameinfo.Top)*frameinfo.Width+x-frameinfo.Left;
    GifByteType gifByteType = savedImages.rastebits[pointpixels]; //得到的是压缩数据
   //解压缩
    GifColorType gifColorType  = colorMapObject->color[gifByteType];
    //压缩的argb解压成RGB
        line[x]=Argb(255,gifColorType.Red ,gifColorType.Green,gifColorType.Blue);
}
px=(int *) (char*)px+info.stride;//换行

}

}
/**
 * 调用c展示gif动态图片
 */
JNIEXPORT jint JNICALL
Java_com_example_xiaoke_gifplayer_GifHandler_getWidth(JNIEnv *env, jclass type, jlong gifHandler) {
    GifFileType *gifFileType = (GifFileType *) gifHandler;
    return gifFileType->SWidth;

}
JNIEXPORT jint JNICALL
Java_com_example_xiaoke_gifplayer_GifHandler_getHeight(JNIEnv *env, jclass type, jlong gifHandler) {

    GifFileType *gifFileType = (GifFileType *) gifHandler;
    return gifFileType->SHeight;

}

JNIEXPORT jint JNICALL
Java_com_example_xiaoke_gifplayer_GifHandler_getNextTime(JNIEnv *env, jclass type,
                                                         jlong gifHandler) {
    GifFileType *gifFileType = (GifFileType *) gifHandler;
    GifBean *gifBean = (GifBean *) gifFileType->UserData;
    return gifBean->frame_duration;//todo
}

JNIEXPORT jint  JNICALL
Java_com_example_xiaoke_gifplayer_GifHandler_updateFrame(JNIEnv *env, jclass type, jlong gifHandler,
                                                         jobject bitmap) {
    //强转代表gif的结构体
    GifFileType *gifFileType = (GifFileType *) gifHandler;
    GifBean *gifBean = (GifBean *) gifFileType->UserData;
    AndroidBitmapInfo info;
    //代表一副图片的像素数组
    void *pixels;
    //info赋值
    AndroidBitmap_getInfo(env, bitmap, &info);
    //锁定bitmap，一副图片是二维数组
    AndroidBitmap_lockPixels(env, bitmap, &pixels);
    //绘制一帧图片
    drawFrame(gifFileType, &info, (int *) pixels, gifBean->current_frame, false);
    //当前帧+1；
    gifBean->current_frame += 1;
    //当绘制到最后一帧
    if (gifBean->current_frame >= gifBean->total_frame) {
        gifBean->current_frame = 0;
    }
    AndroidBitmap_unlockPixels(env, bitmap);
    return gifBean->frame_duration;
}

//jstring to char*
char *jstringTostring(JNIEnv *env, jbyteArray barr) {

    char *rtn = NULL;
//    if ((env)->ExceptionCheck()) {
//        return rtn;
//    }
//    jclass clsstring = env->FindClass("Java/lang/String");
//    jstring strencode = env->NewStringUTF("utf-8");
//
//    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
//    jbyteArray barr= (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}

JNIEXPORT jlong JNICALL
Java_com_example_xiaoke_gifplayer_GifHandler_loadGif(JNIEnv *env, jclass type, jbyteArray path_) {
    char *filePath = jstringTostring(env, path_);//javastring转换成char*,文件路径
    int err;
    GifFileType *gifFileType = DGifOpenFileName(filePath, &err);//调用源码api里方法，打开gif，返回GifFileType实体
    //初始化结构体
    DGifSlurp(gifFileType);
    //实例化bean;
    GifBean *gifBean = (GifBean *) malloc((sizeof(GifBean)));
    gifBean->frame_duration = 0;
    gifBean->current_frame = 0;
    gifBean->total_frame = 0;
    gifBean->total_time = 0;
    //好比一个view打一个tag
    gifFileType->UserData = gifBean;
    //下一步，给gifbean成员变量赋值，得到当前播放时间的总时长；
    int i, j, frame_delay;
    //一帧图片
    SavedImage *frame;
    ExtensionBlock *ext;
    for (i = 0; i < gifFileType->ImageCount; i++) {
        frame = &gifFileType->SavedImages[i];
        for (j = 0; j < frame->ExtensionBlockCount; j++) {
            //找到含有延迟时间的代码块
            if (frame->ExtensionBlocks[j].Function == GRAPHICS_EXT_FUNC_CODE) {
                ext = &(frame->ExtensionBlocks[j]);
            }
        }
        if (ext) {
            //延迟时间1-->10ms
            frame_delay = 10 * (ext->Bytes[2] << 8 | ext->Bytes[1]);
            //得到gif总时间
            gifBean->total_time += frame_delay;
        }
    }
    //每一帧时间
    gifBean->frame_duration = gifBean->total_time / gifFileType->ImageCount;
    //总帧数
    gifBean->total_frame = gifFileType->ImageCount;
    return (long long) gifFileType;
}
}