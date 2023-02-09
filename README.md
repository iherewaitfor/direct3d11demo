# direct3d11demo

#   Tutorial 1: Direct3D 11 Basics
本示例展示创建一个最小的Direct3D11应用的必要步骤。创建窗口、创建设备对象、显示一个颜色到窗口上。
代码位置[Tutorial 01Basics](https://github.com/iherewaitfor/direct3d11demo/tree/main/Tutorial%2001Basics).
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
``

我们现在有一个结构表示顶点了。我们存储了顶点信息在我们的应用的系统内存里。但是，当我们传包含我们顶点的vertext buffer给GPU时，我们只是给了一块内存。GPU必须知道vertext layout，才能正常从buffer中解析出顶点的各个属性。给了解决这个问题，我们需要使用到 input layout.。在Direct3D11中，input layout是一个用描述顶点结构的Direct3D对象，以便GPU能理解。每一人上顶点属性使用[D3D11_INPUT_ELEMENT_DESC](https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_input_element_desc)描述。应用定义了一个D3D11_INPUT_ELEMENT_DESC数组来描述 顶点的input layout.
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