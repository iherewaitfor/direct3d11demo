- [ShareTextureRGBAFFmpeg 使用YUV共享纹理不加锁，支持FFmpeg解码](#sharetexturergbaffmpeg-使用yuv共享纹理不加锁支持ffmpeg解码)
- [mp4文件播放支持缩放](#mp4文件播放支持缩放)
- [CreateShareTexture支持从共享内存读取视频帧渲染](#createsharetexture支持从共享内存读取视频帧渲染)

# ShareTextureRGBAFFmpeg 使用YUV共享纹理不加锁，支持FFmpeg解码
使用RGBA格式共享纹理不加锁，支持FFmpeg解码.

逻辑与ShareTextureYUVFFmpeg项目类似。

编译和运行，请参考[https://github.com/iherewaitfor/direct3d11demo/tree/main/ShareTextureYUVFFmpeg](https://github.com/iherewaitfor/direct3d11demo/tree/main/ShareTextureYUVFFmpeg)

# mp4文件播放支持缩放
若不带大小参数，则直接用原视频大小(如640x420)
比如
```
D:\srccode\direct3d11demo\ShareTextureRGBAFFmpeg\CreateShareTexture\build\Debug>Demo.exe D:\guilinvideo.mp4
```
若带上宽高参考，则会将视频拉伸到对应大小,比如800x600、200x100等
```
D:\srccode\direct3d11demo\ShareTextureRGBAFFmpeg\CreateShareTexture\build\Debug>Demo.exe D:\guilinvideo.mp4 800 600
```

```
D:\srccode\direct3d11demo\ShareTextureRGBAFFmpeg\CreateShareTexture\build\Debug>Demo.exe D:\guilinvideo.mp4 200 100
```

# CreateShareTexture支持从共享内存读取视频帧渲染

视频数据源除了支持rgba文件、mp4文件外，还支持直接从共享内存读取。

要求共享内存名字带"ShareMem"字符：

比如共享内存的名字为myShareMemRGBA456，宽度为 640x360。
```C++
Demo.exe myShareMemRGBA456 640 360
```