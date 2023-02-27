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