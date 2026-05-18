#pragma once

#include <windows.h>
#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:

    Camera();

    void Update();

    XMMATRIX GetViewMatrix();

    XMFLOAT3 GetPosition() const;

    void SetPosition(XMFLOAT3 position);

    void AddYaw(float deltaYaw);

private:

    XMFLOAT3 mPosition;
    XMFLOAT3 mTarget;
    XMFLOAT3 mUp;

    float mYaw;
};