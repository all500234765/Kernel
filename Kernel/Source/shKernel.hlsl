#include "Data.h"

[numthreads(1, 1, 1)]
void main(uint3 q : SV_GroupID) {
    uint index = q.x + q.y * 1024;

    sbData[index].Pixel = float3((float)q.x / 1024.f, (float)q.y / 540.f, 0.f);


}
