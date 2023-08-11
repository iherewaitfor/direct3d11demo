- [RectTexture](#recttexture)
- [创建窗口](#创建窗口)
- [初始化设备](#初始化设备)
  - [初始始化设备](#初始始化设备)
  - [设置渲染目标](#设置渲染目标)
  - [设置Viewport](#设置viewport)
- [设置顶点信息](#设置顶点信息)
  - [设置顶点内存布局](#设置顶点内存布局)
  - [构造顶点数据](#构造顶点数据)
  - [把顶点数据设置到显存](#把顶点数据设置到显存)
  - [设置顶点索引 和三角形拓朴信息](#设置顶点索引-和三角形拓朴信息)
- [矩阵模型构造](#矩阵模型构造)
- [正交投影矩阵](#正交投影矩阵)
  - [参数](#参数)
  - [本例中的实际调用](#本例中的实际调用)

# RectTexture
本项目主要是使用了正交矩阵，进行2D渲染。将纹理渲染到矩形上。

本项目同时展示D3D11的渲染的基础流程。

整个核心概念就是：如何借用 D3D11的接口，申请显卡内存，把系统内存的数据设置到显存中，然后让显卡的逻辑跑起来。

# 创建窗口
核心函数
- CreateWindow

就正常的CreateWindow，如程序中的InitWindow。获取窗口句柄。

# 初始化设备
初始化设备，并创建目标渲染视图，设置viewport。

核心函数：
- D3D11CreateDeviceAndSwapChain

## 初始始化设备
核心函数
- D3D11CreateDeviceAndSwapChain

获取以下3个主要对象
```C++
ID3D11Device*                       g_pd3dDevice = NULL;
ID3D11DeviceContext*                g_pImmediateContext = NULL;
IDXGISwapChain*                     g_pSwapChain = NULL;
```
- 

初始始化逻辑如下。

```C++
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( g_hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = g_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain( NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
                                            D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
        if( SUCCEEDED( hr ) )
            break;
    }
    if( FAILED( hr ) )
        return hr;
```
## 设置渲染目标

核心函数
- ID3D11DeviceContext::OMSetRenderTargets

相关函数：
- IDXGISwapChain::GetBuffer
  - 获取显存Buffer
- ID3D11Device::CreateRenderTargetView
  - 创建渲染目标视图
- ID3D11Device::CreateTexture2D
  - 创建2D纹理
- ID3D11Device::CreateDepthStencilView
  - 深度模具视图

```C++
    // Create a render target view
    ID3D11Texture2D* pBackBuffer = NULL;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory( &descDepth, sizeof(descDepth) );
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D( &descDepth, NULL, &g_pDepthStencil );
    if( FAILED( hr ) )
        return hr;

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
    if( FAILED( hr ) )
        return hr;

    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, g_pDepthStencilView );
```
## 设置Viewport
D3D11需要自己设置Viewport，没默认值。
```C++
    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );
```
# 设置顶点信息

需要先编译顶点着器程序获取ID3DBlob。

- 顶点内存布局
- 顶点组成模型

## 设置顶点内存布局
相关函数
- ID3D11Device::CreateInputLayout
- ID3D11DeviceContext::IASetInputLayout

设置顶点的内存布局，显卡才知道，一个顶点有多大。才知道怎么把设置过去的顶点数据解读出来。

描述顶点单个属性的结构如下
```C++
typedef struct D3D11_INPUT_ELEMENT_DESC
    {
    LPCSTR SemanticName;
    UINT SemanticIndex;
    DXGI_FORMAT Format;
    UINT InputSlot;
    UINT AlignedByteOffset;
    D3D11_INPUT_CLASSIFICATION InputSlotClass;
    UINT InstanceDataStepRate;
    } 	D3D11_INPUT_ELEMENT_DESC;
```
通过数据类型为D3D11_INPUT_ELEMENT_DESC的数组，就可以描述出一个顶点的所有元素了。

如我们在系统内存中定义如下的顶点
```C++
struct SimpleVertex
{
    XMFLOAT3 Pos;
    XMFLOAT2 Tex;
};
```

用D3D11的描述如下。并通过接口CreateInputLayout和IASetInputLayout把设置到显卡中。
```C++
    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE( layout );

    // Create the input layout
    hr = g_pd3dDevice->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
                                          pVSBlob->GetBufferSize(), &g_pVertexLayout );
    pVSBlob->Release();
    if( FAILED( hr ) )
        return hr;

    // Set the input layout
    g_pImmediateContext->IASetInputLayout( g_pVertexLayout );
```

## 构造顶点数据
模型都是由顶点组成的。要先在系统内存中创建顶点数据，然后再设置到显存中。

本项目的模型是一个矩形，因此如下 ，有4个顶点。
```C++
    // Create vertex buffer
    SimpleVertex vertices[] =
    {

        { XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, //左上  A
        { XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },  //右上  B
        { XMFLOAT3( -1.0f, -1.0f, 0.0f ), XMFLOAT2( 0.0f, 1.0f ) },//左下 C
        { XMFLOAT3( 1.0f, -1.0f, 0.0f ), XMFLOAT2( 1.0f, 1.0f ) }, //右下D
    };
```
## 把顶点数据设置到显存

- ID3D11Device::CreateBuffer
  - 创建显存
- ID3D11DeviceContext::IASetVertexBuffers
  - 设置顶点缓存

先创建一块显存，并使用系统内存vertices初始化这块显存。

使用D3D11的这个结构描述显存块的。
```C++
typedef struct D3D11_BUFFER_DESC
    {
    UINT ByteWidth;
    D3D11_USAGE Usage;
    UINT BindFlags;
    UINT CPUAccessFlags;
    UINT MiscFlags;
    UINT StructureByteStride;
    } 	D3D11_BUFFER_DESC;

#if !defined( D3D11_NO_HELPERS ) && defined( __cplusplus )
}
```

创建和设置逻辑如下：

```C++
    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( SimpleVertex ) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = vertices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pVertexBuffer );
    if( FAILED( hr ) )
        return hr;
    // Set vertex buffer
    UINT stride = sizeof( SimpleVertex );
    UINT offset = 0;
    g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );
```

## 设置顶点索引 和三角形拓朴信息
一般使用顶点索引来描述模型。这样可以节省一些显存。

一般使用三角形基元描述整个模型。

- ID3D11Device::CreateBuffer
  - 创建显存
- ID3D11DeviceContext::IASetIndexBuffer
  - 设置顶点索引

先在系统内存中定义好索引数据，如以下indecs数组。

使用CreateBuffer创建显存块，并描述好显存块内存布局，然后使用系统内存indecs初始好这块显存。

通过接口ID3D11DeviceContext::IASetIndexBuffer把创建初始化好的显存块设置为  顶点索引数据。

最后通过 通过接口ID3D11DeviceContext::IASetPrimitiveTopology设置三角形的拓朴信息。这里使用的D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST：每3个点组成一个三角形。

```C++
    // Create index buffer
    // Create vertex buffer
    WORD indices[] =
    {
        0, 1, 2,
        2, 1, 3
    };

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( WORD ) * 6;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pIndexBuffer );
    if( FAILED( hr ) )
        return hr;

    // Set index buffer
    g_pImmediateContext->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );

    // Set primitive topology
    g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
```

# 矩阵模型构造


# 正交投影矩阵
[XMMatrixOrthographicLH](https://learn.microsoft.com/zh-cn/windows/win32/api/directxmath/nf-directxmath-xmmatrixorthographiclh)
为左手坐标系构建一个正交投影矩阵。正交投影的矩阵，不存在远小近大。
```C++
XMMATRIX XM_CALLCONV XMMatrixOrthographicLH(
  [in] float ViewWidth,
  [in] float ViewHeight,
  [in] float NearZ,
  [in] float FarZ
) noexcept;
```
## 参数
[in] ViewWidth

接近剪裁平面的 Frustum 宽度。

[in] ViewHeight

接近剪裁平面的顶端高度。

[in] NearZ

距离接近剪裁平面。

[in] FarZ

与远剪裁平面的距离。

## 本例中的实际调用
```C++
// 创建正交投影矩阵，主要用来实施2D渲染.
    g_Projection = XMMatrixOrthographicLH((float)2, (float)2, 0.1f, 100.0f);
```