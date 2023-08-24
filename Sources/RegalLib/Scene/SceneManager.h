#pragma once
#include "BaseScene.h"

namespace Regal::Scene
{
    class SceneManager
    {
    private:
        SceneManager() {}
        ~SceneManager() {}

    public:
        static SceneManager& Instance()
        {
            static SceneManager instance;
            return instance;
        }

        void Initialize();
        void Update(float elapsedFlame);
        void Render(float elapsedFlame);

        void DrawDebug();
        void Clear();

        //�V�[���؂�ւ�
        void ChangeScene(BaseScene* scene);

        BaseScene* GetCurrentScene()const { return currentScene; }

    private:
        BaseScene* currentScene = nullptr;
        BaseScene* nextScene = nullptr;
    };
}

