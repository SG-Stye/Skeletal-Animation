#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>


// Returns the matrix obtained by interpolating the transformation based on the animationTime
aiMatrix4x4 Interpolate(float animationTime, const aiNodeAnim* nodeAnim);

// returns the global transforms matrix from the given parameters
aiMatrix4x4 GetGlobalTransform(const aiNode* node, const aiAnimation* animation, float animationTime, const aiScene* scene);

// Writes a single mesh in obj format
void MeshToObj(const aiMesh* mesh, std::ofstream& outputFile);

// Applies a determined pose to a single mesh
void ApplyPose(const aiMesh* mesh, const aiAnimation* animation, float animationTime, const aiScene* scene);