cbuffer ConstantBuffer : register(b0)
{
    matrix wvp;
    matrix world;
    float3 lightDirection;
    float padding0;
    float3 lightColor;
    float padding1;
    float3 ambientColor;
    float padding2;
};

struct vertexIn
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 colour : COLOR;
};

struct vertexOut
{
    float4 position : SV_POSITION;
    float3 worldNormal : NORMAL;
    float4 colour : COLOR;
};

vertexOut main(vertexIn input)
{
    vertexOut output;

    output.position = mul(float4(input.position, 1.0f), wvp);
    output.worldNormal = normalize(mul(float4(input.normal, 0.0f), world).xyz);
    output.colour = input.colour;

    return output;
}
