#pragma once

#include "Buffer.h"
#include "Shader.h"

template<typename T>
class StructuredBuffer: public Buffer {
    ID3D11ShaderResourceView *pSRV;
    ID3D11UnorderedAccessView *pUAV;
    D3D11_MAPPED_SUBRESOURCE res;
    bool bMapped;

public:
    void CreateDefault(UINT num, T* data, bool UAV=false, UINT CPUAF=0) {
        HRESULT hr = S_OK;
        Stride = sizeof(T);
        Number = num;

        // Create buffer
        pDesc = {};
        pDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE | (UAV ? D3D11_BIND_UNORDERED_ACCESS : 0);
        pDesc.ByteWidth           = Number * Stride;
        pDesc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        pDesc.Usage               = D3D11_USAGE_DEFAULT;
        pDesc.CPUAccessFlags      = CPUAF;
        pDesc.StructureByteStride = Stride;

        D3D11_SUBRESOURCE_DATA pData = { &data[0], 0, 0 };
        if( (hr = gDirectX->gDevice->CreateBuffer(&pDesc, &pData, &pBuff)) != S_OK ) {
            std::cout << "Failed to create StructuredBuffer (error=" << hr << ")" << std::endl;
            return;
        }

        std::cout << "Successfully created StructuredBuffer " << (UAV ? "with UAV" : "") <<
                     " (size=" << Number * Stride << ", num=" << Number << ")" << std::endl;

        // Create UAV
        if( UAV ) {
            D3D11_UNORDERED_ACCESS_VIEW_DESC pUAVDesc = {};
            pUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
            pUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
            pUAVDesc.Buffer.FirstElement = 0;
            pUAVDesc.Buffer.NumElements = Number;

            if( (hr = gDirectX->gDevice->CreateUnorderedAccessView(pBuff, &pUAVDesc, &pUAV)) != S_OK ) {
                std::cout << "Failed to create UAV for StructuredBuffer (error=" << hr << ")" << std::endl;
                return;
            }

            std::cout << "Successfully created UAV for StructuredBuffer" << std::endl;
        }

        // Create SRV
        D3D11_SHADER_RESOURCE_VIEW_DESC pSRVDesc = {};
        pSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        pSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
        pSRVDesc.Buffer.FirstElement = 0;
        pSRVDesc.Buffer.NumElements = Number;

        if( (hr = gDirectX->gDevice->CreateShaderResourceView(pBuff, &pSRVDesc, &pSRV)) != S_OK ) {
            std::cout << "Failed to create SRV for StructuredBuffer (error=" << hr << ")" << std::endl;
            return;
        }

        std::cout << "Successfully created SRV for " << (UAV ? "UAV " : "") << "StructuredBuffer" << std::endl;
    }

    void Bind(Shader::ShaderType type, UINT slot) { Bind(static_cast<UINT>(type), slot); }

    void Bind(UINT type, UINT slot, bool UAV=false) {
        if( !pSRV ) { return; }

        if( type & Shader::Vertex   ) gDirectX->gContext->VSSetShaderResources(slot, 1, &pSRV);
        if( type & Shader::Pixel    ) gDirectX->gContext->PSSetShaderResources(slot, 1, &pSRV);
        if( type & Shader::Geometry ) gDirectX->gContext->GSSetShaderResources(slot, 1, &pSRV);
        if( type & Shader::Hull     ) gDirectX->gContext->HSSetShaderResources(slot, 1, &pSRV);
        if( type & Shader::Domain   ) gDirectX->gContext->DSSetShaderResources(slot, 1, &pSRV);
        if( type & Shader::Compute  ) {
            if( UAV ) {
                UINT pInitial = { 0 };
                gDirectX->gContext->CSSetUnorderedAccessViews(slot, 1, &pUAV, &pInitial);
            } else {
                gDirectX->gContext->CSSetShaderResources(slot, 1, &pSRV);
            }
        }
    }

    void Release() {
        if( pSRV ) pSRV->Release();
        if( pBuff ) pBuff->Release();
    }

    void* Map() {
        bMapped = false;
        if( !pBuff ) { return nullptr; }

        HRESULT hr;

        // Try to get mapped resource
        hr = gDirectX->gContext->Map(pBuff, 0, D3D11_MAP_READ, 0, &res);
        if( FAILED(hr) ) {
            std::cout << "Can't map StructuredBuffer." << std::endl;
            return NULL;
        }

        bMapped = true;
        return res.pData;
    }

    void Unmap() {
        if( !pBuff ) { return; }
        bMapped = false;
        gDirectX->gContext->Unmap(pBuff, 0);
    }

    T operator[](int i) {
        if( !bMapped ) {
            this->Map();
            this->Unmap();
        }

        return ((T*)res.pData)[i];
    }
};
