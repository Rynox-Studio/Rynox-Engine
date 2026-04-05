#include "Resource/ResourceLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Common/Logger.h>

namespace Rynox
{
    template<>
    std::unique_ptr<Model> ResourceLoader<Model>::Load(std::string_view path)
    {
        Assimp::Importer importer;
        unsigned int flags = aiProcess_Triangulate
            | aiProcess_GenNormals
            | aiProcess_FlipUVs
            | aiProcess_CalcTangentSpace
            | aiProcess_PreTransformVertices
            | aiProcess_MakeLeftHanded
            | aiProcess_FlipWindingOrder;

        const aiScene* scene = importer.ReadFile(path.data(), flags);

        if (!scene || !scene->mRootNode) 
        {
            RNX_LOG_ERROR("[ResourceLoader<Model>] Failed to load model: ({})", path);
            return nullptr;
        }

        auto model = std::make_unique<Model>();

        uint32_t totalVertices = 0;
        uint32_t totalIndices = 0;

        for (uint32_t i = 0; i < scene->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[i];

            totalVertices += mesh->mNumVertices;
            totalVertices += mesh->mNumFaces * 3;
        }

        model->Vertices.reserve(totalVertices);
        model->Indices.reserve(totalIndices);
        model->SubMeshes.reserve(scene->mNumMeshes);

        for (uint32_t  i = 0; i < scene->mNumMeshes; i++) 
        {
            aiMesh* mesh = scene->mMeshes[i];

            SubMesh subMesh{};
            subMesh.baseVertex = (int32_t)model->Vertices.size();
            subMesh.indexOffset = (uint32_t)model->Indices.size();
            subMesh.indexCount = 0;

            for(uint32_t  j = 0; j < mesh->mNumVertices; j++) 
            {
                Vertex v{};

                v.Position = {
                    mesh->mVertices[j].x,
                    mesh->mVertices[j].y,
                    mesh->mVertices[j].z
                };

                if(mesh->HasNormals()) {
                    v.Normal = {
                        mesh->mNormals[j].x,
                        mesh->mNormals[j].y,
                        mesh->mNormals[j].z
                    };
                }

                if(mesh->HasTangentsAndBitangents()) {
                    v.Tangent = {
                        mesh->mTangents[j].x,
                        mesh->mTangents[j].y,
                        mesh->mTangents[j].z
                    };

                    v.Bitangent = {
                        mesh->mBitangents[j].x,
                        mesh->mBitangents[j].y,
                        mesh->mBitangents[j].z
                    };
                }

                if(mesh->HasTextureCoords(0)) {
                    v.TexCoords = {
                        mesh->mTextureCoords[0][j].x,
                        mesh->mTextureCoords[0][j].y
                    };
                }

                model->Vertices.push_back(v);
            }

            for(uint32_t  j = 0; j < mesh->mNumFaces; j++) 
            {
                const aiFace& face = mesh->mFaces[j];
                for(uint32_t  k = 0; k < face.mNumIndices; k++) 
                {
                    model->Indices.push_back(face.mIndices[k]);
                    subMesh.indexCount++;
                }
            }

            model->SubMeshes.push_back(subMesh);
        }

        return model;
    }
}