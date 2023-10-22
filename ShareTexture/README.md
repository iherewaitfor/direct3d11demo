# Shared Texture 共享纹理
本例主要是介绍共享纹理的使用。在CreateSahredTexture项目中创建共享纹理，在UseShareTexture中使用共享纹理。
# CreateSahredTexture创建共享
本项目主要是创建共享纹理，并把更新共享纹理。同时在本进程中，也读取该共享纹理用于窗口渲染。
创建纹理，使用CreateTexture2D，其中D3D11_TEXTURE2D_DESC的关键参数如下描述。
```C++
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
```
# Use ShareTexture使用共享纹理
在另外一个进程中读取共享纹理。
使用OpenSharedResource打开共享纹理。使用CreateShaderResourceView创建视频给显卡用。
```C++
	result = device->OpenSharedResource(sharedHandle, __uuidof(ID3D11Texture2D), (LPVOID*)&g_sharedTexture);
    if (FAILED(result))
    {
        return false;
    }
    g_sharedTexture->GetDesc(&textureDesc);
    
    // 创建shader资源，和纹理关联起来
    shaderResourceViewDesc.Format = textureDesc.Format;
    //shaderResourceViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    result = device->CreateShaderResourceView(g_sharedTexture, &shaderResourceViewDesc, &g_shaderResourceView);
    if (FAILED(result))
    {
        return false;
    }
```