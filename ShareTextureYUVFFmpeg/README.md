- [使用YUV共享纹理不加锁，支持FFmpeg解码](#使用yuv共享纹理不加锁支持ffmpeg解码)
- [How to run](#how-to-run)
  - [CreateShareTexture需要使用64位进行](#createsharetexture需要使用64位进行)
  - [默认如何运行](#默认如何运行)
  - [解码mp4文件运行](#解码mp4文件运行)
  - [读取yuv文件运行](#读取yuv文件运行)

# 使用YUV共享纹理不加锁，支持FFmpeg解码
CreateShareTexture项目创建了YUV共享纹理（不加锁）并渲染到窗口上，创建共享纹理后，把共享句柄写到共享内存中。支持使用FFMpeg直接解码视频文件。UseShareTexture通共享内存读出共享纹理句柄，然后创建ResourceView，把纹理渲染到窗口上。

需要先运行CreateShareTexture，后运行UseShareTexture。

# How to run
先安装cmake，进入源码目录新建build目录。然后在build 目录中进入命令行。然后执行命令
```
cmake .. -G "Visual Studio 17 2022"
```
。然后打开生成的sln文件，将Demo项目设置为启动项即可。 cmake使用可参考本github项目[cmakevisualstudio](https://github.com/iherewaitfor/cmakevisualstudio)

如果你安装的是其他版本的Visual Studio，可以通过以下命令，查看对应的Visual Studio版本。
```
cmake -G help
```

```
 Visual Studio 17 2022        = Generates Visual Studio 2022 project files.
                                 Use -A option to specify architecture.
  Visual Studio 16 2019        = Generates Visual Studio 2019 project files.
                                 Use -A option to specify architecture.
  Visual Studio 15 2017 [arch] = Generates Visual Studio 2017 project files.
                                 Optional [arch] can be "Win64" or "ARM".
  Visual Studio 14 2015 [arch] = Generates Visual Studio 2015 project files.
                                 Optional [arch] can be "Win64" or "ARM".
  Visual Studio 12 2013 [arch] = Generates Visual Studio 2013 project files.
                                 Optional [arch] can be "Win64" or "ARM".
  Visual Studio 11 2012 [arch] = Deprecated.  Generates Visual Studio 2012
                                 project files.  Optional [arch] can be
                                 "Win64" or "ARM".
  Visual Studio 9 2008 [arch]  = Generates Visual Studio 2008 project files
```

比如你安装的是Visual studio 2017，需要构建Win64项目,可以将构建命令改成
```
cmake .. -G "Visual Studio 15 2017 Win64"
```
若需要使用Visual Studio 2017，需要构建win32项目，则可以将构建命令改成
```
cmake .. -G "Visual Studio 15 2017"
```

## CreateShareTexture需要使用64位进行
由于本项目中依赖的FFmpeg库是64位的，没有引入32位的，所以需要生成64位项目进行编译（若你引入的FFmpeg库也有32的，则无此限制）。而UseShareTexture因为没有依赖64位的FFmpeg库，则生成32或64都可以。
## 默认如何运行
默认运行以下命令。默认会读取guilin_640x360_yuv420.yuv文件。该文件只有一帧。
```
Demo.exe
```
## 解码mp4文件运行
可以在程序后，加入mp4文件。如下命令。这时会程序会从mp4文件中读取视频大小，解码mp4文件，并循环解码播放。
```
Demo.exe D:\video.mp4
```
## 读取yuv文件运行
使用以下命令格式 Demo.exe yuv文件路径 视频宽度 视频高度。如以命令所示。
```
Demo.exe video_800x600_yuv420.yuv 800 600
```

其中yuv的文件生成可以使用ffmpeg程序生成。其中-s 640x360表示把视频拉伸到640x360。若不使用-s 640x360，则大小是原视频大小。生成的文件包括视频的所以的视频帧。比如input.mp4是有120帧，则生成的output.yuv文件包括了120帧。
```
ffmpeg -i input.mp4 -s 640x360 -pix_fmt yuv420p output.yuv
```
