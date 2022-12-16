#ifndef TRIGGER_H
#define TRIGGER_H

#include "Components.h"


namespace Components {


class Trigger : public Scripted
{
public:
    Trigger(const Entity& entity, const Entity& target, const float& radius = 0.5f) :
            Scripted("Trigger", entity),
            m_target(target), 
            m_radius(radius) {}

    ~Trigger() = default;

    void OnUpdate() override;

    inline Entity GetTarget() { return m_target; }
    inline void SetTarget(const Entity& target) { m_target = target; }

private:
    Entity m_target;
    float m_radius;
    bool m_isInside = false;
};


} // Namespace Components


#endif // TRIGGER_H