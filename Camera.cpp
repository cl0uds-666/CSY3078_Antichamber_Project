#include "Camera.h"

Camera::Camera()
{
    mPosition = XMFLOAT3(0.0f, 0.0f, 1.0f);
    mTarget = XMFLOAT3(0.0f, 0.0f, 2.0f);
    mUp = XMFLOAT3(0.0f, 1.0f, 0.0f);

    mYaw = 0.0f;
    mPitch = 0.0f;

    mIsFlightMode = false;
    mWasToggleKeyDown = false;
}

void Camera::Update()
{
    float moveSpeed = 0.05f;
    float turnSpeed = 0.03f;
    float pitchLimit = XM_PIDIV2 - 0.05f;

    bool isToggleKeyDown = (GetAsyncKeyState('F') & 0x8000) != 0;
    if (isToggleKeyDown && !mWasToggleKeyDown)
    {
        mIsFlightMode = !mIsFlightMode;
    }
    mWasToggleKeyDown = isToggleKeyDown;

    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        mYaw -= turnSpeed;
    }

    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        mYaw += turnSpeed;
    }

    if (mIsFlightMode)
    {
        if (GetAsyncKeyState(VK_UP) & 0x8000)
        {
            mPitch += turnSpeed;
        }

        if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        {
            mPitch -= turnSpeed;
        }

        if (mPitch > pitchLimit)
        {
            mPitch = pitchLimit;
        }

        if (mPitch < -pitchLimit)
        {
            mPitch = -pitchLimit;
        }
    }
    else
    {
        mPitch = 0.0f;
    }

    float cosPitch = cosf(mPitch);
    float forwardX = sinf(mYaw) * cosPitch;
    float forwardY = sinf(mPitch);
    float forwardZ = cosf(mYaw) * cosPitch;

    float rightX = cosf(mYaw);
    float rightZ = -sinf(mYaw);

    if (GetAsyncKeyState('W') & 0x8000)
    {
        mPosition.x += forwardX * moveSpeed;
        mPosition.y += forwardY * moveSpeed;
        mPosition.z += forwardZ * moveSpeed;
    }

    if (GetAsyncKeyState('S') & 0x8000)
    {
        mPosition.x -= forwardX * moveSpeed;
        mPosition.y -= forwardY * moveSpeed;
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
    mTarget.y = mPosition.y + forwardY;
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
        sinf(mYaw) * cosf(mPitch),
        sinf(mPitch),
        cosf(mYaw) * cosf(mPitch));
}

void Camera::SetPosition(XMFLOAT3 position)
{
    mPosition = position;

    float forwardX = sinf(mYaw) * cosf(mPitch);
    float forwardY = sinf(mPitch);
    float forwardZ = cosf(mYaw) * cosf(mPitch);

    mTarget.x = mPosition.x + forwardX;
    mTarget.y = mPosition.y + forwardY;
    mTarget.z = mPosition.z + forwardZ;
}
void Camera::AddYaw(float deltaYaw)
{
    mYaw += deltaYaw;

    float forwardX = sinf(mYaw) * cosf(mPitch);
    float forwardY = sinf(mPitch);
    float forwardZ = cosf(mYaw) * cosf(mPitch);

    mTarget.x = mPosition.x + forwardX;
    mTarget.y = mPosition.y + forwardY;
    mTarget.z = mPosition.z + forwardZ;
}

bool Camera::IsFlightModeEnabled() const
{
    return mIsFlightMode;
}
