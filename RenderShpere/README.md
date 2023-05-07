
# 如何切割一个圆。

## 切分各顶点位置计算
怎么切分圆。
从正Y轴，垂直看向XZ平面看，一个圆被切看成sw份，比如切成6份。但共切了7次，其中第1次和每7次位置重叠（方便纹理映射）
从负Z轴，垂直看向XZ平面，一个半圆被切成Sj份，比如切成 4份。包括两边的点，共有5个点。

设置球的半径为r，

则计算为
```C++
// float radius; //半径
// int numSlices;  //纵切多少份。
// int numStacks;  //横切多少份
void MakeSphere(VertexList& vertices, IndexList& indices, float radius, int numSlices, int numStacks) {

    //生成顶点
    for (int j = 0; j < numStacks + 1; j++) { 
        float xyStep = j * (XM_PI / numStacks); // 0-PI
        float xyR = radius * sinf(xyStep);   
        float tempY = radius * cosf(xyStep);
        float texY = xyStep / XM_PI;
        if (j == numStacks) { //由于存在浮点数计算精度差，修复南极点（及每个纬线圈接缝处）的位置和纹理坐标。
            xyR = 0.0f;
            tempY = radius;
            texY = 1.0f;
        }
        for (int i = 0; i <= numSlices; i++) {
            float xzStep = 2.0f * XM_PI / numSlices * i;  // 0 - 2PI

            VertexType v;
            v.position.x = xyR * cosf(xzStep);
            v.position.y = tempY;
            v.position.z = xyR * sinf(xzStep);

            //从球面坐标，映射到纹理坐标
            v.texture.x = xzStep / (2.0f * XM_PI);
            v.texture.y = xyStep / XM_PI;
            if (i == numSlices) {
                //修复接缝处纹理。
                v.texture.x = 1.0f;
            }

            vertices.push_back(v);
        }
    }
    // 使用索引生成组成球面的三角形。
    for (int j = 0; j < numStacks ; ++j)
    {
        int offset = j * (numSlices + 1);
        for (int i = 0; i < numSlices; ++i)
        {
            //由于全景视频时，是由球的里面往外看。所以此处三角形使用 逆时针顺序取顶点
            //第一个三角形。
            int index = i + offset;
            indices.push_back(index);
            indices.push_back(index + (numSlices + 1));
            indices.push_back(index + (numSlices + 1) + 1 );

            //第二个三角形
            indices.push_back(index);
            indices.push_back(index + (numSlices + 1) + 1);
            indices.push_back(index + 1);
        }
    }
}
```

### 顶点切分示意图

![images 球面模型的构建和顶点](./images/sphere.png)

例如：我们从纵向将球面切成6份，从横向将球面切成4份，

纵向分析：（投影到XZ平面）

则每个纬线圆上 共有 7 个点。其中每个纬线圆上，头尾的点（比如0、6两点）位置相同。这是为了解决纹理映射，其中纬线圆起点（比如0点、7、14等）的纹理坐标横坐标0。纬结圆终点（比如6、13、20等）的纹理坐标横坐标值为1。

横向分析：

看上图，横向把球面切成4份。考虑纹理映射所需，南北极点都有纬线圆，一共有5个纬线圆。每个纬线圆有7个点，一共有7X5=35个顶点。特别的其中同一极点（比如北极）纬线圆上的顶点，位置坐标相同，只是纹理横坐标不同，以便准确进行纹理映射。








## 纹理映射
从球面坐标  映射到 纹理平面横纵坐标。
设球面描述范围为
- sx：横切面上的角度，取值范围为[0,PI]
- sy: 纵切面上的角度，取值范围为[0,PI/2]，北极点取0，南极点取PI/2

纹理坐标为tx,xy，取范围均为[0,1]

则两者之间的映射关系为

sx-tx，sy-ty
