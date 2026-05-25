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
    // Left room attached directly to corridor wall at x = -2.2
    float roomCentreX = -7.1f;
    float roomCentreZ = 10.0f;

    // Main room floor (touches corridor doorway side)
    AddBox(
        XMFLOAT3(roomCentreX, -1.0f, roomCentreZ),
        XMFLOAT3(6.2f, 0.2f, 3.8f));

    // Outer left wall with a doorway to the connector into the extra side chamber.
    AddBox(
        XMFLOAT3(-12.0f, 0.0f, roomCentreZ - 2.4f),
        XMFLOAT3(0.2f, 2.0f, 1.4f));
    AddBox(
        XMFLOAT3(-12.0f, 0.0f, roomCentreZ + 2.4f),
        XMFLOAT3(0.2f, 2.0f, 1.4f));

    // Back wall with a centred doorway (fake exit A)
    AddBox(
        XMFLOAT3(-4.7f, 0.0f, 7.0f),
        XMFLOAT3(2.9f, 2.0f, 0.2f));
    AddBox(
        XMFLOAT3(-11.2f, 0.0f, 7.0f),
        XMFLOAT3(1.0f, 2.0f, 0.2f));

    // Front wall with a centred doorway (fake exit B)
    AddBox(
        XMFLOAT3(-4.7f, 0.0f, 13.0f),
        XMFLOAT3(2.9f, 2.0f, 0.2f));
    AddBox(
        XMFLOAT3(-11.2f, 0.0f, 13.0f),
        XMFLOAT3(1.0f, 2.0f, 0.2f));

    // Fake mini corridor A (behind back doorway)
    AddBox(
        XMFLOAT3(-9.0f, -1.0f, 5.4f),
        XMFLOAT3(1.5f, 0.2f, 1.4f));
    AddBox(
        XMFLOAT3(-10.5f, 0.0f, 5.4f),
        XMFLOAT3(0.2f, 1.8f, 1.4f));
    AddBox(
        XMFLOAT3(-7.5f, 0.0f, 5.4f),
        XMFLOAT3(0.2f, 1.8f, 1.4f));

    // Fake mini corridor B (beyond front doorway)
    AddBox(
        XMFLOAT3(-9.0f, -1.0f, 14.6f),
        XMFLOAT3(1.5f, 0.2f, 1.4f));
    AddBox(
        XMFLOAT3(-10.5f, 0.0f, 14.6f),
        XMFLOAT3(0.2f, 1.8f, 1.4f));
    AddBox(
        XMFLOAT3(-7.5f, 0.0f, 14.6f),
        XMFLOAT3(0.2f, 1.8f, 1.4f));

    // Distinct side chamber on the left branch to clearly add another navigable space.
    // Connector corridor from main left room into the new chamber.
    AddBox(
        XMFLOAT3(-13.6f, -1.0f, roomCentreZ),
        XMFLOAT3(1.6f, 0.2f, 1.4f));
    AddBox(
        XMFLOAT3(-13.6f, 0.0f, roomCentreZ - 1.4f),
        XMFLOAT3(1.6f, 2.0f, 0.2f));
    AddBox(
        XMFLOAT3(-13.6f, 0.0f, roomCentreZ + 1.4f),
        XMFLOAT3(1.6f, 2.0f, 0.2f));

    // New side chamber (copy-like footprint, offset further left).
    AddBox(
        XMFLOAT3(-17.6f, -1.0f, roomCentreZ),
        XMFLOAT3(3.8f, 0.2f, 3.2f));
    AddBox(
        XMFLOAT3(-21.2f, 0.0f, roomCentreZ),
        XMFLOAT3(0.2f, 2.0f, 3.2f));
    // Inner chamber wall with a doorway aligned to the connector so it is accessible.
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

    // Back wall starts at corridor wall and runs to outer wall.
    // Leave a central doorway-sized gap that Room 3 can either melt open
    // or reseal with a dynamic door slab.
    AddBox(
        XMFLOAT3(4.05f, 0.0f, roomBackZ),
        XMFLOAT3(1.85f, 2.0f, 0.2f));
    AddBox(
        XMFLOAT3(10.15f, 0.0f, roomBackZ),
        XMFLOAT3(1.85f, 2.0f, 0.2f));

    // Front wall uses the same split so the original corridor entrance can
    // appear to shift onto this wall instead of becoming a floating block.
    AddBox(
        XMFLOAT3(4.05f, 0.0f, roomFrontZ),
        XMFLOAT3(1.85f, 2.0f, 0.2f));
    AddBox(
        XMFLOAT3(10.15f, 0.0f, roomFrontZ),
        XMFLOAT3(1.85f, 2.0f, 0.2f));

    // Extra back room for the two-entrance illusion later
    AddBox(
        XMFLOAT3(7.1f, -1.0f, 28.0f),
        XMFLOAT3(6.2f, 0.2f, 3.8f));

    // Connector from right room to extra room
    AddBox(
        XMFLOAT3(7.1f, -1.0f, 24.5f),
        XMFLOAT3(3.0f, 0.2f, 3.5f));
    AddBox(
        XMFLOAT3(5.7f, 0.0f, 24.5f),
        XMFLOAT3(0.2f, 2.0f, 3.5f));
    AddBox(
        XMFLOAT3(8.5f, 0.0f, 24.5f),
        XMFLOAT3(0.2f, 2.0f, 3.5f));

    // Distinct far chamber beyond the extra room to ensure an unambiguous map size.
    // Explicit connector segment between extra room and far chamber.
    AddBox(
        XMFLOAT3(7.1f, -1.0f, 31.8f),
        XMFLOAT3(2.6f, 0.2f, 2.0f));
    AddBox(
        XMFLOAT3(5.9f, 0.0f, 31.8f),
        XMFLOAT3(0.2f, 2.0f, 2.0f));
    AddBox(
        XMFLOAT3(8.3f, 0.0f, 31.8f),
        XMFLOAT3(0.2f, 2.0f, 2.0f));

    // New far chamber (separate, clearly distinct room volume).
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
