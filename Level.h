#pragma once

#include <vector>
#include <DirectXMath.h>

using namespace DirectX;

struct SceneObject
{
    XMFLOAT3 position;
    XMFLOAT3 scale;
};

class Level
{
public:

    Level();

    void BuildLevel();

    const std::vector<SceneObject>& GetSceneObjects() const;

private:

    void AddMainCorridor();

    void AddLeftIllusionRoom();

    void AddRightIllusionRoom();

    void AddBox(XMFLOAT3 position, XMFLOAT3 scale);

private:

    std::vector<SceneObject> mSceneObjects;
};