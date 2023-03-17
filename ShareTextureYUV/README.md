# ShareTextureYUV。YUV共享纹理
CreateShareTexture项目创建了YUV共享纹理并渲染到窗口上，创建共享纹理后，把共享句柄写到共享内存中。UseShareTexture通共享内存读出共享纹理句柄，然后创建ResourceView，把纹理渲染到窗口上。

需要先运行CreateShareTexture，后运行UseShareTexture。

# How to run
先安装cmake，进入源码目录新建build目录。然后在build 目录中进入命令行。然后执行命令
```
cmake .. -G "Visual Studio 17 2022" -A Win32
```
。然后打开生成的sln文件，将Demo项目设置为启动项即可。 cmake使用可参考本github项目[cmakevisualstudio](https://github.com/iherewaitfor/cmakevisualstudio)

# 创建共享纹理
在另外一个进程中读取共享纹理。其中特别注意以下参数。
```C++
    textureDesc.Format = DXGI_FORMAT_R8_UNORM;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
```
其中DXGI_FORMAT_R8_UNORM,表示使用一8位的格式。还有YUV420数据的大小需要注意。按YUV存放。
```C++
    const int textureWidth = 640;
    const int textureHeight = 360;
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
    //textureDesc.MiscFlags = 0;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

    result = g_pd3dDevice->CreateTexture2D(&textureDesc, NULL, &g_texturePlanes_[0]);//YUV单一纹理
```
创建完纹理后，使用g_pd3dDevice->CreateShaderResourceView，创建shader资源，和纹理关联起来。

## 取共享句柄
通过IDXGIResource的GetSharedHandle方法获取共享句柄。然后写到共享内存。
```C++
    HANDLE	g_hsharedHandle = NULL;
  // QI IDXGIResource interface to synchronized shared surface.
    IDXGIResource* pDXGIResource = NULL;
    g_texturePlanes_[0]->QueryInterface(__uuidof(IDXGIResource), (LPVOID*)&pDXGIResource);

    // obtain handle to IDXGIResource object.
    pDXGIResource->GetSharedHandle(&g_hsharedHandle);
```

## IDXGIKeyedMutex
先通过以下接口获取IDXGIKeyedMutex指针。后续更新纹理需要需要使用。
```C++
  // QI IDXGIKeyedMutex interface of synchronized shared surface's resource handle.
    result = g_texturePlanes_[0]->QueryInterface(__uuidof(IDXGIKeyedMutex),
        (LPVOID*)&g_pDXGIKeyedMutex);
```

将获取到的yuv数据更新到纹理。

```C++
    //更新单一纹理数据。
    g_pDXGIKeyedMutex->AcquireSync(0, INFINITE);
    g_pImmediateContext->UpdateSubresource(g_texturePlanes_[0], 0, NULL, buf, Width, 0);
    g_pDXGIKeyedMutex->ReleaseSync(0);
```

## 使用共享纹理
先从共享内存读出共享纹理句柄。然后使用device的OpenSharedResource方法打开共享纹理。

```C++
ID3D11Texture2D*                    g_sharedTexture = NULL; //共享纹理

device->OpenSharedResource(sharedHandle, __uuidof(ID3D11Texture2D), (LPVOID*)&g_sharedTexture);
```

打开共享纹理后，接着创建shader资源。

```C++
    // 创建shader资源，和纹理关联起来
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    result = device->CreateShaderResourceView(g_sharedTexture, &shaderResourceViewDesc, &g_shaderResourceView);
```

可以通过纹理的GetDesc方法获取纹理的描述信息，从中取到纹理的宽高，传到着色器中。
```C++
    D3D11_TEXTURE2D_DESC textureDesc2;
    g_sharedTexture->GetDesc(&textureDesc2);

    bd.ByteWidth = sizeof(CBTextDesc);
    CBTextDesc td2;
    td2.videoWidth = textureDesc2.Width;//640
    td2.videoHeight = textureDesc2.Height;//320
    InitData.pSysMem = &td2;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pCBTextDesc);

    g_pImmediateContext->PSSetConstantBuffers( 3, 1, &g_pCBTextDesc);
```


