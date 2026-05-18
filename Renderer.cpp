#include "Renderer.h"

#include <string>

Renderer::Renderer()
{
    mRotationAngle = 0.0f;
    mGameStarted = false;
    mGameEnded = false;
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



void Renderer::DrawHudGlyph(
    char character,
    float x,
    float y,
    float cellSize,
    XMMATRIX view,
    XMMATRIX projection)
{
    const char* rows[7] =
    {
        "     ",
        "     ",
        "     ",
        "     ",
        "     ",
        "     ",
        "     "
    };

    switch (character)
    {
    case 'A':
        rows[0] = " ### "; rows[1] = "#   #"; rows[2] = "#   #"; rows[3] = "#####"; rows[4] = "#   #"; rows[5] = "#   #"; rows[6] = "#   #";
        break;
    case 'B':
        rows[0] = "#### "; rows[1] = "#   #"; rows[2] = "#   #"; rows[3] = "#### "; rows[4] = "#   #"; rows[5] = "#   #"; rows[6] = "#### ";
        break;
    case 'C':
        rows[0] = " ####"; rows[1] = "#    "; rows[2] = "#    "; rows[3] = "#    "; rows[4] = "#    "; rows[5] = "#    "; rows[6] = " ####";
        break;
    case 'D':
        rows[0] = "#### "; rows[1] = "#   #"; rows[2] = "#   #"; rows[3] = "#   #"; rows[4] = "#   #"; rows[5] = "#   #"; rows[6] = "#### ";
        break;
    case 'E':
        rows[0] = "#####"; rows[1] = "#    "; rows[2] = "#    "; rows[3] = "#### "; rows[4] = "#    "; rows[5] = "#    "; rows[6] = "#####";
        break;
    case 'F':
        rows[0] = "#####"; rows[1] = "#    "; rows[2] = "#    "; rows[3] = "#### "; rows[4] = "#    "; rows[5] = "#    "; rows[6] = "#    ";
        break;
    case 'G':
        rows[0] = " ####"; rows[1] = "#    "; rows[2] = "#    "; rows[3] = "#  ##"; rows[4] = "#   #"; rows[5] = "#   #"; rows[6] = " ####";
        break;
    case 'H':
        rows[0] = "#   #"; rows[1] = "#   #"; rows[2] = "#   #"; rows[3] = "#####"; rows[4] = "#   #"; rows[5] = "#   #"; rows[6] = "#   #";
        break;
    case 'I':
        rows[0] = "#####"; rows[1] = "  #  "; rows[2] = "  #  "; rows[3] = "  #  "; rows[4] = "  #  "; rows[5] = "  #  "; rows[6] = "#####";
        break;
    case 'J':
        rows[0] = "#####"; rows[1] = "   # "; rows[2] = "   # "; rows[3] = "   # "; rows[4] = "   # "; rows[5] = "#  # "; rows[6] = " ##  ";
        break;
    case 'K':
        rows[0] = "#   #"; rows[1] = "#  # "; rows[2] = "# #  "; rows[3] = "##   "; rows[4] = "# #  "; rows[5] = "#  # "; rows[6] = "#   #";
        break;
    case 'L':
        rows[0] = "#    "; rows[1] = "#    "; rows[2] = "#    "; rows[3] = "#    "; rows[4] = "#    "; rows[5] = "#    "; rows[6] = "#####";
        break;
    case 'M':
        rows[0] = "#   #"; rows[1] = "## ##"; rows[2] = "# # #"; rows[3] = "#   #"; rows[4] = "#   #"; rows[5] = "#   #"; rows[6] = "#   #";
        break;
    case 'N':
        rows[0] = "#   #"; rows[1] = "##  #"; rows[2] = "# # #"; rows[3] = "#  ##"; rows[4] = "#   #"; rows[5] = "#   #"; rows[6] = "#   #";
        break;
    case 'O':
        rows[0] = " ### "; rows[1] = "#   #"; rows[2] = "#   #"; rows[3] = "#   #"; rows[4] = "#   #"; rows[5] = "#   #"; rows[6] = " ### ";
        break;
    case 'P':
        rows[0] = "#### "; rows[1] = "#   #"; rows[2] = "#   #"; rows[3] = "#### "; rows[4] = "#    "; rows[5] = "#    "; rows[6] = "#    ";
        break;
    case 'Q':
        rows[0] = " ### "; rows[1] = "#   #"; rows[2] = "#   #"; rows[3] = "#   #"; rows[4] = "# # #"; rows[5] = "#  # "; rows[6] = " ## #";
        break;
    case 'R':
        rows[0] = "#### "; rows[1] = "#   #"; rows[2] = "#   #"; rows[3] = "#### "; rows[4] = "# #  "; rows[5] = "#  # "; rows[6] = "#   #";
        break;
    case 'S':
        rows[0] = " ####"; rows[1] = "#    "; rows[2] = "#    "; rows[3] = " ### "; rows[4] = "    #"; rows[5] = "    #"; rows[6] = "#### ";
        break;
    case 'T':
        rows[0] = "#####"; rows[1] = "  #  "; rows[2] = "  #  "; rows[3] = "  #  "; rows[4] = "  #  "; rows[5] = "  #  "; rows[6] = "  #  ";
        break;
    case 'U':
        rows[0] = "#   #"; rows[1] = "#   #"; rows[2] = "#   #"; rows[3] = "#   #"; rows[4] = "#   #"; rows[5] = "#   #"; rows[6] = " ### ";
        break;
    case 'V':
        rows[0] = "#   #"; rows[1] = "#   #"; rows[2] = "#   #"; rows[3] = "#   #"; rows[4] = "#   #"; rows[5] = " # # "; rows[6] = "  #  ";
        break;
    case 'W':
        rows[0] = "#   #"; rows[1] = "#   #"; rows[2] = "#   #"; rows[3] = "# # #"; rows[4] = "# # #"; rows[5] = "## ##"; rows[6] = "#   #";
        break;
    case 'X':
        rows[0] = "#   #"; rows[1] = "#   #"; rows[2] = " # # "; rows[3] = "  #  "; rows[4] = " # # "; rows[5] = "#   #"; rows[6] = "#   #";
        break;
    case 'Y':
        rows[0] = "#   #"; rows[1] = "#   #"; rows[2] = " # # "; rows[3] = "  #  "; rows[4] = "  #  "; rows[5] = "  #  "; rows[6] = "  #  ";
        break;
    case 'Z':
        rows[0] = "#####"; rows[1] = "    #"; rows[2] = "   # "; rows[3] = "  #  "; rows[4] = " #   "; rows[5] = "#    "; rows[6] = "#####";
        break;
    case '0':
        rows[0] = " ### "; rows[1] = "#   #"; rows[2] = "#  ##"; rows[3] = "# # #"; rows[4] = "##  #"; rows[5] = "#   #"; rows[6] = " ### ";
        break;
    case '1':
        rows[0] = "  #  "; rows[1] = " ##  "; rows[2] = "  #  "; rows[3] = "  #  "; rows[4] = "  #  "; rows[5] = "  #  "; rows[6] = "#####";
        break;
    case '2':
        rows[0] = " ### "; rows[1] = "#   #"; rows[2] = "    #"; rows[3] = "   # "; rows[4] = "  #  "; rows[5] = " #   "; rows[6] = "#####";
        break;
    case '3':
        rows[0] = "#### "; rows[1] = "    #"; rows[2] = "    #"; rows[3] = " ### "; rows[4] = "    #"; rows[5] = "    #"; rows[6] = "#### ";
        break;
    default:
        break;
    }

    for (int row = 0; row < 7; row++)
    {
        for (int column = 0; column < 5; column++)
        {
            if (rows[row][column] == '#')
            {
                DrawHudSegment(
                    x + column * cellSize,
                    y - row * cellSize,
                    cellSize * 0.32f,
                    cellSize * 0.32f,
                    0.0f,
                    view,
                    projection);
            }
        }
    }
}

void Renderer::DrawHudText(
    const char* text,
    float centerX,
    float centerY,
    float cellSize,
    XMMATRIX view,
    XMMATRIX projection)
{
    int characterCount = 0;

    while (text[characterCount] != '\0')
    {
        characterCount++;
    }

    float characterStride = cellSize * 6.0f;
    float textWidth = characterCount * characterStride - cellSize;
    float startX = centerX - textWidth * 0.5f;
    float startY = centerY + cellSize * 3.0f;

    for (int i = 0; i < characterCount; i++)
    {
        DrawHudGlyph(
            text[i],
            startX + i * characterStride,
            startY,
            cellSize,
            view,
            projection);
    }
}

void Renderer::DrawStartScreen()
{
    XMMATRIX hudView = XMMatrixIdentity();
    XMMATRIX hudProjection = XMMatrixOrthographicLH(
        1280.0f,
        720.0f,
        0.0f,
        10.0f);

    mDeviceContext->OMSetDepthStencilState(mHudDepthStencilState.Get(), 0);

    DrawHudText("CLICK TO PLAY", 0.0f, 35.0f, 13.0f, hudView, hudProjection);
    DrawHudText("COLLECT ALL 3", 0.0f, -80.0f, 9.0f, hudView, hudProjection);

    mDeviceContext->OMSetDepthStencilState(nullptr, 0);
}

void Renderer::DrawEndScreen()
{
    XMMATRIX hudView = XMMatrixIdentity();
    XMMATRIX hudProjection = XMMatrixOrthographicLH(
        1280.0f,
        720.0f,
        0.0f,
        10.0f);

    mDeviceContext->OMSetDepthStencilState(mHudDepthStencilState.Get(), 0);

    DrawHudText("ALL COLLECTIBLES FOUND", 0.0f, 55.0f, 8.0f, hudView, hudProjection);
    DrawHudText("YOU WIN", 0.0f, -55.0f, 14.0f, hudView, hudProjection);

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

    if (!mGameStarted)
    {
        if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) ||
            (GetAsyncKeyState(VK_RETURN) & 0x8000))
        {
            mGameStarted = true;
        }
        else
        {
            DrawStartScreen();
            mSwapChain->Present(1, 0);
            return;
        }
    }

    if (!mGameEnded)
    {
        mCamera.Update();

        mGame.Update(mCamera);

        int collectedCount = mGame.GetCollectedCount();
        int totalCollectibles = static_cast<int>(mGame.GetCollectibles().size());

        if (totalCollectibles > 0 && collectedCount >= totalCollectibles)
        {
            mGameEnded = true;
        }
    }

    if (mGameEnded)
    {
        DrawEndScreen();
        mSwapChain->Present(1, 0);
        return;
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

    DrawHudCounter();

    mSwapChain->Present(1, 0);
}
