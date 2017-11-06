#pragma once

#include "math_3d.h"


bool loadOBJ(
	const char * path, 
	std::vector<Vector3f> & out_vertices, 
	std::vector<Vector2f> & out_uvs, 
	std::vector<Vector3f> & out_normals
);
