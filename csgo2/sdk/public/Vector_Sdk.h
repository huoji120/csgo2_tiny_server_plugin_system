#pragma once
#include "../sdk.h"

//cancer fix me
struct Vector_SimPleSdk
{
public:
    float x, y, z;

    Vector_SimPleSdk()
    {}

    Vector_SimPleSdk(float x_, float y_, float z_)
    {
        x = x_;
        y = y_;
        z = z_;
    }
};
struct Vector2D
{
public:
    float x, y;

    Vector2D()
    {}

    Vector2D(float x_, float y_)
    {
        x = x_;
        y = y_;
    }
};
struct Vector4D
{
public:
    float x, y, z, c;

    Vector4D()
    {}

    Vector4D(float x_, float y_, float z_, float c_)
    {
        x = x_;
        y = y_;
        z = z_;
        c = c_;
    }
};