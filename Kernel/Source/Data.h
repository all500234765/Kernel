#ifdef __cplusplus
template<typename T>
class StructuredBuffer;

#define RWStructuredBuffer StructuredBuffer

#include <DirectXMath.h>

typedef DirectX::XMFLOAT3 float3;
#endif

struct Data {
    float3 Pixel;
};

RWStructuredBuffer<Data> sbData;
