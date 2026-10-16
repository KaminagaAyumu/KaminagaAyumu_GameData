#include "ObjectFactory.h"
#include "ObjectManager.h"
#include "GameObject.h"
#include "../Object/Player/Player.h"
#include "../Object/Enemy/EnemyBase.h"
#include "../Object/Enemy/BulletEnemy.h"
#include "../Object/Enemy/SkullEnemy.h"
#include "../Object/Item/Item.h"
#include "../Object/Projectile/EnemyBullet.h"
#include "../Object/UI/UIImageObj.h"
#include "../Object/UI/UICountDownObj.h"
#include "../Object/UI/UISelectListObj.h"
#include "../Object/UI/UITelopObj.h"
#include "../Object/UI/UITextObj.h"
#include "../Object/UI/UIComboObj.h"
#include "../Object/Stage.h"
#include "../Object/Skybox.h"
#include "../Object/Camera/PlayerCamera.h"
#include "Component/Rigidbody.h"
#include "Component/Collision/Collidable.h"
#include "Component/Draw/Drawable3D.h"
#include "Component/Draw/UI/UIImage.h"
#include "Component/Draw/UI/UISelectList.h"
#include "Component/Draw/UI/UITelop.h"
#include "Component/Draw/UI/UICountDown.h"
#include "Component/Draw/UI/UIText.h"
#include "Component/Draw/UI/UICombo.h"

std::shared_ptr<MyLib::GameObject> MyLib::ObjectFactory::CreatePlayer()
{
    // プレイヤーを生成
    std::shared_ptr<Player> pPlayer = std::make_shared<Player>();

    // オブジェクト管理クラスに追加
	MyLib::ObjectManager::GetInstance().AddObject(pPlayer);

    return pPlayer;
}

std::shared_ptr<MyLib::GameObject> MyLib::ObjectFactory::CreateEnemy(EnemyManager::EnemyType type)
{
    // FIXME:どういう敵を生成するのかを引数で受け取り、それによって生成するオブジェクトを変える


    // 敵を生成
    std::shared_ptr<EnemyBase> pEnemy;

    switch (type)
    {
    case EnemyManager::EnemyType::BulletEnemy:
        pEnemy = std::make_shared<BulletEnemy>();
        break;
    case EnemyManager::EnemyType::SkullEnemy:
        pEnemy = std::make_shared<SkullEnemy>();
        break;
    default:
        break;
    }

    // オブジェクト管理クラスに追加
    MyLib::ObjectManager::GetInstance().AddObject(pEnemy);

    return pEnemy;
}

std::shared_ptr<MyLib::GameObject> MyLib::ObjectFactory::CreateItem()
{
    std::shared_ptr<Item> pItem;

    pItem = std::make_shared<Item>();

    MyLib::ObjectManager::GetInstance().AddObject(pItem);

    return pItem;
}

std::shared_ptr<MyLib::GameObject> MyLib::ObjectFactory::CreateSkybox()
{
    // ステージを生成
    std::shared_ptr<Skybox> pSkybox = std::make_shared<Skybox>();

    // オブジェクト管理クラスに追加
    MyLib::ObjectManager::GetInstance().AddObject(pSkybox);

    return pSkybox;
}

std::shared_ptr<MyLib::GameObject> MyLib::ObjectFactory::CreateBullet(const Vector3& pos, const Vector3& target, std::weak_ptr<GameObject> shooter)
{
    std::shared_ptr<EnemyBullet> pBullet = std::make_shared<EnemyBullet>();

    pBullet->Init(pos, target, shooter);

    // オブジェクト管理クラスに追加
    MyLib::ObjectManager::GetInstance().AddObject(pBullet);

    return pBullet;
}

std::shared_ptr<MyLib::GameObject> MyLib::ObjectFactory::CreateCamera()
{
    // FIXME:どういうカメラを生成するのかを引数で受け取り、それによって生成するオブジェクトを変える

    // カメラを生成
    std::shared_ptr<PlayerCamera> pCamera = std::make_shared<PlayerCamera>();

    // オブジェクト管理クラスに追加
    MyLib::ObjectManager::GetInstance().AddObject(pCamera);

    return pCamera;
}

std::shared_ptr<MyLib::GameObject> MyLib::ObjectFactory::CreateUIImage(const Vector2Int& pos)
{
	// UI画像を生成
	std::shared_ptr<UIImageObj> pUIImage = std::make_shared<UIImageObj>();

	auto pTransform = pUIImage->GetComponent<MyLib::Transform>();
    if (auto transform = pTransform.lock())
    {
        transform->SetScreenPos(pos);
    }

	auto pImage = pUIImage->GetComponent<MyLib::UIImage>();

    // オブジェクト管理クラスに追加
    MyLib::ObjectManager::GetInstance().AddObject(pUIImage);

    return pUIImage;
}

std::shared_ptr<MyLib::GameObject> MyLib::ObjectFactory::CreateUISelectList(const Vector2Int& pos, MyLib::Renderer::FontType type)
{
    // UI画像を生成
    std::shared_ptr<UISelectListObj> pUISelectList = std::make_shared<UISelectListObj>();

    auto pTransform = pUISelectList->GetComponent<MyLib::Transform>();
    if (auto transform = pTransform.lock())
    {
        transform->SetScreenPos(pos);
    }

    auto pSelectList = pUISelectList->GetComponent<MyLib::UISelectList>().lock();
    pSelectList->SetFontHandle(MyLib::Renderer::GetInstance().GetFontHandle(type));

    // オブジェクト管理クラスに追加
    MyLib::ObjectManager::GetInstance().AddObject(pUISelectList);

    return pUISelectList;
}

std::shared_ptr<MyLib::GameObject> MyLib::ObjectFactory::CretateUITelop(const Vector2Int& pos, MyLib::Renderer::FontType type)
{
    // UIテロップを生成
    std::shared_ptr<UITelopObj> pUITelop = std::make_shared<UITelopObj>();

    auto pTransform = pUITelop->GetComponent<MyLib::Transform>();
    if (auto transform = pTransform.lock())
    {
        transform->SetScreenPos(pos);
    }

    auto pTelop = pUITelop->GetComponent<MyLib::UITelop>().lock();
    pTelop->SetFontHandle(MyLib::Renderer::GetInstance().GetFontHandle(type));

    // オブジェクト管理クラスに追加
    MyLib::ObjectManager::GetInstance().AddObject(pUITelop);

    return pUITelop;
}

std::shared_ptr<MyLib::GameObject> MyLib::ObjectFactory::CreateUICountDown(const Vector2Int& pos, MyLib::Renderer::FontType type)
{
    // UIテロップを生成
    std::shared_ptr<UICountDownObj> pUICountDown = std::make_shared<UICountDownObj>();

    auto pTransform = pUICountDown->GetComponent<MyLib::Transform>();
    if (auto transform = pTransform.lock())
    {
        transform->SetScreenPos(pos);
    }

    auto pCountDown = pUICountDown->GetComponent<MyLib::UICountDown>().lock();
    pCountDown->SetFontHandle(MyLib::Renderer::GetInstance().GetFontHandle(type));

    // オブジェクト管理クラスに追加
    MyLib::ObjectManager::GetInstance().AddObject(pUICountDown);

    return pUICountDown;
}

std::shared_ptr<MyLib::GameObject> MyLib::ObjectFactory::CreateUIText(const Vector2Int& pos, MyLib::Renderer::FontType type)
{
    // UIテキストを生成
    std::shared_ptr<UITextObj> pUIText = std::make_shared<UITextObj>();

    auto pTransform = pUIText->GetComponent<MyLib::Transform>();
    if (auto transform = pTransform.lock())
    {
        transform->SetScreenPos(pos);
    }

    auto pText = pUIText->GetComponent<MyLib::UIText>().lock();
    pText->SetFontHandle(MyLib::Renderer::GetInstance().GetFontHandle(type));

    // オブジェクト管理クラスに追加
    MyLib::ObjectManager::GetInstance().AddObject(pUIText);

    return pUIText;
}

std::shared_ptr<MyLib::GameObject> MyLib::ObjectFactory::CreateUICombo(const Vector2Int& pos, MyLib::Renderer::FontType type)
{
    // コンボ用のUIを生成
    std::shared_ptr<UIComboObj> pUICombo = std::make_shared<UIComboObj>();

    auto pTransform = pUICombo->GetComponent<MyLib::Transform>();
    if (auto transform = pTransform.lock())
    {
        transform->SetScreenPos(pos);
    }

    auto pCombo = pUICombo->GetComponent<MyLib::UICombo>().lock();
    pCombo->SetFontHandle(MyLib::Renderer::GetInstance().GetFontHandle(type));

    // オブジェクト管理クラスに追加
    MyLib::ObjectManager::GetInstance().AddObject(pUICombo);

    return pUICombo;
}
