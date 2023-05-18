//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
//由c++中的PSSetShaderResources( 0, 1, g_resourceViewPlanes_)设置了1个纹理。
Texture2D tx : register( t0 );  //t0对应纹理0，

SamplerState samLinear : register( s0 );

//由  g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pCBNeverChanges )指定。
cbuffer cbNeverChanges : register( b0 )  //b0对应ConstantBuffer0
{
    matrix View;
};

cbuffer cbChangeOnResize : register( b1 ) //b1对应ConstantBuffer1
{
    matrix Projection;
};

cbuffer cbChangesEveryFrame : register( b2 )//b2对应ConstantBuffer2
{
    matrix World;
};

cbuffer cbTextDesc : register( b3 )//b3对应ConstantBuffer3
{
    int videoWidth;  //原视频的宽度
    int videoHeight; //原视频的高度
    int t0; //占位不用
    int t1; //占位不用
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0; //单一纹理坐标。Y、U、V对应纹理的坐标需要做对应转换
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
    return tx.Sample( samLinear, input.Tex );
}
