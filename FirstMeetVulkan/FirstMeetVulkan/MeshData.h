#pragma once
#ifndef MESH_DATA_H
#define MESH_DATA_H

struct VertexWithColor {
	float x, y, z, w;
	float r, g, b, a;
};

static const VertexWithColor triangleData[] = {
	{0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
	{1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f},
	{-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f};
};

#endif /* !MESH_DATA_H */
