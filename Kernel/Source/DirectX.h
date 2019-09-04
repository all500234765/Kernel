#pragma once

#include <d3d11.h>

class _DirectX {
public:
    ID3D11Device* gDevice;
    ID3D11DeviceContext* gContext;
    ID3D11Debug *gDebug;

};
