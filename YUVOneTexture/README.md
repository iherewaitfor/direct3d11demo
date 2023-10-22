# YUVTexture
使用单一纹理，实现YUV420数据渲染。通过写像素着色器，从单一纹理中，通过纹理坐标变换，分别获取y、u、v的数据，然后再计算RGBA数据，从而实现单一纹理渲染yuv420数据。
# 像素着色器
从单纹理中，将YUV数据转成RGB数据。
```C++
float4 PS( PS_INPUT input) : SV_Target
{
    //先计算yuv数据对应的纹理坐标，在单一纹理中的对应的纹理位置
    float2 yTextUV; //Y纹理在单一纹理中的纹理坐标。
    yTextUV.x = input.Tex.x; //y纹理对应的横坐标，即单一纹理的横坐标
    yTextUV.y = input.Tex.y * 2/3; // y纹理对应纵坐标，对应单一纹理的纵坐标的位置。

    float2 uTextUV; //U纹理在单一纹理中的纹理坐标。
    const float PRECISION_OFFSET = 0.2f;
    const int uHeight = videoHeight/2; // U纹理的高度值 
    int uh = floor(input.Tex.y*uHeight + PRECISION_OFFSET); //从归一化坐标 转换回整数矩形坐标。U的高为videoHeight/2
    uTextUV.x = input.Tex.x*0.5f + 0.5f*(uh%2);

    uh = (uh < 2 ? 2 : uh); //上边缘对齐。解决上边缘绿色问题
    uh = (uHeight - uh < 2 ? (uHeight - 2) : uh ); //下边缘对齐，解决下边缘绿色问题
    uTextUV.y = 2.0f/3 + uh/2*2.0f/3.0/videoHeight;

    float2 vTextUV; //V纹理在单一纹理中的纹理坐标。
    vTextUV.x = uTextUV.x;//UV纹理的横坐标相同。
    vTextUV.y = 1.0f/6 + uTextUV.y; //在单一纹理中，V纹理的纵坐标比U纹理的低1/6。填充数据量，是按YUV的顺序填的数据。

	float y = tx.Sample(samLinear, yTextUV).r;
    float u = tx.Sample(samLinear, uTextUV).r  - 0.5f;
    float v = tx.Sample(samLinear, vTextUV).r  - 0.5f;
    float r = y + 1.14f * v;
	float g = y - 0.394f * u - 0.581f * v;
	float b = y + 2.03f * u;
	return float4(r,g,b, 1.0f);
}
```