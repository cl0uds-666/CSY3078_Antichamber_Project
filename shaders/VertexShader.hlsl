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
