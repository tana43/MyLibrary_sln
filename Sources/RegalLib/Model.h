#pragma once
#include "Transform.h"
#include "SkinnedMesh.h"

namespace Regal::Model
{
    class Model
    {
    public:
        Model() {}
        virtual ~Model() {}

        virtual void Render() = 0;

        virtual void DrawDebug() = 0;

    protected:
        Regal::Game::TransformEuler transform;

        std::unique_ptr<Regal::Resource::SkinnedMesh> skinnedMesh;
    };
}
