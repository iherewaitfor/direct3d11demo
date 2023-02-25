# RectTexture
本项目主要是使用了正交矩阵，进行2D渲染。将纹理渲染到矩形上。
## 矩阵模型构造

## 正交投影矩阵
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
### 参数
[in] ViewWidth

接近剪裁平面的 Frustum 宽度。

[in] ViewHeight

接近剪裁平面的顶端高度。

[in] NearZ

距离接近剪裁平面。

[in] FarZ

与远剪裁平面的距离。

### 本例中的实际调用
```C++
// 创建正交投影矩阵，主要用来实施2D渲染.
    g_Projection = XMMatrixOrthographicLH((float)2, (float)2, 0.1f, 100.0f);
```