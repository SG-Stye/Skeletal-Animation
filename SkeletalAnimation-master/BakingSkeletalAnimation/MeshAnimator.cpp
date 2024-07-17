#include "MeshAnimator.h"


aiMatrix4x4 Interpolate(float animTime, const aiNodeAnim* nodeAnim)
{
    aiMatrix4x4 transform;

    // TRANSLATION
    if (nodeAnim->mNumPositionKeys == 1)
    {
        transform = aiMatrix4x4();
        transform.a4 = nodeAnim->mPositionKeys[0].mValue.x;
        transform.b4 = nodeAnim->mPositionKeys[0].mValue.y;
        transform.c4 = nodeAnim->mPositionKeys[0].mValue.z;
    }

    else
    {
        unsigned int frameIndex = 0;
        for (unsigned int i = 0; i < nodeAnim->mNumPositionKeys - 1; i++)
        {
            if (animTime < nodeAnim->mPositionKeys[i+1].mTime)
            {
                frameIndex = i;
                break;
            }
        }

        unsigned int nextFrameIndex = (frameIndex + 1) % nodeAnim->mNumPositionKeys;

        float deltaTime = (float)(nodeAnim->mPositionKeys[nextFrameIndex].mTime - nodeAnim->mPositionKeys[frameIndex].mTime);
        float factor = (animTime - (float)nodeAnim->mPositionKeys[frameIndex].mTime) / deltaTime;

        const aiVector3D& startPosition = nodeAnim->mPositionKeys[frameIndex].mValue;
        const aiVector3D& endPosition = nodeAnim->mPositionKeys[nextFrameIndex].mValue;

        aiVector3D interpolatedPosition = startPosition + factor * (endPosition - startPosition);

        transform = aiMatrix4x4();
        transform.a4 = interpolatedPosition.x;
        transform.b4 = interpolatedPosition.y;
        transform.c4 = interpolatedPosition.z;
    }

    // ROTATION
    if (nodeAnim->mNumRotationKeys == 1)
    {
        aiQuaternion rotationQ = nodeAnim->mRotationKeys[0].mValue;
        aiMatrix4x4 rotationMatrix = aiMatrix4x4(rotationQ.GetMatrix());
        transform *= rotationMatrix;
    }

    else
    {
        unsigned int frameIndex = 0;
        for (unsigned int i = 0; i < nodeAnim->mNumRotationKeys - 1; i++)
        {
            if (animTime < nodeAnim->mRotationKeys[i+1].mTime)
            {
                frameIndex = i;
                break;
            }
        }

        unsigned int nextFrameIndex = (frameIndex + 1) % nodeAnim->mNumRotationKeys;

        float deltaTime = (float)(nodeAnim->mRotationKeys[nextFrameIndex].mTime - nodeAnim->mRotationKeys[frameIndex].mTime);
        float factor = (animTime - (float)nodeAnim->mRotationKeys[frameIndex].mTime) / deltaTime;

        const aiQuaternion& startRotationQ = nodeAnim->mRotationKeys[frameIndex].mValue;
        const aiQuaternion& endRotationQ = nodeAnim->mRotationKeys[nextFrameIndex].mValue;

        aiQuaternion interpolatedRotationQ;
        aiQuaternion::Interpolate(interpolatedRotationQ, startRotationQ, endRotationQ, factor);
        interpolatedRotationQ.Normalize();

        aiMatrix4x4 rotationMatrix = aiMatrix4x4(interpolatedRotationQ.GetMatrix());
        transform *= rotationMatrix;
    }

    // SCALING
    if (nodeAnim->mNumScalingKeys == 1)
    {
        aiVector3D scale = nodeAnim->mScalingKeys[0].mValue;

        aiMatrix4x4 scalingMatrix;
        scalingMatrix.a1 = scale.x;
        scalingMatrix.a2 = 0.0f;
        scalingMatrix.a3 = 0.0f;
        scalingMatrix.a4 = 0.0f;

        scalingMatrix.b1 = 0.0f;
        scalingMatrix.b2 = scale.y;
        scalingMatrix.b3 = 0.0f;
        scalingMatrix.b4 = 0.0f;

        scalingMatrix.c1 = 0.0f;
        scalingMatrix.c2 = 0.0f;
        scalingMatrix.c3 = scale.z;
        scalingMatrix.c4 = 0.0f;

        scalingMatrix.d1 = 0.0f;
        scalingMatrix.d2 = 0.0f;
        scalingMatrix.d3 = 0.0f;
        scalingMatrix.d4 = 1.0f;

        transform *= scalingMatrix;
    }

    else
    {
        unsigned int frameIndex = 0;
        for (unsigned int i = 0; i < nodeAnim->mNumScalingKeys - 1; i++)
        {
            if (animTime < nodeAnim->mScalingKeys[i+1].mTime)
            {
                frameIndex = i;
                break;
            }
        }

        unsigned int nextFrameIndex = (frameIndex + 1) % nodeAnim->mNumScalingKeys;

        float deltaTime = (float)(nodeAnim->mScalingKeys[nextFrameIndex].mTime - nodeAnim->mScalingKeys[frameIndex].mTime);
        float factor = (animTime - (float)nodeAnim->mScalingKeys[frameIndex].mTime) / deltaTime;

        const aiVector3D& startScaling = nodeAnim->mScalingKeys[frameIndex].mValue;
        const aiVector3D& endScaling = nodeAnim->mScalingKeys[nextFrameIndex].mValue;
        aiVector3D interpolatedScaling = startScaling + factor * (endScaling - startScaling);
        aiMatrix4x4 scalingMatrix;

        scalingMatrix.a1 = interpolatedScaling.x;
        scalingMatrix.a2 = 0.0f;
        scalingMatrix.a3 = 0.0f;
        scalingMatrix.a4 = 0.0f;

        scalingMatrix.b1 = 0.0f;
        scalingMatrix.b2 = interpolatedScaling.y;
        scalingMatrix.b3 = 0.0f;
        scalingMatrix.b4 = 0.0f;

        scalingMatrix.c1 = 0.0f;
        scalingMatrix.c2 = 0.0f;
        scalingMatrix.c3 = interpolatedScaling.z;
        scalingMatrix.c4 = 0.0f;

        scalingMatrix.d1 = 0.0f;
        scalingMatrix.d2 = 0.0f;
        scalingMatrix.d3 = 0.0f;
        scalingMatrix.d4 = 1.0f;

        transform *= scalingMatrix;
    }

    return transform;
}


aiMatrix4x4 GetGlobalTransform(const aiNode* node, const aiAnimation* anim, float animTime, const aiScene* scene)
{
    aiMatrix4x4 globalTransform;

    const aiNodeAnim* nodeAnim = nullptr;
    for (unsigned int i = 0; i < anim->mNumChannels; i++)
    {
        if (anim->mChannels[i]->mNodeName == node->mName)
        {
            nodeAnim = anim->mChannels[i];
            break;
        }
    }

    if (nodeAnim) globalTransform = Interpolate(animTime, nodeAnim);
    else globalTransform = node->mTransformation;

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        const aiNode* childNode = node->mChildren[i];
        if (childNode)
        {
            aiMatrix4x4 childTransform = GetGlobalTransform(childNode, anim, animTime, scene);
            globalTransform *= childTransform;
        }
    }

    return globalTransform;
}


void MeshToObj(const aiMesh* mesh, std::ofstream& outputFile)
{
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        aiVector3D vertex = mesh->mVertices[i];
        outputFile << "v " << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        if (face.mNumIndices != 3)
        {
            std::cout << "Il formato OBJ supporta solo triangoli. La mesh contiene facce con " << face.mNumIndices << " vertici." << std::endl;
            return;
        }
        outputFile << "f ";
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            outputFile << face.mIndices[j] + 1 << " ";
        }
        outputFile << std::endl;
    }
}


void ApplyPose(const aiMesh* mesh, const aiAnimation* animation, float animationTime, const aiScene* scene)
{
    // If the mesh has no bones, there is no need to apply a pose
    if (!mesh->HasBones()) return;

    aiMatrix4x4 globalTransformation = GetGlobalTransform(scene->mRootNode, animation, animationTime, scene);

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        aiVector3D vertex = mesh->mVertices[i];
        aiVector3D normal = mesh->mNormals ? mesh->mNormals[i] : aiVector3D(0.0f, 0.0f, 0.0f);

        aiVector3D transformedVertex = globalTransformation * vertex;
        aiVector3D transformedNormal = globalTransformation * normal;

        aiVector3D translation = mesh->mVertices[i] - transformedVertex;
        transformedVertex += translation;

        if (mesh->mNormals) transformedNormal.Normalize();

        // Assegna il vertice trasformato alla mesh
        mesh->mVertices[i] = transformedVertex;
        if (mesh->mNormals) mesh->mNormals[i] = transformedNormal;
    }
}