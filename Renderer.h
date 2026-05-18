#pragma once

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include "Camera.h"
#include "Level.h"
#include "Game.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

struct Vertex
{
    float x;
    float y;
    float z;

    float r;
    float g;
    float b;
    float a;
};

struct ConstantBuffer
{
    XMMATRIX wvp;
};


class Renderer
{
public:

    Renderer();

    bool Initialise(HWND hwnd);

    void RenderFrame();

private:

    bool CreateDeviceAndSwapChain(HWND hwnd);

    bool CreateRenderTarget();

    bool CreateDepthBuffer();

    bool CreateViewport();

    bool CreateRasterizerState();

    bool CreateHudDepthStencilState();

    bool CreateShaders();

    bool CreateCube();

    bool CreateConstantBuffer();

    void DrawCube(XMMATRIX world, XMMATRIX view, XMMATRIX projection);

    void DrawHudCounter();

    void DrawStartScreen();

    void DrawEndScreen();

    void DrawHudText(
        const char* text,
        float centerX,
        float centerY,
        float cellSize,
        XMMATRIX view,
        XMMATRIX projection);

    void DrawHudGlyph(
        char character,
        float x,
        float y,
        float cellSize,
        XMMATRIX view,
        XMMATRIX projection);

    void DrawHudDigit(
        int digit,
        float x,
        float y,
        XMMATRIX view,
        XMMATRIX projection);

    void DrawHudSegment(
        float x,
        float y,
        float scaleX,
        float scaleY,
        float rotation,
        XMMATRIX view,
        XMMATRIX projection);

private:

    ComPtr<ID3D11Device> mDevice;
    ComPtr<ID3D11DeviceContext> mDeviceContext;
    ComPtr<IDXGISwapChain> mSwapChain;
    ComPtr<ID3D11RenderTargetView> mRenderTargetView;
    ComPtr<ID3D11DepthStencilView> mDepthStencilView;

    ComPtr<ID3D11VertexShader> mVertexShader;
    ComPtr<ID3D11PixelShader> mPixelShader;
    ComPtr<ID3D11InputLayout> mInputLayout;

    ComPtr<ID3D11Buffer> mVertexBuffer;
    ComPtr<ID3D11Buffer> mIndexBuffer;
    ComPtr<ID3D11Buffer> mConstantBuffer;
    ComPtr<ID3D11RasterizerState> mRasterizerState;
    ComPtr<ID3D11DepthStencilState> mHudDepthStencilState;

    float mRotationAngle;
    bool mGameStarted;
    bool mGameEnded;

    Camera mCamera;

    Level mLevel;

    Game mGame;

    
};