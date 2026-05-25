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

    XMFLOAT3 GetForwardDirection() const;

    void SetPosition(XMFLOAT3 position);

    void AddYaw(float deltaYaw);

    bool IsFlightModeEnabled() const;

private:

    XMFLOAT3 mPosition;
    XMFLOAT3 mTarget;
    XMFLOAT3 mUp;

    float mYaw;
    float mPitch;

    bool mIsFlightMode;
    bool mWasToggleKeyDown;
};
