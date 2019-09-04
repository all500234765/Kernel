#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <D3Dcompiler.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "DirectXChild.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

class Shader: public DirectXChild {
private:
    ID3D11PixelShader *sPixel;
    ID3D11GeometryShader *sGeometry;
    ID3D11HullShader *sHull;
    ID3D11DomainShader *sDomain;
    ID3D11ComputeShader *sCompute;

    ID3DBlob *bShader[6];

    char Type = 0 /* Original */, Linked = 0 /* Linked from another shader */;

public:
    typedef enum {
        Vertex   = 1, 
        Pixel    = 2, 
        Geometry = 4, 
        Hull     = 8, 
        Domain   = 16, 
        Compute  = 32, 
        Count    = 6
    } ShaderType;

    Shader();

    void SetNullShader(ShaderType type);

    bool LoadFile(std::string fname, ShaderType type);
    void DeleteShaders();

    void ReleaseBlobs();
    ID3DBlob* GetBlob(ShaderType type);

    void AttachShader(Shader *origin, ShaderType type);

    void Bind();
    void Dispatch(UINT x=1, UINT y=1, UINT z=1);

    void Release() { DeleteShaders(); }
};
