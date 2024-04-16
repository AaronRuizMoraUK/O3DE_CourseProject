
#include "MoverComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Console/ILogger.h>
#include <AzCore/Component/Entity.h>
#include <AzCore/Component/TransformBus.h>

namespace CourseProject
{
    AZ_COMPONENT_IMPL(MoverComponent, "MoverComponent", "{DDA89E4C-0FAC-4C79-AFE8-C65010D6FA6F}");

    void MoverComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<MoverComponent, AZ::Component>()
                ->Version(1)
                ->Field("MoveVelocity", &MoverComponent::m_moveVelocity)
                ->Field("MoveDistance", &MoverComponent::m_moveDistance)
                ->Field("RotateVelocity", &MoverComponent::m_rotateVelocity)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<MoverComponent>("MoverComponent", "Moves an entity back and forth. Rotation can be added as well.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "Course Project")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))

                    ->DataElement(AZ::Edit::UIHandlers::Default, &MoverComponent::m_moveVelocity, "Move Velocity", "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &MoverComponent::m_moveDistance, "Move Distance", "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &MoverComponent::m_rotateVelocity, "Rotate Velocity", "")
                    ;
            }
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<MoverComponent>("Mover Component Group")
                ->Attribute(AZ::Script::Attributes::Category, "CourseProject Gem Group")
                ;
        }
    }

    void MoverComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("MoverComponentService"));
    }

    void MoverComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
    }

    void MoverComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void MoverComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    void MoverComponent::Activate()
    {
        m_startLocation = GetEntity()->GetTransform()->GetLocalTranslation();

        MoverRequestBus::Handler::BusConnect(GetEntityId());
        AZ::TickBus::Handler::BusConnect();
    }

    void MoverComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        MoverRequestBus::Handler::BusDisconnect(GetEntityId());
    }

    void MoverComponent::OnTick(float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        MovePlatform(deltaTime);
        RotatePlatform(deltaTime);
    }

    int MoverComponent::GetTickOrder()
    {
        return AZ::TICK_GAME;
    }

    void MoverComponent::MovePlatform(float deltaTime)
    {
        auto* transform = GetEntity()->GetTransform();

        AZ::Vector3 currentLocation = transform->GetLocalTranslation() + m_moveVelocity * deltaTime;

        // New location will reach or surpass the move distance?
        if (m_startLocation.GetDistance(currentLocation) >= m_moveDistance)
        {
            // Reset the start location to the other extreme
            m_startLocation += m_moveVelocity.GetNormalizedSafe() * m_moveDistance;
            currentLocation = m_startLocation;

            // Reverse the velocity to go in the opposite direction from the next frame
            m_moveVelocity = -m_moveVelocity;
        }

        transform->SetLocalTranslation(currentLocation);
    }

    void MoverComponent::RotatePlatform([[maybe_unused]] float deltaTime)
    {
        auto* transform = GetEntity()->GetTransform();

        AZ::Vector3 currentRotation = transform->GetLocalRotation() + m_rotateVelocity * deltaTime * (AZ::Constants::Pi / 180.0f);

        transform->SetLocalRotation(currentRotation);
    }
} // namespace CourseProject
