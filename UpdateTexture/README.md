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
  - [设置RasterizerState](#设置rasterizerstate)
- [着色器编程概念](#着色器编程概念)
  - [纹理值](#纹理值)
  - [ConstantBuffer值](#constantbuffer值)
  - [顶点着色器的输入和输出](#顶点着色器的输入和输出)
  - [顶点着色器入口函数](#顶点着色器入口函数)
  - [像素着色器的输入和输出](#像素着色器的输入和输出)
- [顶点着色器](#顶点着色器)
  - [编译](#编译)
  - [创建顶点着色器](#创建顶点着色器)
  - [顶点着色器设置 及其ConstanBuffer值。](#顶点着色器设置-及其constanbuffer值)
- [像素着色器](#像素着色器)
  - [编译](#编译-1)
  - [创建像素着色器程序](#创建像素着色器程序)
  - [设置像素着色器及相关变量](#设置像素着色器及相关变量)
    - [设置纹理](#设置纹理)
    - [创建纹理 及资源视图](#创建纹理-及资源视图)
      - [准备纹理数据](#准备纹理数据)
      - [创建纹理，并用传入初始化的数据](#创建纹理并用传入初始化的数据)
      - [创建着色器资源视图（纹理视图）](#创建着色器资源视图纹理视图)
      - [更新纹理数据](#更新纹理数据)
    - [设置采样器](#设置采样器)
- [正交投影矩阵](#正交投影矩阵)
  - [参数](#参数)
  - [本例中的实际调用](#本例中的实际调用)
- [执行渲染](#执行渲染)

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

## 设置RasterizerState
这里全部使用默认值：
- CullMode默认值为D3D11_CULL_BACK，（不显示背面）
- 模型中三角形的顺序（此处三角形用的索引）：此项目使用顺时针
- FrontCounterClockwise：正面是逆时针方向？ 默认值为false（即顺时针方向 为正面）

以上三个值决定渲染模型的三角形的哪个面还是所有面。

[RectTexture3D/README.md#cullmode默认为d3d11_cull_back](https://github.com/iherewaitfor/direct3d11demo/blob/main/RectTexture3D/README.md#cullmode%E9%BB%98%E8%AE%A4%E4%B8%BAd3d11_cull_back)

# 着色器编程概念

以下为着色器源码：（包含顶点着色器和像素色差器）
```C++
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );
cbuffer cbNeverChanges : register( b0 )
{
    matrix View;
};
cbuffer cbChangeOnResize : register( b1 )
{
    matrix Projection;
};
cbuffer cbChangesEveryFrame : register( b2 )
{
    matrix World;
};
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( input.Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Tex = input.Tex;
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
    return txDiffuse.Sample( samLinear, input.Tex );
}

```

- register( t0 )表示取值纹理0
- register( s0 )表示取值采样器0
- register( b0 )表示取值ConstantBuffer0
- register( b1 )表示取值ConstantBuffer1
- register( b2 )表示取值ConstantBuffer2

可以通过D3D11对应的接口值，分别设置这些值到显卡。
## 纹理值 
以下表示第0块纹理。
```C++
Texture2D txDiffuse : register( t0 );
```
## ConstantBuffer值
以下ConstantBuffer值，由主应用程序设置进来 
```C++
cbuffer cbNeverChanges : register( b0 )
{
    matrix View;
};
cbuffer cbChangeOnResize : register( b1 )
{
    matrix Projection;
};
cbuffer cbChangesEveryFrame : register( b2 )
{
    matrix World;
};
```
主应用程序设置值进来
```C++
    g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pCBNeverChanges );
    g_pImmediateContext->VSSetConstantBuffers( 1, 1, &g_pCBChangeOnResize );
    g_pImmediateContext->VSSetConstantBuffers( 2, 1, &g_pCBChangesEveryFrame );
```

## 顶点着色器的输入和输出
顶点着色器输入，该定义的结构与上面编程的顶点内存布局要一致。只是这里的数据类型使用的是着色器的数据类型。
这两个成员值分别是 顶点位置和顶点的纹理坐标。

```C++
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};
```
## 顶点着色器入口函数
该函数入参为输入，输出类型为PS_INPUT。

主要逻辑功能：把原始模型的顶点分别进行世界转换、视图转换和投影转换。

其中mul为着色器的内置乘法函数。
```C++
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( input.Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Tex = input.Tex;
    
    return output;
}
```
## 像素着色器的输入和输出
像素着色器的输入为
```C++
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};
```

# 顶点着色器

## 编译
核心函数
- D3DX11CompileFromFile

错误输出：
ID3DBlob* pErrorBlob;
通过 LPCSTR errmsg = (char*)pErrorBlob->GetBufferPointer();
可以获取到编译的出错信息。调试时，可以用。

其中指定
- 源文件
  - 指定为了"RectTexture.fx"
- 入口函数
  - 这里指定为"VS"
- 着器模型
  - 模型版本，这里指定为4.0
  
```C++
    // Compile the vertex shader
    ID3DBlob* pVSBlob = NULL;
    hr = CompileShaderFromFile( L"RectTexture.fx", "VS", "vs_4_0", &pVSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }
```

```C++
//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile( szFileName, NULL, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
            LPCSTR errmsg = (char*)pErrorBlob->GetBufferPointer();
        if( pErrorBlob ) pErrorBlob->Release();
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();
    return S_OK;
}
```
## 创建顶点着色器
核心函数：
- CreateVertexShader
  - 输入：由上面CompileShaderFromFile输出的ID3DBlob。
  - 输出：ID3D11VertexShader

```C++
    ID3D11VertexShader*                 g_pVertexShader = NULL;

    // Create the vertex shader
    hr = g_pd3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader );
    if( FAILED( hr ) )
    {    
        pVSBlob->Release();
        return hr;
    }
```

## 顶点着色器设置 及其ConstanBuffer值。

```C++
  g_pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
```

```C++
cbuffer cbNeverChanges : register( b0 )
{
    matrix View;
};
cbuffer cbChangeOnResize : register( b1 )
{
    matrix Projection;
};
cbuffer cbChangesEveryFrame : register( b2 )
{
    matrix World;
};
```
分别对应顶点着色器的b0、b1、b2

```C++
    g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pCBNeverChanges );
    g_pImmediateContext->VSSetConstantBuffers( 1, 1, &g_pCBChangeOnResize );
    g_pImmediateContext->VSSetConstantBuffers( 2, 1, &g_pCBChangesEveryFrame );
```
# 像素着色器

## 编译
和顶点着色号器的编译类型，可以参考上面顶点着色器的编译部分。入口函数为"PS"，着色器模型为"ps_4_0"。
```C++
    // Compile the pixel shader
    ID3DBlob* pPSBlob = NULL;
    hr = CompileShaderFromFile( L"RectTexture.fx", "PS", "ps_4_0", &pPSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }
```
## 创建像素着色器程序
```C++
    // Create the pixel shader
    hr = g_pd3dDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader );
    pPSBlob->Release();
    if( FAILED( hr ) )
        return hr;
```
## 设置像素着色器及相关变量
设置像素着色器程序。
```C++
    g_pImmediateContext->PSSetShader( g_pPixelShader, NULL, 0 );
```

设置像素着色器需要的变量

### 设置纹理
- PSSetShaderResources参数含义
  - 0： 索引偏移量0
  - 1： 共一个纹理
  - &g_pTextureRV： ID3D11ShaderResourceView* 数组
    - 资源视图指针数组地址

```C++
    g_pImmediateContext->PSSetShaderResources( 0, 1, &g_pTextureRV );
```
### 创建纹理 及资源视图

```C++
bool createTexture() {
    const int textureWidth = 512;
    const int textureHeight = 512;
    D3D11_TEXTURE2D_DESC textureDesc;
    HRESULT result;
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;


    char* pData = new char[textureWidth * textureHeight * 4];
    ZeroMemory(pData, textureWidth * textureHeight * 4);
    for (int j = 0; j < textureHeight; j++) {
        for (int i = 0; i < textureWidth; i++) {
            int offset = j * textureWidth * 4 + i * 4;
            *(pData + offset) = 255; //R
            *(pData + offset + 1) = 0; //G
            *(pData + offset + 2) = 125; //B
            *(pData + offset + 3) = 255; //A
        }
    }

    D3D11_SUBRESOURCE_DATA initData = { 0 };
    initData.pSysMem = (const void*)pData;
    initData.SysMemPitch = textureWidth * 4; //一行的字节大小（子资源为2D/3D纹理时使用）
    //initData.SysMemSlicePitch = textureWidth * textureHeight * 4;
    initData.SysMemSlicePitch = 0;

    //创建2d纹理，
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    textureDesc.Width = textureWidth;
    textureDesc.Height = textureHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    result = g_pd3dDevice->CreateTexture2D(&textureDesc, &initData, &g_programTexture);//传入数据初始化纹理。
    //result = g_pd3dDevice->CreateTexture2D(&textureDesc, NULL, &g_programTexture); //不传入数据初始化，（D3D11_USAGE_DEFAULT）后续可以使用UpdateSubresource更新纹理。
    if (FAILED(result))
    {
        return false;
    }

    // 创建shader资源，和纹理关联起来
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    result = g_pd3dDevice->CreateShaderResourceView(g_programTexture, &shaderResourceViewDesc, &g_pTextureRV);
    if (FAILED(result))
    {
        return false;
    }
}
```

#### 准备纹理数据
为了减少依赖，这里直接自己生成纹理数据。

实际应用中，可能来自视频文件解码出来的视频帧，或者从网络上获取的视频帧。
```C++
    const int textureWidth = 512;
    const int textureHeight = 512;
    char* pData = new char[textureWidth * textureHeight * 4];
    ZeroMemory(pData, textureWidth * textureHeight * 4);
    for (int j = 0; j < textureHeight; j++) {
        for (int i = 0; i < textureWidth; i++) {
            int offset = j * textureWidth * 4 + i * 4;
            *(pData + offset) = 255; //R
            *(pData + offset + 1) = 0; //G
            *(pData + offset + 2) = 125; //B
            *(pData + offset + 3) = 255; //A
        }
    }
```

#### 创建纹理，并用传入初始化的数据
D3D11的2D纹理描述结构如下。
```C++
typedef struct D3D11_TEXTURE2D_DESC
    {
    UINT Width;
    UINT Height;
    UINT MipLevels;
    UINT ArraySize;
    DXGI_FORMAT Format;
    DXGI_SAMPLE_DESC SampleDesc;
    D3D11_USAGE Usage;
    UINT BindFlags;
    UINT CPUAccessFlags;
    UINT MiscFlags;
    } 	D3D11_TEXTURE2D_DESC;

#if !defined( D3D11_NO_HELPERS ) && defined( __cplusplus )
}
```

其中部分成员变量含义：
- Width 宽度
- Height 调试
- DXGI_FORMAT纹理格式，如RGAB为DXGI_FORMAT_R8G8B8A8_UNORM


初始化数据_子资源数据 的描述结构如下：

```C++
typedef struct D3D11_SUBRESOURCE_DATA
    {
    const void *pSysMem;
    UINT SysMemPitch;//一行的字节大小（子资源为2D/3D纹理时使用）
    UINT SysMemSlicePitch;
    } 	D3D11_SUBRESOURCE_DATA;
```

创建纹理的过程如下
- 准备初始化数据
  - 填充D3D11_SUBRESOURCE_DATA结构
- 填充纹理描述结构对象D3D11_TEXTURE2D_DESC
- 调用CreateTexture2D创建纹理

```C++
    const int textureWidth = 512;
    const int textureHeight = 512;
    D3D11_TEXTURE2D_DESC textureDesc;
    HRESULT result;
    D3D11_SUBRESOURCE_DATA initData = { 0 };
    initData.pSysMem = (const void*)pData;
    initData.SysMemPitch = textureWidth * 4; //一行的字节大小（子资源为2D/3D纹理时使用）
    initData.SysMemSlicePitch = 0;

    //创建2d纹理，
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    textureDesc.Width = textureWidth;
    textureDesc.Height = textureHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    result = g_pd3dDevice->CreateTexture2D(&textureDesc, &initData, &g_programTexture);//传入数据初始化纹理。
```
#### 创建着色器资源视图（纹理视图）
纹理只是一块内存数据，着色器需要访问，需要通过资源视图。
```C++
    ID3D11ShaderResourceView*           g_pTextureRV = NULL;

    // 创建shader资源，和纹理关联起来
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    result = g_pd3dDevice->CreateShaderResourceView(g_programTexture, &shaderResourceViewDesc, &g_pTextureRV);
    if (FAILED(result))
    {
        return false;
    }
```

创建出来的资源视图ID3D11ShaderResourceView，就可以通过函数PSSetShaderResources设置纹理了。本应用设置了一张纹理。
#### 更新纹理数据
除了通过创建纹理时，直接传数据来初始化纹理数据之外，还可以通过UpdateSubresource来更新纹理的数据。

可以通过 D3D11_BOX来控制只更新部分纹理数据。 一般都是更新整张纹理的话，直接传NULL就可以。

```C++
    //更新纹理数据。
    D3D11_BOX destRegion;
    destRegion.left = 0;
    destRegion.right = textureWidth;
    destRegion.top = 0;
    destRegion.bottom = textureHeight;
    destRegion.front = 0;
    destRegion.back = 1;
    g_pImmediateContext->UpdateSubresource(g_programTexture, 0, &destRegion, pData, textureWidth * 4 , 0);

    //D3D11_BOX填NULL，表示整张纹理。
    //g_pImmediateContext->UpdateSubresource(g_programTexture, 0, NULL, pData, textureWidth * 4 , 0);
```

### 设置采样器

参数
- 开始的偏移值
- 个数
- 采样器数组
```C++
    g_pImmediateContext->PSSetSamplers( 0, 1, &g_pSamplerLinear );
```

创建采样器
```C++
    ID3D11SamplerState*                 g_pSamplerLinear = NULL;
    // Create the sample state
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory( &sampDesc, sizeof(sampDesc) );
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = g_pd3dDevice->CreateSamplerState( &sampDesc, &g_pSamplerLinear );
    if( FAILED( hr ) )
        return hr;
```

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

# 执行渲染
核心函数
-  ID3D11DeviceContext::DrawIndexed
   -  画一个模型，使用索引描述的模型。
   -  如果有多个模型，可以调用多次
-  IDXGISwapChain::Present
   -  把画好的内存，切到前端呈现。
```C++
相关函数
- ID3D11DeviceContext::ClearRenderTargetView
  - 在渲染前，如有必要，先擦除目标渲染视图。
- ID3D11DeviceContext::ClearDepthStencilView
  - 清空深度模具视图

void Render()
{
    //
    // Clear the back buffer
    //
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
    g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, ClearColor );

    //
    // Clear the depth buffer to 1.0 (max depth)
    //
    g_pImmediateContext->ClearDepthStencilView( g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

    //
    // Update variables that change once per frame
    //
    CBChangesEveryFrame cb;
    cb.mWorld = XMMatrixTranspose( g_World );
    g_pImmediateContext->UpdateSubresource( g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0 );

    //
    // Render the cube
    //
    g_pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
    g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pCBNeverChanges );
    g_pImmediateContext->VSSetConstantBuffers( 1, 1, &g_pCBChangeOnResize );
    g_pImmediateContext->VSSetConstantBuffers( 2, 1, &g_pCBChangesEveryFrame );
    g_pImmediateContext->PSSetShader( g_pPixelShader, NULL, 0 );
    g_pImmediateContext->PSSetShaderResources( 0, 1, &g_pTextureRV );
    g_pImmediateContext->PSSetSamplers( 0, 1, &g_pSamplerLinear );
    g_pImmediateContext->DrawIndexed( 6, 0, 0 );

    //
    // Present our back buffer to our front buffer
    //
    g_pSwapChain->Present( 0, 0 );
}
```