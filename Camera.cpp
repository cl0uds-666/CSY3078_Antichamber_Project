#include "Camera.h"

Camera::Camera()
{
    mPosition = XMFLOAT3(0.0f, 0.0f, 1.0f);
    mTarget = XMFLOAT3(0.0f, 0.0f, 2.0f);
    mUp = XMFLOAT3(0.0f, 1.0f, 0.0f);

    mYaw = 0.0f;
}

void Camera::Update()
{
    float moveSpeed = 0.05f;
    float turnSpeed = 0.03f;

    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        mYaw -= turnSpeed;
    }

    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        mYaw += turnSpeed;
    }

    float forwardX = sinf(mYaw);
    float forwardZ = cosf(mYaw);

    float rightX = cosf(mYaw);
    float rightZ = -sinf(mYaw);

    if (GetAsyncKeyState('W') & 0x8000)
    {
        mPosition.x += forwardX * moveSpeed;
        mPosition.z += forwardZ * moveSpeed;
    }

    if (GetAsyncKeyState('S') & 0x8000)
    {
        mPosition.x -= forwardX * moveSpeed;
        mPosition.z -= forwardZ * moveSpeed;
    }

    if (GetAsyncKeyState('A') & 0x8000)
    {
        mPosition.x -= rightX * moveSpeed;
        mPosition.z -= rightZ * moveSpeed;
    }

    if (GetAsyncKeyState('D') & 0x8000)
    {
        mPosition.x += rightX * moveSpeed;
        mPosition.z += rightZ * moveSpeed;
    }

    mTarget.x = mPosition.x + forwardX;
    mTarget.y = mPosition.y;
    mTarget.z = mPosition.z + forwardZ;
}

XMMATRIX Camera::GetViewMatrix()
{
    return XMMatrixLookAtLH(
        XMLoadFloat3(&mPosition),
        XMLoadFloat3(&mTarget),
        XMLoadFloat3(&mUp));
}

XMFLOAT3 Camera::GetPosition() const
{
    return mPosition;
}

XMFLOAT3 Camera::GetForwardDirection() const
{
    return XMFLOAT3(
        sinf(mYaw),
        0.0f,
        cosf(mYaw));
}

void Camera::SetPosition(XMFLOAT3 position)
{
    mPosition = position;

    float forwardX = sinf(mYaw);
    float forwardZ = cosf(mYaw);

    mTarget.x = mPosition.x + forwardX;
    mTarget.y = mPosition.y;
    mTarget.z = mPosition.z + forwardZ;
}
void Camera::AddYaw(float deltaYaw)
{
    mYaw += deltaYaw;

    float forwardX = sinf(mYaw);
    float forwardZ = cosf(mYaw);

    mTarget.x = mPosition.x + forwardX;
    mTarget.y = mPosition.y;
    mTarget.z = mPosition.z + forwardZ;
}
