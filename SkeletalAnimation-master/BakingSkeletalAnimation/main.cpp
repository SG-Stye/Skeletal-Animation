#include "MeshAnimator.h"


int main()
{
    // Init importer
    Assimp::Importer importer;

    // Specifying import flags
    unsigned int importFlags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_LimitBoneWeights;
    const aiScene* scene = importer.ReadFile("Mesh/Capoeira.fbx", importFlags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "Failed loading skinned mesh: " << importer.GetErrorString() << std::endl;
        return -1;
    }

    aiAnimation* animation = scene->mAnimations[0];
    float desiredTime = 60.0f;

    // Applying the specified pose to every mesh in scene
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) ApplyPose(scene->mMeshes[i], animation, desiredTime, scene);

    // Output obj file
    std::ofstream outputFile("Mesh/OutputMesh.obj");
    if (!outputFile.is_open())
    {
        std::cout << "Cannot open OutputMesh.obj." << std::endl;
        return -1;
    }

    // Writes each mesh into the obj file
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) MeshToObj(scene->mMeshes[i], outputFile);

    outputFile.close();

    return 0;
}