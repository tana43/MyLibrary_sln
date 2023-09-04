#include "SceneManager.h"
#include "../../../External/imgui/imgui.h"

namespace Regal::Scene
{
    void SceneManager::Update(float elapsedFlame)
    {
        //���̃V�[�����Z�b�g����Ă���Ȃ�؂�ւ�
        if (nextScene)
        {
            //�Â��V�[�����I��
            Clear();

            //�V�����V�[����ݒ�
            currentScene = nextScene;
            nextScene = nullptr;

            //�V�[������������(�}���`�X���b�h���������Ă��Ȃ��ꍇ�ɍs��)
            if (!currentScene->IsReady())
            {
                currentScene->CreateResource();
                currentScene->Initialize();
            }
        }

        //�V�[���X�V
        if (currentScene)
        {
            currentScene->Begin();
            currentScene->Update(elapsedFlame);
            currentScene->End();
        }

    }

    void SceneManager::Render(float elapsedTime)
    {
        if (!currentScene)return;
        
        currentScene->Render(elapsedTime);
    }

    void SceneManager::DrawDebug()
    {
        if (!currentScene)return;

        //�e�V�[���̃f�o�b�O�\��
        if (ImGui::BeginMenu("Scene"))
        {
            currentScene->DrawDebug();
            ImGui::EndMenu();
        }
    }

    void SceneManager::Clear()
    {
        if (!currentScene)return;
        {
            currentScene->Finalize();
            delete currentScene;
        }
    }

    //�V�[���؂�ւ�
    void SceneManager::ChangeScene(BaseScene* scene)
    {
        //�V�����V�[����ݒ�
        nextScene = scene;
    }
}