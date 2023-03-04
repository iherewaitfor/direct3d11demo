//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txY : register( t0 );
Texture2D txU : register( t1 );
Texture2D txV : register( t2 );
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
    float4 vMeshColor;
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
	float y = txY.Sample(samLinear, input.Tex).r;
    float u = txU.Sample(samLinear, input.Tex).r  - 0.5f;
    float v = txV.Sample(samLinear, input.Tex).r  - 0.5f;
    float r = y + 1.14f * v;
	float g = y - 0.394f * u - 0.581f * v;
	float b = y + 2.03f * u;
	return float4(r,g,b, 1.0f);
}
