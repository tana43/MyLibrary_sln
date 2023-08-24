#include "SceneManager.h"

namespace Regal::Scene
{
    void SceneManager::Initialize()
    {

    }

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
#ifdef _DEBUG
        //�e�V�[���̃f�o�b�O�\��
        currentScene->DrawDebug();

        //�V�[���}�l�[�W���[�̃f�o�b�O�\��
        DrawDebug();
#endif // _DEBUG

    }

    void SceneManager::Render(float elapsedFlame)
    {
        if (!currentScene)return;
        
        currentScene->Render(elapsedFlame);
    }

    void SceneManager::DrawDebug()
    {

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