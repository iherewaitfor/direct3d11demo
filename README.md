# direct3d11demo

#   Tutorial 1: Direct3D 11 Basics
本示例展示创建一个最小的Direct3D11应用的必要步骤。创建窗口、创建设备对象、显示一个颜色到窗口上。
代码位置[Tutorial 01Basics](https://github.com/iherewaitfor/direct3d11demo/tree/main/Tutorial%2001Basics).

## How to run
先安装cmake，进入源码目录新建build目录。然后在build 目录中进入命令行。然后执行命令
```
cmake .. -G "Visual Studio 17 2022" -A Win32
```
。然后打开生成的sln文件，将Demo项目设置为启动项即可。 cmake使用可参考本github项目[cmakevisualstudio](https://github.com/iherewaitfor/cmakevisualstudio)

```bat
D:\srccode\direct3d11demo\Tutorial 01Basics\build>cmake .. -G "Visual Studio 17 2022" -A Win32
CMake Deprecation Warning at CMakeLists.txt:2 (cmake_minimum_required):
  Compatibility with CMake < 2.8.12 will be removed from a future version of
  CMake.

  Update the VERSION argument <min> value or use a ...<max> suffix to tell
  CMake that the project does not need compatibility with older versions.


-- Selecting Windows SDK version 10.0.22621.0 to target Windows 10.0.19045.
-- The C compiler identification is MSVC 19.33.31630.0
-- The CXX compiler identification is MSVC 19.33.31630.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.33.31629/bin/Hostx64/x86/cl.exe - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.33.31629/bin/Hostx64/x86/cl.exe - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: D:/srccode/direct3d11demo/Tutorial 01Basics/build

D:\srccode\direct3d11demo\Tutorial 01Basics\build>
```
或者根据你自己电脑安装的不同版本修改对应命令。可参考如下。项目中使用"Visual Studio 17 2022"
```
D:\srccode\direct3d11demo\Tutorial05Transformation\build>cmake -G help
CMake Error: Could not create named generator help

Generators
* Visual Studio 17 2022        = Generates Visual Studio 2022 project files.
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
  Visual Studio 9 2008 [arch]  = Generates Visual Studio 2008 project files.
                                 Optional [arch] can be "Win64" or "IA64".
```


## 创建window
就是正常的创建Window的流程。RegisterClassEx，CreateWindow。
## 创建设备对象[D3D11CreateDeviceAndSwapChain](https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-d3d11createdeviceandswapchain)
首先要创建三个对象： a Device、an imediat context 和 a swap chain。其中 the imediate context是一个Direct3D11引入的新对象（相对旧版Direct3D)。

在Direct3D10中，设备对象用于渲染和资源创建。而在Direct11中，the immediate context 用来渲染到buffer，而the device包含方法创建资源。

The swap chain则用于获取渲染的buffer，以及将内容显示到屏幕上。the swap chain 包含两个或多个buffer。主要包含the front buffer和the back buffer.这些buffer其实是纹理。the Device就是渲染内容到这些纹理上的。the front buffer就是正在显示内容，是只读的。不能修改。the back buffer是设备的渲染目标。一旦完成drawing操作，the swap chain就会通过交换这两个buffer显示the backbuffer。 然后当前 back buffer就变成 front buffer了，反之亦然。

在创建 swap chain前要先填充[DXGI_SWAPCHAIN_DESC](https://learn.microsoft.com/en-us/windows/win32/api/dxgi/ns-dxgi-dxgi_swap_chain_desc)结构。特别注意这几个字段：
- BackBufferUsage：用于指明我们怎么用the backbuffer.在本例中，我们是想把内容渲染到backbuffer，所以我们设置值DXGI_USAGE_RENDER_TARGET_OUTPUT。
- OutputWindow：用于显示的窗口。
- SampleDesc：用于启用多重采样。由于本例没有使用多重采样。SampleDesc的count设置成了1，Quality设置成了0，从而禁用了多重采样。

```C++
    ID3D11Device*           g_pd3dDevice = NULL;
    ID3D11DeviceContext*    g_pImmediateContext = NULL;
    IDXGISwapChain*         g_pSwapChain = NULL;

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof(sd) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 640;
    sd.BufferDesc.Height = 480;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = g_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    if( FAILED( D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels, numFeatureLevels,
                     D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, NULL, &g_pImmediateContext ) ) )
    {
        return FALSE;
    }

```

## create a render target view
创建完成设备后，接下来就要创建渲染目标视图。
A render target view is a type of resource view in Direct3D 11. A resource view allows a resource to be bound to the graphics pipeline at a specific stage. Think of resource views as typecast in C. A chunk of raw memory in C can be cast to any data type. We can cast that chunk of memory to an array of integers, an array of floats, a structure, an array of structures, and so on. The raw memory itself is not very useful to us if we don't know its type. Direct3D 11 resource views act in a similar way. For instance, a 2D texture, analogous to the raw memory chunk, is the raw underlying resource. Once we have that resource we can create different resource views to bind that texture to different stages in the graphics pipeline with different formats: as a render target to which to render, as a depth stencil buffer that will receive depth information, or as a texture resource. Where C typecasts allow a memory chunk to be used in a different manner, so do Direct3D 11 resource views.

We need to create a render target view because we would like to bind the back buffer of our swap chain as a render target. This enables Direct3D 11 to render onto it. We first call GetBuffer() to get the back buffer object. Optionally, we can fill in a D3D11_RENDERTARGETVIEW_DESC structure that describes the render target view to be created. This description is normally the second parameter to CreateRenderTargetView. However, for these tutorials, the default render target view will suffice. The default render target view can be obtained by passing NULL as the second parameter. Once we have created the render target view, we can call OMSetRenderTargets() on the immediate context to bind it to the pipeline. This ensures the output that the pipeline renders gets written to the back buffer. The code to create and set the render target view is as follows:

```C++
// Create a render target view
    ID3D11Texture2D *pBackBuffer;
    if( FAILED( g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)&pBackBuffer ) ) )
        return FALSE;
    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return FALSE;
    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, NULL );
```

The last thing we need to set up before Direct3D 11 can render is initialize the viewport. The viewport maps clip space coordinates, where X and Y range from -1 to 1 and Z ranges from 0 to 1, to render target space, sometimes known as pixel space. In Direct3D 9, if the application does not set up a viewport, a default viewport is set up to be the same size as the render target. In Direct3D 11, no viewport is set by default. Therefore, we must do so before we can see anything on the screen. Since we would like to use the entire render target for the output, we set the top left point to (0, 0) and width and height to be identical to the render target's size. To do this, use the following code:
```C++
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );
```

## Modifying the Message Loop

```C++
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            Render();  // Do some rendering
        }
    }

```
## The Rendering Code
```C++
    void Render()
    {
        //
        // Clear the backbuffer
        //
        float ClearColor[4] = { 0.0f, 0.125f, 0.6f, 1.0f }; // RGBA
        g_pd3dDevice->ClearRenderTargetView( g_pRenderTargetView, ClearColor );
    
        g_pSwapChain->Present( 0, 0 );
    }

```

# Tutorial02 Rendering a Triangle
本例将画一个三角形到屏幕。我们会过一遍和三角形相关的数据结构。本例的输出结果是画一个三角形到窗口的正中央。
## Elements of a Triangle
三个顶点定义了一个三角形。我们怎么把三个顶点传给GPU呢?在Direct3D11中，顶点信息（比如位置信息）存储在buffer资源里的。存储顶点的buffer叫vertext buffer。所以我们必须创建一个足够大的vertext buffer存储这三个顶点的信息，并且填充顶点位置信息。在Direct3D11中，在创建buffer资源时，应用必须指明buffer的大小是多少字节。我们知道三个顶点就够了，但每个顶点的需要多少个字节呢？为了回答这个问题，我们就需要理解 vertex layout了。（顶点布局）
## Input Layout  D3D11_INPUT_ELEMENT_DESC
顶点有位置信息。通常顶点同时还有其他信息，比如法线，一个或多个颜色，纹理坐标等等。Vertext layout定义了这些属性在内存的布局：属性的数据类型，个数，这些属性在内存中的顺序等。顶点通常使用一个结构表示。顶点的大小可以很方便地使用结构的大小表示。
本例我们只使用到了位置信息。使用了XMFLOAT3类型。这个类型表示一个有3个点的vector.

```C++
struct SimpleVertex
{
    XMFLOAT3 Pos;  // Position
};
```

我们现在有一个结构表示顶点了。我们存储了顶点信息在我们的应用的系统内存里。但是，当我们传包含我们顶点的vertext buffer给GPU时，我们只是给了一块内存。GPU必须知道vertext layout，才能正常从buffer中解析出顶点的各个属性。给了解决这个问题，我们需要使用到 input layout.。在Direct3D11中，input layout是一个用来描述顶点结构的Direct3D对象，以便GPU能理解。每一人上顶点属性使用[D3D11_INPUT_ELEMENT_DESC](https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_input_element_desc)描述。应用定义了一个D3D11_INPUT_ELEMENT_DESC数组来描述 顶点的input layout.
本例中，因为我们的顶点的属性只有一个，所以我们input layout的描述数组，只有一个元素。
```C++
// Define the input layout
D3D11_INPUT_ELEMENT_DESC layout[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
};
UINT numElements = ARRAYSIZE(layout);

```

## Vertext Layout
创建vertext layout对象需要vertext shader的input签名。
我们使用ID3DBlob对象(从D3DX11CompileFromFile返回)来获取表示vertextshader的input签名的二进制数据。获取到这一数据，我们就可以立即调用 ID3D11Device::CreateInputLayout() 创建vertext layout对象，使用ID3D11DeviceContext::IASetInputLayout() 来设置它成为被激活的vertext layout。
```C++
// Create the input layout
if( FAILED( g_pd3dDevice->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(), 
        pVSBlob->GetBufferSize(), &g_pVertexLayout ) ) )
    return FALSE;
// Set the input layout
g_pImmediateContext->IASetInputLayout( g_pVertexLayout );
```
## Creating Vertex Buffer
在初始化时，我们还必须创建vertext buffer来存储我们的顶点数据。在Direct3D11中，为了创建vertext buffer，我们需要先填充两个结构：D3D11_BUFFER_DESC 和 D3D11_SUBRESOURCE_DATA，然后调用ID3D11Device::CreateBuffer()。D3D11_BUFFER_DESC描述vetex buffer对象，D3D11_SUBRESOURCE_DATA描述在创建时复制到vertex buffer的实际数据。创建完后，使用ID3D11DeviceContext::IASetVertexBuffers()来绑定vertex buffer 到设备。
```C++
// Create vertex buffer
SimpleVertex vertices[] =
{
    XMFLOAT3( 0.0f, 0.5f, 0.5f ),
    XMFLOAT3( 0.5f, -0.5f, 0.5f ),
    XMFLOAT3( -0.5f, -0.5f, 0.5f ),
};
D3D11_BUFFER_DESC bd;
ZeroMemory( &bd, sizeof(bd) );
bd.Usage = D3D11_USAGE_DEFAULT;
bd.ByteWidth = sizeof( SimpleVertex ) * 3;
bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
bd.CPUAccessFlags = 0;
bd.MiscFlags = 0;
D3D11_SUBRESOURCE_DATA InitData; 
ZeroMemory( &InitData, sizeof(InitData) );
InitData.pSysMem = vertices;
if( FAILED( g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pVertexBuffer ) ) )
    return FALSE;

// Set vertex buffer
UINT stride = sizeof( SimpleVertex );
UINT offset = 0;
g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );
```
## Primitive Topology
表示两个三角形的两种topology
- triangle list: ABC CBD
- triangle strip: ABCD

```C++
// Set primitive topology
g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
```
## Rendering the Triangle

最后我们需要 调用 ID3D11DeviceContext::Draw()。该函数命令GPU使用当前的vertex buffer，vetex layout和基元topology。第一个参数是发送给GPU的顶点数量，第2个参数第一个被发磅的顶点的索引。因为本例中，我们只画一个三角形，并且是从vertex buffer的开头元素开始传的，所以我们用了3和0.
```C++
// Render a triangle 
g_pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
g_pImmediateContext->PSSetShader( g_pPixelShader, NULL, 0 );
g_pImmediateContext->Draw( 3, 0 );
```
# Tutorial 03 Shaders
本例将单独解释什么是shader，以及它们是怎么工作的。为了理解各个shader，我们将了解整个图形管线。如下图：

![image Direct11图形管线](./images/d3d11-pipeline-stages.jpg)

在教程2中，我们调用 了VSSetShader()和PSSetShader()，实际上是将shader绑定到管线对应阶段。然后当我们调用 了Draw之后，我们开始处理已经传到图形管线上的顶点数据。下面的部分将详细描述 Draw之后发生的事情。

## Shaders
在Direct3D11中，各个shader位于图形管线的不同阶段。它们是由GPU执行的很短的程序。取特定的输入数据，处理这些数据，然后输出结果给到管线的下一阶段使用。Direct3D11支持3种基本的shader: vertex shader， geometry shader和pixel shader。
- vertext shader： 以vertex为输入。由vertex buffer输入给GPU的顶点，都会跑一次这个程序。
- geometry shader: 以基元作为 输入。传给GPU的基元都会跑一次。基元可能是一个点，线或者三角形
- pixel shader: 以像素（也叫fragment)作为输入。我们将渲染的基元的每个像素都会跑一次。

vertex， geometry, 和pixel shader就是各个动作发生的核心。当我们使用Direct3D11渲染时，GPU必须要有合适的vertex shader(顶点着色器)和pixel shader。Geometry shder是Direct3D11的高级特性，是可选的。本教程不做讨论。

## Vertex Shaders（顶点着色器）
Vertex shader是由GPU针对顶点运行的短程序。可以把vertex shader比作这样的C函数：以vertext作为输入，处理这输入，然后输出修改过的顶点。在应用以vertex buffer的方式传顶点数据给GPU后，GPU遍历在vertext buffer的所有顶点，并且对每个顶点执行激动的vertex shader。传顶点数据给vertext shader作为入参。
vertext shader可以用来执行多种任务。vertext shader最生分的工作就是转换。转换是将向量从一个坐标系统转换到另一个的过程。比如，在3D场景的一个3角形，顶点是(0,0,0)(1,0,0)(0,1,0)。当这个三角被画到2D的texture buffer时，GPU必须知道这些顶点将要画到的vertex buffer中对应的点的2D坐标。转换帮助我们完成此类任务。转换会在下一教程中讨论。在本教程中，我们会使用一人上简单的 vertext shader。它除了将输入作为输出 ，什么都不做。
High-Level Shading Language（HLSL)

```C++
    float4 VS( float4 Pos : POSITION ) : SV_POSITION
    {
        return Pos;
    }
```
这个vertext shader看似C函数。HLSL用类C语法，以便让C/C++程序员更易学习。这个vertex shader，名叫VS，以float4类型为入参，输出一个float4值。在HLSL中，一个float4是一个包含4个元素的向量，每个元素都是一个浮点型数字。两个冒号分别定义了入参的语义 和返回值的语义。如上所述，HLSL中的语义描述了数据的性质。在我们上面的shader中，我们选择了POSITiON作为入参位置的语义，是因为这个参数必须包含顶点的位置。返回值的语义SV_POSITION是一个有特定含义的预定义的语义。这个语音告诉图形管线，和这个语义关联的数据定义了clip-space位置。GPU需要此位置才能在屏幕上绘制像素。（我们将在下一教程讨论clip-space）。在我们的shader中，我们输入位置 数据，并输入同样的数据到管线上。

## Pixel Shaders（像素着色器）
现代计算机显示器通常是光栅显示器。这意味着显示屏实际上是由一个个叫像素点组件 二维网格。每个像素包含一个独立的颜色。当我们在显示屏上渲染一个三角形时，我们实际上并不是作为一个整体渲染这个三角开，而是我们点亮了这个三角形覆盖一区域中的这些像素点。如下图所示。右边为实际情况。

![image 光栅化 ](./images/Tutorial03_Figure2_Rasterization.png)

将三个顶点定义的三角形转换成被这个三角形覆盖的一组像素的过程，叫做光栅化。GPU首先要确定哪些像素点是正在被渲染的三角形所覆盖。然后它调用 激活的pixel shader去处理每一个被覆盖的像素。pixel shader主要的作用是去计算每一个pixel的颜色。pixel shader接受特定的关于这个像素将怎样着色的输入，计算这个像素的颜色，然后输出颜色到管线 上。这个输入来自激活的geometry sahder。或者如果geometry sahder不存在的话，如本教程这样，输出直接来自vertex shader。

我们之前的创建的vertext shader 输出 了一个语义是SV_POSITION的float4。这个将是我们pixel shader的输入。因为pixel shader输出 颜色值，我们pixel shader的输出将是float4。我们为输出提供语义SV_TARGET，以表示输出到渲染目标。
```C++
    float4 PS( float4 Pos : SV_POSITION ) : SV_Target
    {
        return float4( 1.0f, 1.0f, 0.0f, 1.0f );    // Yellow, with Alpha = 1
    }

```

## 创建shaders 
D3DX11CompileFromFile

```c++
    // Create the vertex shader
    if( FAILED( D3DX11CompileFromFile( "Tutorial03.fx", NULL, NULL, "VS", "vs_4_0", D3DCOMPILE_ENABLE_STRICTNESS, NULL, NULL, &pVSBlob, &pErrorBlob, NULL ) ) )
        return FALSE;

    // Create the pixel shader
    if( FAILED( D3DX11CompileFromFile( "Tutorial03.fx", NULL, NULL, "PS", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS, NULL, NULL, &pPSBlob, &pErrorBlob, NULL ) ) )
        return FALSE;

```

在我们过一遍图形管线后，我们开始理解渲染一个三角开的过程了。关键两步。
1. 在顶点数据中创建资源数据
2. 创建各个shader：转换这些数据进行渲染。

# Tutorial04 3Dspaces
在本教程，我们将深入研究3D位置和转换的细节。 本教程的输出结果是一个被渲染到屏幕的3D对象。
## 3D Spaces(3维空间)

在上个教程中，三角形的顶点被特意放置，以其在屏幕上完美对齐。然后情况并百都是如此。因此我们需要一个在3D空间中表示对象的系统和显示它们的系统。

在现实世界中，物体存在于3维空间中。这意味着，要将对象放置在世界上的特定位置，我们需要使用坐标系，并定义该 位置对应的三个坐标。在计算机图形学中，3维空间通常是使用迪卡尔坐标系。在这个坐标系中，相互垂直的三个轴X、Y和Z，决定 了空间中每个点的坐标。这坐标系统还进一步分为左手系和右手系。在左手系中，当X轴指出右，Y轴指向上时，Z轴指向前。在右手系中，当同样的X、Y指向，Z轴指出后。

![image 左手系和右手系](./images/T04coordinate_systems.png)

现在我们已经讨论了考虑三维空间时的坐标系。一个定在不同的空间中，会有不同的坐标。比如在一维空间中，假设我们有一个买尺子，我们标记一个点P,标记在尺子5寸的位置 。现在如果我们向右移动尺子1寸，同样的点，躺在了4寸的位置 。通过移动尺子，参考系发生了变化。因此虽然该点没有移动 ，但它已经有了新的坐标。

![image 一维空间图示](./images/T04SpacesIllustrationIn1D.png)

在3D中，空间通常由一个原点和来自该原点的三个唯一轴定义：X、Y和Z。在计算机图形学中，常用的空间有：对象空间、世界空间、视图空间和屏幕空间。

## Object Space 对象空间

![image 对象空间](./images/T04cube.png)

注意这个立方体是以原点为中心的。对象空间也叫模型空间，是指艺术家在创建三维模型时使用的空间。通常艺术家创建模板时是以原点为中心的，这样更易于执行诸如旋转等转换，我们后续将废教讨论转换。以下八个点的坐标如下：
```
    (-1,  1, -1)
    ( 1,  1, -1)
    (-1, -1, -1)
    ( 1, -1, -1)
    (-1,  1,  1)
    ( 1,  1,  1)
    (-1, -1,  1)
    ( 1, -1,  1)
```

因为对象空间是世界在设置和创建模板时使用的。所以这些存储在磁盘的模型也是在对象空间中。应用程序 可以创建vertex buffer表示模型，并使用模型数据初始化vertext buffer。因此通过在vertex buffer中的顶点也是在对象空间中。这也意味着，vertex shader接收的输入顶点数据在模型空间中。

## world Space 世界空间
世界空间是场景中每个对象共享的空间。它用于定义要渲染的对象之间的空间关系。为了可视化世界空间，我们可以想象我们站在一个朝北的长方形房间的相关西南角。我们将脚所处的角定义为原点(0,0,0)。则X轴指向我们右边，Y轴指向上，Z轴指向前，和我们面向的方向一致。当我们这样做后，房间中的每一个点都可以通过一组XYZ坐标标记。比如可能会有一把椅子在我们前面5尺，右边2尺的位置。也可能8英尺高的天花板上有一盏灯，就在椅子的正上方。我们可以称椅子的位置为(2,0,5)，灯的位置为（2，8，5）。正如我们所看到的，世界空间之所以被称为“世界空间”，是历史遗留它们告诉我们物体 在世界上彼此之间的关系。
