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

struct pixelIn
{
    float4 position : SV_POSITION;
    float3 worldNormal : NORMAL;
    float4 colour : COLOR;
};

float4 main(pixelIn input) : SV_TARGET
{
    float3 normal = normalize(input.worldNormal);
    float3 toLight = normalize(-lightDirection);
    float lambert = max(dot(normal, toLight), 0.0f);

    float3 litColor = input.colour.rgb * (ambientColor + (lightColor * lambert));
    return float4(saturate(litColor), input.colour.a);
}
