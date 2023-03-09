//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
//由c++中的PSSetShaderResources( 0, 3, g_resourceViewPlanes_)设置了3个纹理。
Texture2D txY : register( t0 );  //t0对应纹理0，
Texture2D txU : register( t1 );  //t1对应纹理1
Texture2D txV : register( t2 );  //t2对应纹理2
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

cbuffer cbChangesEveryFrame : register( b2 )//b1对应ConstantBuffer2
{
    matrix World;
};


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0; //纹理0的坐标。由于YUV的纹理坐标用的都是一样的。所以只取纹理0（Y纹理）的坐标就可以的。UV也用Y的纹理坐标
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
	float y = txY.Sample(samLinear, input.Tex).r;
    float u = txU.Sample(samLinear, input.Tex).r  - 0.5f;
    float v = txV.Sample(samLinear, input.Tex).r  - 0.5f;
    float r = y + 1.14f * v;
	float g = y - 0.394f * u - 0.581f * v;
	float b = y + 2.03f * u;
	return float4(r,g,b, 1.0f);
}
