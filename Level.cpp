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
    // Main corridor dimensions:
    // Corridor centre = x 0
    // Corridor walls = x -2.2 and x 2.2
    // Left room entrance = around z 10
    // Right room entrance = around z 18

    // Floor pieces down the corridor
    for (int i = 0; i < 24; i++)
    {
        AddBox(
            XMFLOAT3(0.0f, -1.0f, i * 2.0f),
            XMFLOAT3(4.0f, 0.2f, 2.0f));
    }

    // Left corridor wall, with a doorway gap at z 10-12
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

    // Right corridor wall, with a doorway gap at z 18-20
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

    // Back wall behind the player
    AddBox(
        XMFLOAT3(0.0f, 0.0f, -2.0f),
        XMFLOAT3(4.0f, 2.0f, 0.2f));

    // Visible block far down the corridor.
    // The teleport trigger should stay before this, so the end always looks far away.
    AddBox(
        XMFLOAT3(0.0f, 0.0f, 42.0f),
        XMFLOAT3(3.0f, 3.0f, 3.0f));
}

void Level::AddLeftIllusionRoom()
{
    // LEFT ROOM MATH:
    // Corridor left wall is at x = -2.2
    // Room extends left to x = -12.0
    // Room centre x = (-2.2 + -12.0) / 2 = -7.1
    // Room centre z = 10.0
    //
    // Important:
    // The room starts EXACTLY from the corridor wall at x = -2.2.
    // No floating gap.

    float roomStartX = -2.2f;
    float roomEndX = -12.0f;
    float roomCentreX = -7.1f;

    float roomCentreZ = 10.0f;
    float roomBackZ = 7.0f;
    float roomFrontZ = 13.0f;

    // Floor connecting directly from corridor wall into the room
    AddBox(
        XMFLOAT3(roomCentreX, -1.0f, roomCentreZ),
        XMFLOAT3(6.2f, 0.2f, 3.8f));

    // Outer left wall
    AddBox(
        XMFLOAT3(roomEndX, 0.0f, roomCentreZ),
        XMFLOAT3(0.2f, 2.0f, 3.8f));

    // Back wall starts at corridor wall and runs to outer wall
    AddBox(
        XMFLOAT3(roomCentreX, 0.0f, roomBackZ),
        XMFLOAT3(6.2f, 2.0f, 0.2f));

    // Front wall starts at corridor wall and runs to outer wall
    AddBox(
        XMFLOAT3(roomCentreX, 0.0f, roomFrontZ),
        XMFLOAT3(6.2f, 2.0f, 0.2f));

    // Small visual block inside the room so it does not feel empty
    AddBox(
        XMFLOAT3(-9.5f, -0.2f, 10.0f),
        XMFLOAT3(0.8f, 0.8f, 0.8f));
}

void Level::AddRightIllusionRoom()
{
    // RIGHT ROOM MATH:
    // Corridor right wall is at x = 2.2
    // Room extends right to x = 12.0
    // Room centre x = (2.2 + 12.0) / 2 = 7.1
    // Room centre z = 18.0
    //
    // Important:
    // The room starts EXACTLY from the corridor wall at x = 2.2.
    // No floating gap.

    float roomStartX = 2.2f;
    float roomEndX = 12.0f;
    float roomCentreX = 7.1f;

    float roomCentreZ = 18.0f;
    float roomBackZ = 15.0f;
    float roomFrontZ = 21.0f;

    // Floor connecting directly from corridor wall into the room
    AddBox(
        XMFLOAT3(roomCentreX, -1.0f, roomCentreZ),
        XMFLOAT3(6.2f, 0.2f, 3.8f));

    // Outer right wall
    AddBox(
        XMFLOAT3(roomEndX, 0.0f, roomCentreZ),
        XMFLOAT3(0.2f, 2.0f, 3.8f));

    // Back wall starts at corridor wall and runs to outer wall
    AddBox(
        XMFLOAT3(roomCentreX, 0.0f, roomBackZ),
        XMFLOAT3(6.2f, 2.0f, 0.2f));

    // Front wall starts at corridor wall and runs to outer wall
    AddBox(
        XMFLOAT3(roomCentreX, 0.0f, roomFrontZ),
        XMFLOAT3(6.2f, 2.0f, 0.2f));

    // Extra back room for the two-entrance illusion later
    AddBox(
        XMFLOAT3(7.1f, -1.0f, 28.0f),
        XMFLOAT3(6.2f, 0.2f, 3.8f));

    // Connector from right room to extra room
    AddBox(
        XMFLOAT3(7.1f, -1.0f, 24.5f),
        XMFLOAT3(3.0f, 0.2f, 3.5f));
}

const std::vector<SceneObject>& Level::GetSceneObjects() const
{
    return mSceneObjects;
}