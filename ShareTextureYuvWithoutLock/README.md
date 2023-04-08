# 使用共享纹理不加锁
CreateShareTexture项目创建了YUV共享纹理（不加锁）并渲染到窗口上，创建共享纹理后，把共享句柄写到共享内存中。UseShareTexture通共享内存读出共享纹理句柄，然后创建ResourceView，把纹理渲染到窗口上。

需要先运行CreateShareTexture，后运行UseShareTexture。

# How to run
先安装cmake，进入源码目录新建build目录。然后在build 目录中进入命令行。然后执行命令
```
cmake .. -G "Visual Studio 17 2022" -A Win32
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

## 创建共享纹理
创建纹理的一些关键参数，其中MiscFlags使用D3D11_RESOURCE_MISC_SHARED，不加锁。
```C++
    textureDesc.Format = DXGI_FORMAT_R8_UNORM;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
```
创建纹理的代码
```C++
    const int textureWidth = g_videoWidth;  //640
    const int textureHeight = g_videoHeight;//360
    D3D11_TEXTURE2D_DESC textureDesc;
    HRESULT result;
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

    //创建2d纹理，
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    textureDesc.Width = textureWidth;      //单一纹理的宽度与视频宽度相同
    textureDesc.Height = textureHeight*3/2;//单一纹理的高度（w*h+1/2*w*1/2*h+1/2*w*1/2h)/2=3/2h
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8_UNORM;

    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

    result = g_pd3dDevice->CreateTexture2D(&textureDesc, NULL, &g_texturePlanes_[0]);//YUV单一纹理
    if (FAILED(result))
    {
        return false;
    }
```