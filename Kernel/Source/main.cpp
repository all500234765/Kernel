#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>

#include "Shader.h"
#include "StructuredBuffer.h"
#include "ConstantBuffer.h"

#pragma comment(lib, "d3d11.lib")

_DirectX* gDirectX = new _DirectX();

#include "Data.h"

void Release() {
    if( gDirectX->gDevice ) gDirectX->gDevice->Release();
    if( gDirectX->gContext ) gDirectX->gContext->Release();
    if( gDirectX->gDebug ) gDirectX->gDebug->Release();
    delete gDirectX;
}

int main() {
    // 
    UINT Flags = 0;
#ifdef _DEBUG
    Flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_12_1 };
    if( D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, Flags, levels, 1, D3D11_SDK_VERSION, 
                          &gDirectX->gDevice, NULL, &gDirectX->gContext) != S_OK ) {
        Release();
        return 1;
    }

    // Set global object
    DirectXChild::SetDirectX(gDirectX);

#ifdef _DEBUG
    gDirectX->gDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&gDirectX->gDebug));
    if( gDirectX->gDebug ) gDirectX->gDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

    // Create buffers
    const int gWidth = 1024;
    const int gHeight = 540;

    const int Amount = gWidth * gHeight;

    std::vector<Data> Empty(Amount);
    sbData.CreateDefault(Amount, &Empty[0], true, D3D11_CPU_ACCESS_READ);

    // Load shaders
    Shader shCompute;
    shCompute.LoadFile("shKernel.cso", Shader::Compute);

    // Dispatch
    sbData.Bind(Shader::Compute, 0, true);

    shCompute.Dispatch(1024, 540, 1);

    // Release temp blobs
    shCompute.ReleaseBlobs();

    // 
    std::cout << "Compute is done. Saving BMP." << std::endl;
#pragma region Fill bmp file
    // 
    FILE *f;
    unsigned char *img = NULL;
    int filesize = 54 + 3 * gWidth * gHeight;  //w is your image width, h is image height, both int

    img = (unsigned char *)malloc(3 * gWidth * gHeight);
    memset(img, 0, 3 * gWidth * gHeight);

    for( int i = 0; i < gWidth * gHeight; i++ ) {
        img[i * 3 + 0] = (unsigned char)(sbData[i].Pixel.z * 255);
        img[i * 3 + 1] = (unsigned char)(sbData[i].Pixel.y * 255);
        img[i * 3 + 2] = (unsigned char)(sbData[i].Pixel.x * 255);

        /*for( int j = 0; j < 3; j++ ) {
            r = sbData[i].a[j] * 255;
            if( r > 255 ) r = 255;

            img[i * 3 + 2 - j] = (unsigned char)(r);
        }*/
    }

    // Image header
    unsigned char bmpfileheader[14] = { 'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0 };
    unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0 };
    unsigned char bmppad[3] = { 0,0,0 };

    bmpfileheader[2] = (unsigned char)(filesize);
    bmpfileheader[3] = (unsigned char)(filesize >> 8);
    bmpfileheader[4] = (unsigned char)(filesize >> 16);
    bmpfileheader[5] = (unsigned char)(filesize >> 24);

    bmpinfoheader[4] = (unsigned char)(gWidth);
    bmpinfoheader[5] = (unsigned char)(gWidth >> 8);
    bmpinfoheader[6] = (unsigned char)(gWidth >> 16);
    bmpinfoheader[7] = (unsigned char)(gWidth >> 24);
    bmpinfoheader[8] = (unsigned char)(gHeight);
    bmpinfoheader[9] = (unsigned char)(gHeight >> 8);
    bmpinfoheader[10] = (unsigned char)(gHeight >> 16);
    bmpinfoheader[11] = (unsigned char)(gHeight >> 24);

    f = fopen("Render.bmp", "wb");
    fwrite(bmpfileheader, 1, 14, f);
    fwrite(bmpinfoheader, 1, 40, f);
    for( int i = 0; i < gHeight; i++ ) {
        fwrite(img + (gWidth*(gHeight - i - 1) * 3), 3, gWidth, f);
        fwrite(bmppad, 1, (4 - (gWidth * 3) % 4) % 4, f);
    }

    free(img);
    fclose(f);
#pragma endregion

    // 
    std::cout << "Done!" << std::endl;
    std::cin.get();

    // Unload DirectX objects
    Release();
    sbData.Release();
    shCompute.Release();

    // 
    return 0;
}
