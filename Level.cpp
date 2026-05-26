#include "Level.h"

Level::Level()
{

}

void Level::BuildLevel()
{
    mSceneObjects.clear();

    AddMainCorridor();

    AddLeftIllusionRoom();

    AddRightIllusionRoom();
}

void Level::AddBox(XMFLOAT3 position, XMFLOAT3 scale)
{
    SceneObject object;

    object.position = position;
    object.scale = scale;

    mSceneObjects.push_back(object);
}

void Level::AddMainCorridor()
{
    // Corridor shell with door gaps for both illusion rooms.

    // Floor tiles for the repeating hallway.
    for (int i = 0; i < 24; i++)
    {
        AddBox(
            XMFLOAT3(0.0f, -1.0f, i * 2.0f),
            XMFLOAT3(4.0f, 0.2f, 2.0f));
    }

    // Leave a gap for the left room entrance.
    for (int i = 0; i < 24; i++)
    {
        if (i == 5 || i == 6)
        {
            continue;
        }

        AddBox(
            XMFLOAT3(-2.2f, 0.0f, i * 2.0f),
            XMFLOAT3(0.2f, 2.0f, 2.0f));
    }

    // Leave a gap for the right room entrance.
    for (int i = 0; i < 24; i++)
    {
        if (i == 9 || i == 10)
        {
            continue;
        }

        AddBox(
            XMFLOAT3(2.2f, 0.0f, i * 2.0f),
            XMFLOAT3(0.2f, 2.0f, 2.0f));
    }

    // Close off the spawn side.
    AddBox(
        XMFLOAT3(0.0f, 0.0f, -2.0f),
        XMFLOAT3(4.0f, 2.0f, 0.2f));

    // Far blocker sells the endless-corridor trick.
    AddBox(
        XMFLOAT3(0.0f, 0.0f, 42.0f),
        XMFLOAT3(3.0f, 3.0f, 3.0f));
}

void Level::AddLeftIllusionRoom()
{
    // Left room used for fake-exit teleports.
    float roomCentreX = -7.1f;
    float roomCentreZ = 10.0f;

    // Main left-room floor slab.
    AddBox(
        XMFLOAT3(roomCentreX, -1.0f, roomCentreZ),
        XMFLOAT3(6.2f, 0.2f, 3.8f));

    // Split wall leaves a doorway-sized opening.
    AddBox(
        XMFLOAT3(-12.0f, 0.0f, roomCentreZ - 2.4f),
        XMFLOAT3(0.2f, 2.0f, 1.4f));
    AddBox(
        XMFLOAT3(-12.0f, 0.0f, roomCentreZ + 2.4f),
        XMFLOAT3(0.2f, 2.0f, 1.4f));

    // Back fake exit frame.
    AddBox(
        XMFLOAT3(-4.7f, 0.0f, 7.0f),
        XMFLOAT3(2.9f, 2.0f, 0.2f));
    AddBox(
        XMFLOAT3(-11.2f, 0.0f, 7.0f),
        XMFLOAT3(1.0f, 2.0f, 0.2f));

    // Front fake exit frame.
    AddBox(
        XMFLOAT3(-4.7f, 0.0f, 13.0f),
        XMFLOAT3(2.9f, 2.0f, 0.2f));
    AddBox(
        XMFLOAT3(-11.2f, 0.0f, 13.0f),
        XMFLOAT3(1.0f, 2.0f, 0.2f));

    // Short branch to hide trigger A.
    AddBox(
        XMFLOAT3(-9.0f, -1.0f, 5.4f),
        XMFLOAT3(1.5f, 0.2f, 1.4f));
    AddBox(
        XMFLOAT3(-10.5f, 0.0f, 5.4f),
        XMFLOAT3(0.2f, 1.8f, 1.4f));
    AddBox(
        XMFLOAT3(-7.5f, 0.0f, 5.4f),
        XMFLOAT3(0.2f, 1.8f, 1.4f));

    // Short branch to hide trigger B.
    AddBox(
        XMFLOAT3(-9.0f, -1.0f, 14.6f),
        XMFLOAT3(1.5f, 0.2f, 1.4f));
    AddBox(
        XMFLOAT3(-10.5f, 0.0f, 14.6f),
        XMFLOAT3(0.2f, 1.8f, 1.4f));
    AddBox(
        XMFLOAT3(-7.5f, 0.0f, 14.6f),
        XMFLOAT3(0.2f, 1.8f, 1.4f));

    // Extra side chamber makes this room less predictable.
    // Connector between main space and side chamber.
    AddBox(
        XMFLOAT3(-13.6f, -1.0f, roomCentreZ),
        XMFLOAT3(1.6f, 0.2f, 1.4f));
    AddBox(
        XMFLOAT3(-13.6f, 0.0f, roomCentreZ - 1.4f),
        XMFLOAT3(1.6f, 2.0f, 0.2f));
    AddBox(
        XMFLOAT3(-13.6f, 0.0f, roomCentreZ + 1.4f),
        XMFLOAT3(1.6f, 2.0f, 0.2f));

    // Side chamber footprint.
    AddBox(
        XMFLOAT3(-17.6f, -1.0f, roomCentreZ),
        XMFLOAT3(3.8f, 0.2f, 3.2f));
    AddBox(
        XMFLOAT3(-21.2f, 0.0f, roomCentreZ),
        XMFLOAT3(0.2f, 2.0f, 3.2f));
    // Keep a doorway so the side chamber stays traversable.
    AddBox(
        XMFLOAT3(-14.0f, 0.0f, roomCentreZ - 2.0f),
        XMFLOAT3(0.2f, 2.0f, 1.2f));
    AddBox(
        XMFLOAT3(-14.0f, 0.0f, roomCentreZ + 2.0f),
        XMFLOAT3(0.2f, 2.0f, 1.2f));
    AddBox(
        XMFLOAT3(-17.6f, 0.0f, roomCentreZ - 3.0f),
        XMFLOAT3(3.8f, 2.0f, 0.2f));
    AddBox(
        XMFLOAT3(-17.6f, 0.0f, roomCentreZ + 3.0f),
        XMFLOAT3(3.8f, 2.0f, 0.2f));
}

void Level::AddRightIllusionRoom()
{
    // Right room aligns flush with corridor wall to avoid a visible seam.

    float roomStartX = 2.2f;
    float roomEndX = 12.0f;
    float roomCentreX = 7.1f;

    float roomCentreZ = 18.0f;
    float roomBackZ = 15.0f;
    float roomFrontZ = 21.0f;

    // Floor runs straight from corridor into room.
    AddBox(
        XMFLOAT3(roomCentreX, -1.0f, roomCentreZ),
        XMFLOAT3(6.2f, 0.2f, 3.8f));

    // Hard stop on room outer edge.
    AddBox(
        XMFLOAT3(roomEndX, 0.0f, roomCentreZ),
        XMFLOAT3(0.2f, 2.0f, 3.8f));

    // Back wall keeps a center gap for the shifting doorway illusion.
    AddBox(
        XMFLOAT3(4.05f, 0.0f, roomBackZ),
        XMFLOAT3(1.85f, 2.0f, 0.2f));
    AddBox(
        XMFLOAT3(10.15f, 0.0f, roomBackZ),
        XMFLOAT3(1.85f, 2.0f, 0.2f));

    // Front wall mirrors the same doorway-gap setup.
    AddBox(
        XMFLOAT3(4.05f, 0.0f, roomFrontZ),
        XMFLOAT3(1.85f, 2.0f, 0.2f));
    AddBox(
        XMFLOAT3(10.15f, 0.0f, roomFrontZ),
        XMFLOAT3(1.85f, 2.0f, 0.2f));

    AddBox(
        XMFLOAT3(7.1f, -1.0f, 28.0f),
        XMFLOAT3(6.2f, 0.2f, 3.8f));

    AddBox(
        XMFLOAT3(7.1f, -1.0f, 24.5f),
        XMFLOAT3(3.0f, 0.2f, 3.5f));
    AddBox(
        XMFLOAT3(5.7f, 0.0f, 26.5f),
        XMFLOAT3(0.2f, 2.0f, 6.0f));
    AddBox(
        XMFLOAT3(8.5f, 0.0f, 26.5f),
        XMFLOAT3(0.2f, 2.0f, 6.0f));

    AddBox(
        XMFLOAT3(7.1f, -1.0f, 31.8f),
        XMFLOAT3(2.6f, 0.2f, 2.0f));
    AddBox(
        XMFLOAT3(5.9f, 0.0f, 31.8f),
        XMFLOAT3(0.2f, 2.0f, 2.0f));
    AddBox(
        XMFLOAT3(8.3f, 0.0f, 31.8f),
        XMFLOAT3(0.2f, 2.0f, 2.0f));

    AddBox(
        XMFLOAT3(7.1f, -1.0f, 36.2f),
        XMFLOAT3(5.2f, 0.2f, 3.2f));
    AddBox(
        XMFLOAT3(4.6f, 0.0f, 36.2f),
        XMFLOAT3(0.2f, 2.0f, 3.2f));
    AddBox(
        XMFLOAT3(9.6f, 0.0f, 36.2f),
        XMFLOAT3(0.2f, 2.0f, 3.2f));
    AddBox(
        XMFLOAT3(7.1f, 0.0f, 33.2f),
        XMFLOAT3(5.2f, 2.0f, 0.2f));
    AddBox(
        XMFLOAT3(7.1f, 0.0f, 39.2f),
        XMFLOAT3(5.2f, 2.0f, 0.2f));
}

const std::vector<SceneObject>& Level::GetSceneObjects() const
{
    return mSceneObjects;
}
