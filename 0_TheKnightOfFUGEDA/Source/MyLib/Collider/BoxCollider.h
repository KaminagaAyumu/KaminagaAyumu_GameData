#pragma once
#include "ColliderBase.h"

namespace MyLib
{
    /// <summary>
    /// ボックス状の当たり判定
    /// </summary>
    class BoxCollider : public ColliderBase
    {
    public:

        BoxCollider(ColliderBase::ObjectTag tag, const Vector3& size, bool isTrigger, const std::string& name = "");
        virtual ~BoxCollider() = default;

        const Vector3& GetSize() const { return m_size; }

        virtual BoundingBox GetBoundingBox(const Vector3& worldPos, const Quaternion& rotation) const override;

        virtual void DrawDebug(const Vector3& worldPos, const Quaternion& rotation) const override;

    private:
        // 矩形の大きさ(x,y,z)
        Vector3 m_size;
    };
}


