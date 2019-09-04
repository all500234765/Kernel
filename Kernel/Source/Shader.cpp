#include "Shader.h"

#include <D3Dcompiler.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

Shader::Shader() {
}

void Shader::SetNullShader(ShaderType type) {
    // Create shader
    switch( type ) {
        case Pixel   : sPixel = 0; break;
        case Geometry: sGeometry = 0; break;
        case Hull    : sHull = 0; break;
        case Domain  : sDomain = 0; break;
        case Compute : sCompute = 0; break;
    }

    // Shader was compiled natively
    Type |= type;
}

bool Shader::LoadFile(std::string fname, ShaderType type) {

    if( type == Vertex ) {
        return 1;
    }

    // Create shader
    HRESULT hr;
    ID3DBlob *ShaderBuffer; ShaderBuffer = 0;

    fname = "Shaders/" + fname;

    // Read blob
    hr = D3DReadFileToBlob(std::wstring(fname.begin(), fname.end()).c_str(), &ShaderBuffer);
    if( FAILED(hr) ) {
        std::cout << "Can't read shader to blob! (" << type << ")" << std::endl;
        return 1;
    }

    // Create shader
    switch( type ) {
        case Pixel   : hr = gDirectX->gDevice->CreatePixelShader(ShaderBuffer->GetBufferPointer(), ShaderBuffer->GetBufferSize(), NULL, &sPixel);       break;
        case Geometry: hr = gDirectX->gDevice->CreateGeometryShader(ShaderBuffer->GetBufferPointer(), ShaderBuffer->GetBufferSize(), NULL, &sGeometry); break;
        case Hull    : hr = gDirectX->gDevice->CreateHullShader(ShaderBuffer->GetBufferPointer(), ShaderBuffer->GetBufferSize(), NULL, &sHull);         break;
        case Domain  : hr = gDirectX->gDevice->CreateDomainShader(ShaderBuffer->GetBufferPointer(), ShaderBuffer->GetBufferSize(), NULL, &sDomain);     break;
        case Compute : hr = gDirectX->gDevice->CreateComputeShader(ShaderBuffer->GetBufferPointer(), ShaderBuffer->GetBufferSize(), NULL, &sCompute);   break;
    }

    if( FAILED(hr) ) {
        ShaderBuffer->Release();
        ShaderBuffer = 0;
        std::cout << "Can't create shader! (" << type << ")" << std::endl;
        return 1;
    }

    // Shader was compiled natively
    Type |= type;

    // Store blob for futher use
    bShader[(int)log2((int)type)] = ShaderBuffer;

    if( type == Vertex ) {
    }

    // Done
    return 0;
}

void Shader::DeleteShaders() {
    // We can't release data that we don't own
    if( ((Type & Pixel   ) == Pixel   ) && ((Linked & Pixel   ) == 0) && sPixel    ) { sPixel->Release();    }
    if( ((Type & Geometry) == Geometry) && ((Linked & Geometry) == 0) && sGeometry ) { sGeometry->Release(); }
    if( ((Type & Hull    ) == Hull    ) && ((Linked & Hull    ) == 0) && sHull     ) { sHull->Release();     }
    if( ((Type & Domain  ) == Domain  ) && ((Linked & Domain  ) == 0) && sDomain   ) { sDomain->Release();   }
    if( ((Type & Compute ) == Compute ) && ((Linked & Compute ) == 0) && sCompute  ) { sCompute->Release();  }
}

void Shader::ReleaseBlobs() {
    for( int i = 0; i < Count; i++ ) {
        char j = 1 << i;

        // If exists and not linked
        if( ((Type & j) == Type) && ((Linked & j) == 0) && bShader[i] ) {
            bShader[i]->Release();
            bShader[i] = 0;
        }
    }
}

ID3DBlob* Shader::GetBlob(ShaderType type) {
    return bShader[(int)log2((int)type)];
}

void Shader::AttachShader(Shader* origin, ShaderType type) {
    // If already loaded compiled shader
    if( (Type & type) == type ) {
        std::cout << "Can't attach shader type of " << type << ", since it's already loaded." << std::endl;
        return;
    }

    // Add ref to shader
    switch( type ) {
        case Pixel   : sPixel    = origin->sPixel   ; break;
        case Geometry: sGeometry = origin->sGeometry; break;
        case Hull    : sHull     = origin->sHull    ; break;
        case Domain  : sDomain   = origin->sDomain  ; break;
        case Compute : sCompute  = origin->sCompute ; break;
    }

    // Add ref to polygon layout class
    if( type == Vertex ) {
    }

    // Add as linked
    Linked |= type;
    Type |= type;
}

void Shader::Bind() {
    if( ((Type & Pixel   ) == Pixel   ) ) { gDirectX->gContext->PSSetShader(sPixel, NULL, 0);    } else { gDirectX->gContext->PSSetShader(nullptr, NULL, 0); }
    if( ((Type & Geometry) == Geometry) ) { gDirectX->gContext->GSSetShader(sGeometry, NULL, 0); } else { gDirectX->gContext->GSSetShader(nullptr, NULL, 0); }
    if( ((Type & Hull    ) == Hull    ) ) { gDirectX->gContext->HSSetShader(sHull, NULL, 0);     } else { gDirectX->gContext->HSSetShader(nullptr, NULL, 0); }
    if( ((Type & Domain  ) == Domain  ) ) { gDirectX->gContext->DSSetShader(sDomain, NULL, 0);   } else { gDirectX->gContext->DSSetShader(nullptr, NULL, 0); }
//  if( ((Type & Compute ) == Compute ) ) { gDirectX->gContext->CSSetShader(sCompute, NULL, 0);  } else { gDirectX->gContext->CSSetShader(nullptr, NULL, 0); }
}

void Shader::Dispatch(UINT x, UINT y, UINT z) {
    if( (Type & Compute) == Compute ) {
        gDirectX->gContext->CSSetShader(sCompute, NULL, 0);
        gDirectX->gContext->Dispatch(x, y, z);
    }
}
