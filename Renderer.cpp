#include "Renderer.h"

#include <string>

Renderer::Renderer()
{
    mRotationAngle = 0.0f;
}

bool Renderer::Initialise(HWND hwnd)
{
    if (!CreateDeviceAndSwapChain(hwnd))
    {
        return false;
    }

    if (!CreateRenderTarget())
    {
        return false;
    }

    if (!CreateDepthBuffer())
    {
        return false;
    }

    if (!CreateViewport())
    {
        return false;
    }

    if (!CreateRasterizerState())
    {
        return false;
    }

    if (!CreateShaders())
    {
        return false;
    }

    if (!CreateCube())
    {
        return false;
    }

    if (!CreateConstantBuffer())
    {
        return false;
    }

    mLevel.BuildLevel();

    return true;
}
bool Renderer::CreateDeviceAndSwapChain(HWND hwnd)
{
    DXGI_SWAP_CHAIN_DESC scd = {};

    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &scd,
        mSwapChain.GetAddressOf(),
        mDevice.GetAddressOf(),
        nullptr,
        mDeviceContext.GetAddressOf());

    return SUCCEEDED(hr);
}

bool Renderer::CreateRenderTarget()
{
    ComPtr<ID3D11Texture2D> backBuffer;

    HRESULT hr = mSwapChain->GetBuffer(
        0,
        __uuidof(ID3D11Texture2D),
        (void**)backBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    hr = mDevice->CreateRenderTargetView(
        backBuffer.Get(),
        nullptr,
        mRenderTargetView.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool Renderer::CreateDepthBuffer()
{
    D3D11_TEXTURE2D_DESC depthBufferDesc = {};

    depthBufferDesc.Width = 1280;
    depthBufferDesc.Height = 720;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    ComPtr<ID3D11Texture2D> depthBuffer;

    HRESULT hr = mDevice->CreateTexture2D(
        &depthBufferDesc,
        nullptr,
        depthBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    hr = mDevice->CreateDepthStencilView(
        depthBuffer.Get(),
        nullptr,
        mDepthStencilView.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool Renderer::CreateViewport()
{
    D3D11_VIEWPORT viewport = {};

    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = 1280.0f;
    viewport.Height = 720.0f;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    mDeviceContext->RSSetViewports(
        1,
        &viewport);

    return true;
}

bool Renderer::CreateRasterizerState()
{
    D3D11_RASTERIZER_DESC rasterDesc = {};

    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.DepthClipEnable = true;

    HRESULT hr = mDevice->CreateRasterizerState(
        &rasterDesc,
        mRasterizerState.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    mDeviceContext->RSSetState(mRasterizerState.Get());

    return true;
}

bool Renderer::CreateShaders()
{
    std::string vertexShaderCode = R"(

cbuffer ConstantBuffer : register(b0)
{
    matrix wvp;
};

struct vertexIn
{
    float3 position : POSITION;
    float4 colour : COLOR;
};

struct vertexOut
{
    float4 position : SV_POSITION;
    float4 colour : COLOR;
};

vertexOut main(vertexIn input)
{
    vertexOut output;

    output.position = mul(float4(input.position, 1.0f), wvp);
    output.colour = input.colour;

    return output;
}

)";

    std::string pixelShaderCode = R"(

struct pixelIn
{
    float4 position : SV_POSITION;
    float4 colour : COLOR;
};

float4 main(pixelIn input) : SV_TARGET
{
    return input.colour;
}

)";

    ComPtr<ID3DBlob> vertexShaderBlob;
    ComPtr<ID3DBlob> pixelShaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3DCompile(
        vertexShaderCode.c_str(),
        vertexShaderCode.length(),
        nullptr,
        nullptr,
        nullptr,
        "main",
        "vs_5_0",
        0,
        0,
        vertexShaderBlob.GetAddressOf(),
        errorBlob.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    hr = D3DCompile(
        pixelShaderCode.c_str(),
        pixelShaderCode.length(),
        nullptr,
        nullptr,
        nullptr,
        "main",
        "ps_5_0",
        0,
        0,
        pixelShaderBlob.GetAddressOf(),
        errorBlob.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    hr = mDevice->CreateVertexShader(
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        nullptr,
        mVertexShader.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    hr = mDevice->CreatePixelShader(
        pixelShaderBlob->GetBufferPointer(),
        pixelShaderBlob->GetBufferSize(),
        nullptr,
        mPixelShader.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        {
            "COLOR",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            12,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
    };

    hr = mDevice->CreateInputLayout(
        layout,
        2,
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        mInputLayout.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool Renderer::CreateCube()
{
    Vertex vertices[] =
    {
        { -0.8f, -0.8f, -0.8f,  1.0f, 0.0f, 0.0f, 1.0f },
        { -0.8f,  0.8f, -0.8f,  1.0f, 0.0f, 0.0f, 1.0f },
        {  0.8f,  0.8f, -0.8f,  1.0f, 0.0f, 0.0f, 1.0f },
        {  0.8f, -0.8f, -0.8f,  1.0f, 0.0f, 0.0f, 1.0f },

        { -0.8f, -0.8f,  0.8f,  0.0f, 1.0f, 0.0f, 1.0f },
        {  0.8f, -0.8f,  0.8f,  0.0f, 1.0f, 0.0f, 1.0f },
        {  0.8f,  0.8f,  0.8f,  0.0f, 1.0f, 0.0f, 1.0f },
        { -0.8f,  0.8f,  0.8f,  0.0f, 1.0f, 0.0f, 1.0f }
    };

    unsigned int indices[] =
    {
        0, 1, 2,
        0, 2, 3,

        4, 6, 5,
        4, 7, 6,

        1, 7, 6,
        1, 6, 2,

        0, 3, 5,
        0, 5, 4,

        0, 4, 7,
        0, 7, 1,

        3, 2, 6,
        3, 6, 5
    };

    D3D11_BUFFER_DESC vertexBufferDesc = {};

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * 8;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexData = {};

    vertexData.pSysMem = vertices;

    HRESULT hr = mDevice->CreateBuffer(
        &vertexBufferDesc,
        &vertexData,
        mVertexBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    D3D11_BUFFER_DESC indexBufferDesc = {};

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned int) * 36;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA indexData = {};

    indexData.pSysMem = indices;

    hr = mDevice->CreateBuffer(
        &indexBufferDesc,
        &indexData,
        mIndexBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool Renderer::CreateConstantBuffer()
{
    D3D11_BUFFER_DESC bufferDesc = {};

    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(ConstantBuffer);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;

    HRESULT hr = mDevice->CreateBuffer(
        &bufferDesc,
        nullptr,
        mConstantBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

void Renderer::DrawCube(
    XMMATRIX world,
    XMMATRIX view,
    XMMATRIX projection)
{
    XMMATRIX wvp = world * view * projection;

    ConstantBuffer cb;

    cb.wvp = XMMatrixTranspose(wvp);

    mDeviceContext->UpdateSubresource(
        mConstantBuffer.Get(),
        0,
        nullptr,
        &cb,
        0,
        0);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    mDeviceContext->IASetInputLayout(mInputLayout.Get());

    mDeviceContext->IASetVertexBuffers(
        0,
        1,
        mVertexBuffer.GetAddressOf(),
        &stride,
        &offset);

    mDeviceContext->IASetIndexBuffer(
        mIndexBuffer.Get(),
        DXGI_FORMAT_R32_UINT,
        0);

    mDeviceContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    mDeviceContext->VSSetShader(
        mVertexShader.Get(),
        nullptr,
        0);

    mDeviceContext->VSSetConstantBuffers(
        0,
        1,
        mConstantBuffer.GetAddressOf());

    mDeviceContext->PSSetShader(
        mPixelShader.Get(),
        nullptr,
        0);

    mDeviceContext->DrawIndexed(36, 0, 0);
}



void Renderer::RenderFrame()
{
    float colour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    // Set the render target and depth buffer
    mDeviceContext->OMSetRenderTargets(
        1,
        mRenderTargetView.GetAddressOf(),
        mDepthStencilView.Get());

    // Clear the screen every frame
    mDeviceContext->ClearRenderTargetView(
        mRenderTargetView.Get(),
        colour);

    // Clear the depth buffer every frame
    mDeviceContext->ClearDepthStencilView(
        mDepthStencilView.Get(),
        D3D11_CLEAR_DEPTH,
        1.0f,
        0);

    mCamera.Update();

    mGame.Update(mCamera);

    // Update cube rotation

   // mRotationAngle += 0.01f;

   // XMMATRIX world =
      //  XMMatrixRotationY(mRotationAngle) *
		//XMMatrixRotationX(mRotationAngle * 0.5f); 
    
    XMMATRIX view = mCamera.GetViewMatrix();

    XMMATRIX projection = XMMatrixPerspectiveFovLH(
        XM_PIDIV4,
        1280.0f / 720.0f,
        0.1f,
        100.0f);

    const std::vector<SceneObject>& sceneObjects = mLevel.GetSceneObjects();

    for (int i = 0; i < sceneObjects.size(); i++)
    {
        SceneObject object = sceneObjects[i];

        XMMATRIX objectWorld =
            XMMatrixScaling(
                object.scale.x,
                object.scale.y,
                object.scale.z) *
            XMMatrixTranslation(
                object.position.x,
                object.position.y,
                object.position.z);

        DrawCube(objectWorld, view, projection);
    }

    mSwapChain->Present(1, 0);
}