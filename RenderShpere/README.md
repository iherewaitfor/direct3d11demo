
# 如何切割一个圆。

## 切分各顶点位置计算
怎么切分圆。
从正Y轴，垂直看向XZ平面看，一个圆被切看成sw份，比如切成6份。但共切了7次，其中第1次和每7次位置重叠（方便纹理映射）
从负Z轴，垂直看向XZ平面，一个半圆被切成Sj份，比如切成 4份。包括两边的点，共有5个点。

设置球的半径为r，

则计算为
```C++
    // float radius; //半径
    // int numSlices;  //纵切多个。
    // int numStacks;  //横切多少个
void MakeSphere(VertexList& vertices, IndexList& indices, float radius, int numSlices, int numStacks) {

    //生成顶点
    for(int j = 0; j <= numslices ; j++){
        float xyStep = j * ( PI / numStacks);
        float xyR = radius * sinf(xyStep);
        for(int i =0; i <= numSlices; i++){
            float xzStep = 2.0f * PI / numbSlices * i ;
            v.position.x = xyR * cosf(xzStep);
            v.position.y = radius * cosf(xyStep);
            v.position.z = xyR * sinf(xzStep);
            
            //从球面坐标，映射到纹理坐标
			v.texture.x = xzStep / (2.0f*PI);
			v.texture.y = xyStep / PI;
        }
    }
    // to do ： 使用索引生成组成球面的三角形。
}
```

## 纹理映射
从球面坐标  映射到 纹理平面横纵坐标。
设球面描述范围为
- sx：横切面上的角度，取值范围为[0,PI]
- sy: 纵切面上的角度，取值范围为[0,PI/2]，北极点取0，南极点取PI/2

纹理坐标为tx,xy，取范围均为[0,1]

则两者之间的映射关系为

sx-tx，sy-ty
