struct pixelIn
{
    float4 position : SV_POSITION;
    float4 colour : COLOR;
};

float4 main(pixelIn input) : SV_TARGET
{
    return input.colour;
}
