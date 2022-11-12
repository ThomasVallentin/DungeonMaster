#include "Model.h"


void Model::AddMesh(const ResourceHandle<Mesh>& meshHandle) 
{
    m_meshes.push_back(meshHandle);
}

ModelPtr Model::Create() 
{
    return ModelPtr(new Model);
}

