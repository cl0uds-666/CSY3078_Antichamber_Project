#include "Renderer.h"

#include <string>
#include <iostream>
#include <sstream>

namespace
{
const char** GetGlyphRows(char character)
{
    static const char* blank[7] =
    {
        "00000",
        "00000",
        "00000",
        "00000",
        "00000",
        "00000",
        "00000"
    };

    static const char* a[7] =
    {
        "01110",
        "10001",
        "10001",
        "11111",
        "10001",
        "10001",
        "10001"
    };

    static const char* b[7] =
    {
        "11110",
        "10001",
        "10001",
        "11110",
        "10001",
        "10001",
        "11110"
    };

    static const char* c[7] =
    {
        "01111",
        "10000",
        "10000",
        "10000",
        "10000",
        "10000",
        "01111"
    };

    static const char* d[7] =
    {
        "11110",
        "10001",
        "10001",
        "10001",
        "10001",
        "10001",
        "11110"
    };

    static const char* e[7] =
    {
        "11111",
        "10000",
        "10000",
        "11110",
        "10000",
        "10000",
        "11111"
    };

    static const char* f[7] =
    {
        "11111",
        "10000",
        "10000",
        "11110",
        "10000",
        "10000",
        "10000"
    };

    static const char* h[7] =
    {
        "10001",
        "10001",
        "10001",
        "11111",
        "10001",
        "10001",
        "10001"
    };

    static const char* i[7] =
    {
        "11111",
        "00100",
        "00100",
        "00100",
        "00100",
        "00100",
        "11111"
    };

    static const char* k[7] =
    {
        "10001",
        "10010",
        "10100",
        "11000",
        "10100",
        "10010",
        "10001"
    };

    static const char* l[7] =
    {
        "10000",
        "10000",
        "10000",
        "10000",
        "10000",
        "10000",
        "11111"
    };

    static const char* m[7] =
    {
        "10001",
        "11011",
        "10101",
        "10101",
        "10001",
        "10001",
        "10001"
    };

    static const char* n[7] =
    {
        "10001",
        "11001",
        "10101",
        "10011",
        "10001",
        "10001",
        "10001"
    };

    static const char* o[7] =
    {
        "01110",
        "10001",
        "10001",
        "10001",
        "10001",
        "10001",
        "01110"
    };

    static const char* p[7] =
    {
        "11110",
        "10001",
        "10001",
        "11110",
        "10000",
        "10000",
        "10000"
    };

    static const char* r[7] =
    {
        "11110",
        "10001",
        "10001",
        "11110",
        "10100",
        "10010",
        "10001"
    };

    static const char* s[7] =
    {
        "01111",
        "10000",
        "10000",
        "01110",
        "00001",
        "00001",
        "11110"
    };

    static const char* t[7] =
    {
        "11111",
        "00100",
        "00100",
        "00100",
        "00100",
        "00100",
        "00100"
    };

    static const char* u[7] =
    {
        "10001",
        "10001",
        "10001",
        "10001",
        "10001",
        "10001",
        "01110"
    };

    static const char* w[7] =
    {
        "10001",
        "10001",
        "10001",
        "10101",
        "10101",
        "10101",
        "01010"
    };

    static const char* y[7] =
    {
        "10001",
        "10001",
        "01010",
        "00100",
        "00100",
        "00100",
        "00100"
    };

    if (character >= 'a' && character <= 'z')
    {
        character = static_cast<char>(character - 'a' + 'A');
    }

    switch (character)
    {
    case 'A': return a;
    case 'B': return b;
    case 'C': return c;
    case 'D': return d;
    case 'E': return e;
    case 'F': return f;
    case 'H': return h;
    case 'I': return i;
    case 'K': return k;
    case 'L': return l;
    case 'M': return m;
    case 'N': return n;
    case 'O': return o;
    case 'P': return p;
    case 'R': return r;
    case 'S': return s;
    case 'T': return t;
    case 'U': return u;
    case 'W': return w;
    case 'Y': return y;
    default: return blank;
    }
}
}

Renderer::Renderer()
{
    mRotationAngle = 0.0f;
    mScreenState = ScreenState::Start;
    mWasMouseDown = false;
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

    if (!CreateHudDepthStencilState())
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

    mGame.SetLevel(&mLevel);

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


bool Renderer::CreateHudDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

    depthStencilDesc.DepthEnable = FALSE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    depthStencilDesc.StencilEnable = FALSE;

    HRESULT hr = mDevice->CreateDepthStencilState(
        &depthStencilDesc,
        mHudDepthStencilState.GetAddressOf());

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool Renderer::CreateShaders()
{
    const auto logShaderCompileError = [](const wchar_t* shaderPath, const char* shaderModel, HRESULT hr, ID3DBlob* errorBlob)
    {
        std::ostringstream message;
        message << "Failed to compile shader ";
        message << std::string(shaderModel);
        message << " from path: ";

        if (shaderPath != nullptr)
        {
            std::wstring wPath(shaderPath);
            message << std::string(wPath.begin(), wPath.end());
        }
        else
        {
            message << "<null>";
        }

        message << " (HRESULT=0x" << std::hex << static_cast<unsigned long>(hr) << std::dec << ")";

        if (errorBlob != nullptr && errorBlob->GetBufferPointer() != nullptr)
        {
            message << "\n";
            message << static_cast<const char*>(errorBlob->GetBufferPointer());
        }

        message << "\n";

        OutputDebugStringA(message.str().c_str());
        std::cerr << message.str();
    };

    ComPtr<ID3DBlob> vertexShaderBlob;
    ComPtr<ID3DBlob> pixelShaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    constexpr wchar_t kVertexShaderPath[] = L"shaders/VertexShader.hlsl";
    constexpr wchar_t kPixelShaderPath[] = L"shaders/PixelShader.hlsl";

    UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
    compileFlags |= D3DCOMPILE_DEBUG;
    compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr = D3DCompileFromFile(
        kVertexShaderPath,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "vs_5_0",
        compileFlags,
        0,
        vertexShaderBlob.GetAddressOf(),
        errorBlob.GetAddressOf());

    if (FAILED(hr))
    {
        logShaderCompileError(kVertexShaderPath, "vs_5_0", hr, errorBlob.Get());
        return false;
    }

    errorBlob.Reset();

    hr = D3DCompileFromFile(
        kPixelShaderPath,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "ps_5_0",
        compileFlags,
        0,
        pixelShaderBlob.GetAddressOf(),
        errorBlob.GetAddressOf());

    if (FAILED(hr))
    {
        logShaderCompileError(kPixelShaderPath, "ps_5_0", hr, errorBlob.Get());
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
            24,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        {
            "NORMAL",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            12,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
    };

    hr = mDevice->CreateInputLayout(
        layout,
        3,
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
        { -0.8f, -0.8f, -0.8f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
        { -0.8f,  0.8f, -0.8f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
        {  0.8f,  0.8f, -0.8f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
        {  0.8f, -0.8f, -0.8f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f },

        { -0.8f, -0.8f,  0.8f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
        {  0.8f, -0.8f,  0.8f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
        {  0.8f,  0.8f,  0.8f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
        { -0.8f,  0.8f,  0.8f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f },

        { -0.8f,  0.8f, -0.8f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f },
        { -0.8f,  0.8f,  0.8f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f },
        {  0.8f,  0.8f,  0.8f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f },
        {  0.8f,  0.8f, -0.8f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f },

        { -0.8f, -0.8f, -0.8f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, 0.0f, 1.0f },
        {  0.8f, -0.8f, -0.8f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, 0.0f, 1.0f },
        {  0.8f, -0.8f,  0.8f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, 0.0f, 1.0f },
        { -0.8f, -0.8f,  0.8f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, 0.0f, 1.0f },

        { -0.8f, -0.8f, -0.8f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f },
        { -0.8f, -0.8f,  0.8f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f },
        { -0.8f,  0.8f,  0.8f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f },
        { -0.8f,  0.8f, -0.8f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f },

        {  0.8f, -0.8f, -0.8f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f },
        {  0.8f,  0.8f, -0.8f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f },
        {  0.8f,  0.8f,  0.8f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f },
        {  0.8f, -0.8f,  0.8f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f }
    };

    unsigned int indices[] =
    {
        0, 1, 2, 0, 2, 3,
        4, 6, 5, 4, 7, 6,
        8, 9, 10, 8, 10, 11,
        12, 14, 13, 12, 15, 14,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23
    };

    D3D11_BUFFER_DESC vertexBufferDesc = {};

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * 24;
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
    cb.world = XMMatrixTranspose(world);
    cb.lightDirection = XMFLOAT3(-0.4f, -1.0f, -0.2f);
    cb.padding0 = 0.0f;
    cb.lightColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
    cb.padding1 = 0.0f;
    cb.ambientColor = XMFLOAT3(0.2f, 0.2f, 0.25f);
    cb.padding2 = 0.0f;

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

    mDeviceContext->PSSetConstantBuffers(
        0,
        1,
        mConstantBuffer.GetAddressOf());

    mDeviceContext->DrawIndexed(36, 0, 0);
}


void Renderer::DrawHudGlyph(
    char character,
    float x,
    float y,
    float pixelSize,
    XMMATRIX view,
    XMMATRIX projection)
{
    const char** glyphRows = GetGlyphRows(character);
    float blockScale = pixelSize * 0.45f;

    for (int row = 0; row < 7; row++)
    {
        for (int column = 0; column < 5; column++)
        {
            if (glyphRows[row][column] != '1')
            {
                continue;
            }

            DrawHudSegment(
                x + column * pixelSize,
                y - row * pixelSize,
                blockScale,
                blockScale,
                0.0f,
                view,
                projection);
        }
    }
}

void Renderer::DrawHudText(
    const char* text,
    float centreX,
    float centreY,
    float pixelSize,
    XMMATRIX view,
    XMMATRIX projection)
{
    float textWidth = 0.0f;

    for (int i = 0; text[i] != '\0'; i++)
    {
        if (text[i] == ' ')
        {
            textWidth += pixelSize * 3.0f;
        }
        else
        {
            textWidth += pixelSize * 6.0f;
        }
    }

    float cursorX = centreX - textWidth * 0.5f + pixelSize * 0.5f;
    float startY = centreY + pixelSize * 3.0f;

    for (int i = 0; text[i] != '\0'; i++)
    {
        if (text[i] == ' ')
        {
            cursorX += pixelSize * 3.0f;
            continue;
        }

        DrawHudGlyph(text[i], cursorX, startY, pixelSize, view, projection);
        cursorX += pixelSize * 6.0f;
    }
}

void Renderer::DrawScreenOverlay(
    const char* title,
    const char* subtitle)
{
    XMMATRIX hudView = XMMatrixIdentity();
    XMMATRIX hudProjection = XMMatrixOrthographicLH(
        1280.0f,
        720.0f,
        0.0f,
        10.0f);

    mDeviceContext->OMSetDepthStencilState(mHudDepthStencilState.Get(), 0);

    DrawHudText(title, 0.0f, 75.0f, 16.0f, hudView, hudProjection);
    DrawHudText(subtitle, 0.0f, -75.0f, 10.0f, hudView, hudProjection);

    mDeviceContext->OMSetDepthStencilState(nullptr, 0);
}



void Renderer::DrawHudSegment(
    float x,
    float y,
    float scaleX,
    float scaleY,
    float rotation,
    XMMATRIX view,
    XMMATRIX projection)
{
    XMMATRIX world =
        XMMatrixScaling(scaleX, scaleY, 0.04f) *
        XMMatrixRotationZ(rotation) *
        XMMatrixTranslation(x, y, 1.0f);

    DrawCube(world, view, projection);
}

void Renderer::DrawHudDigit(
    int digit,
    float x,
    float y,
    XMMATRIX view,
    XMMATRIX projection)
{
    bool segments[10][7] =
    {
        { true,  true,  true,  true,  true,  true,  false },
        { false, true,  true,  false, false, false, false },
        { true,  true,  false, true,  true,  false, true  },
        { true,  true,  true,  true,  false, false, true  },
        { false, true,  true,  false, false, true,  true  },
        { true,  false, true,  true,  false, true,  true  },
        { true,  false, true,  true,  true,  true,  true  },
        { true,  true,  true,  false, false, false, false },
        { true,  true,  true,  true,  true,  true,  true  },
        { true,  true,  true,  true,  false, true,  true  }
    };

    if (digit < 0 || digit > 9)
    {
        return;
    }

    float horizontalScaleX = 18.0f;
    float horizontalScaleY = 3.0f;
    float verticalScaleX = 3.0f;
    float verticalScaleY = 13.0f;

    if (segments[digit][0])
    {
        DrawHudSegment(x, y + 22.0f, horizontalScaleX, horizontalScaleY, 0.0f, view, projection);
    }

    if (segments[digit][1])
    {
        DrawHudSegment(x + 18.0f, y + 11.0f, verticalScaleX, verticalScaleY, 0.0f, view, projection);
    }

    if (segments[digit][2])
    {
        DrawHudSegment(x + 18.0f, y - 11.0f, verticalScaleX, verticalScaleY, 0.0f, view, projection);
    }

    if (segments[digit][3])
    {
        DrawHudSegment(x, y - 22.0f, horizontalScaleX, horizontalScaleY, 0.0f, view, projection);
    }

    if (segments[digit][4])
    {
        DrawHudSegment(x - 18.0f, y - 11.0f, verticalScaleX, verticalScaleY, 0.0f, view, projection);
    }

    if (segments[digit][5])
    {
        DrawHudSegment(x - 18.0f, y + 11.0f, verticalScaleX, verticalScaleY, 0.0f, view, projection);
    }

    if (segments[digit][6])
    {
        DrawHudSegment(x, y, horizontalScaleX, horizontalScaleY, 0.0f, view, projection);
    }
}

void Renderer::DrawHudCounter()
{
    int collectedCount = mGame.GetCollectedCount();
    int totalCollectibles = static_cast<int>(mGame.GetCollectibles().size());

    XMMATRIX hudView = XMMatrixIdentity();
    XMMATRIX hudProjection = XMMatrixOrthographicLH(
        1280.0f,
        720.0f,
        0.0f,
        10.0f);

    mDeviceContext->OMSetDepthStencilState(mHudDepthStencilState.Get(), 0);

    // Top-left HUD counter: collected / total.
    DrawHudDigit(collectedCount, -575.0f, 315.0f, hudView, hudProjection);
    DrawHudSegment(-535.0f, 315.0f, 3.0f, 22.0f, -0.45f, hudView, hudProjection);
    DrawHudDigit(totalCollectibles, -495.0f, 315.0f, hudView, hudProjection);

    mDeviceContext->OMSetDepthStencilState(nullptr, 0);
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

    bool isMouseDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    bool wasClicked = isMouseDown && !mWasMouseDown;
    mWasMouseDown = isMouseDown;

    if (mScreenState == ScreenState::Start)
    {
        if (wasClicked)
        {
            mScreenState = ScreenState::Playing;
        }
        else
        {
            DrawScreenOverlay("CLICK TO PLAY", "COLLECT THEM ALL");
            mSwapChain->Present(1, 0);
            return;
        }
    }

    if (mScreenState == ScreenState::Playing)
    {
        mCamera.Update();

        mGame.Update(mCamera);

        if (mGame.GetCollectedCount() == static_cast<int>(mGame.GetCollectibles().size()))
        {
            mScreenState = ScreenState::End;
        }
    }

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

    const std::vector<Collectible>& collectibles = mGame.GetCollectibles();

    for (int i = 0; i < collectibles.size(); i++)
    {
        Collectible collectible = collectibles[i];

        if (!collectible.isSpawned || collectible.isCollected)
        {
            continue;
        }

        XMMATRIX collectibleWorld =
            XMMatrixScaling(0.35f, 0.35f, 0.35f) *
            XMMatrixTranslation(
                collectible.position.x,
                collectible.position.y,
                collectible.position.z);

        DrawCube(collectibleWorld, view, projection);
    }

    const std::vector<SceneObject>& room3LayoutProps = mGame.GetRoom3LayoutProps();

    for (int i = 0; i < room3LayoutProps.size(); i++)
    {
        SceneObject object = room3LayoutProps[i];

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

    if (mScreenState == ScreenState::End)
    {
        DrawScreenOverlay("ALL COLLECTED", "YOU FOUND THEM ALL");
    }
    else
    {
        DrawHudCounter();
    }

    mSwapChain->Present(1, 0);
}
