#include "Data.h"

float Scene(float3 t) {
    return length(t - float3(0.f, 0.f, 0.f)) - 1.f;
}

float3 Trace(float3 p0, float3 dir, float t0, float t1, float bounces=1) {
    for( float t = t0; t <= t1; t += .1f ) {
        float d = Scene(p0 + dir * t);

        if( d <= t0 ) return 1.f;
    }

    return 0.f;
}

[numthreads(1, 1, 1)]
void main(uint3 q : SV_GroupID) {
    uint index = q.x + q.y * 1024;
    float2 uv = float2((float)q.x / 1024.f, (float)q.y / 540.f);
    float3 Dir = float3(uv, 0.f);

    float3 Result = (Trace(float3(0.f, 0.f, -5.f), Dir, 0.f, 128.f));

    sbData[index].Pixel = Result;
}
