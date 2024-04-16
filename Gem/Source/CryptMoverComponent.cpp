
#include "CryptMoverComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Console/ILogger.h>
#include <AzCore/Component/Entity.h>
#include <AzCore/Component/TransformBus.h>

namespace CourseProject
{
    AZ_COMPONENT_IMPL(CryptMoverComponent, "CryptMoverComponent", "{334E66FD-8765-4C4E-997B-41A534870B86}");

    void CryptMoverComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<CryptMoverComponent, AZ::Component>()
                ->Version(1)
                ->Field("MoveOffset", &CryptMoverComponent::m_moveOffset)
                ->Field("MoveDistance", &CryptMoverComponent::m_moveTime)
                ->Field("ShouldMove", &CryptMoverComponent::m_shouldMove)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<CryptMoverComponent>("CryptMoverComponent", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "Course Project")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))

                    ->DataElement(AZ::Edit::UIHandlers::Default, &CryptMoverComponent::m_moveOffset, "Move Offset", "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CryptMoverComponent::m_moveTime, "Move Time", "")
                    ->Attribute(AZ::Edit::Attributes::Min, 0.001f)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CryptMoverComponent::m_shouldMove, "Should Move", "")
                    ;
            }
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<CryptMoverComponent>("CryptMover Component Group")
                ->Attribute(AZ::Script::Attributes::Category, "CourseProject Gem Group")
                ;

            behaviorContext->EBus<CryptMoverRequestBus>("CryptMoverRequestBus")
                ->Attribute(AZ::Script::Attributes::Category, "CourseProject Gem Group")
                ->Event("GetShouldMove", &CryptMoverRequests::GetShouldMove)
                ->Event("SetShouldMove", &CryptMoverRequests::SetShouldMove)
                ;
        }
    }

    void CryptMoverComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("CryptMoverComponentService"));
    }

    void CryptMoverComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("CryptMoverComponentService"));
    }

    void CryptMoverComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void CryptMoverComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    bool CryptMoverComponent::GetShouldMove() const
    {
        return m_shouldMove;
    }

    void CryptMoverComponent::SetShouldMove(bool shouldMove)
    {
        m_shouldMove = shouldMove;
    }

    void CryptMoverComponent::Activate()
    {
        m_startLocation = GetEntity()->GetTransform()->GetLocalTranslation();

        CryptMoverRequestBus::Handler::BusConnect(GetEntityId());
        AZ::TickBus::Handler::BusConnect();
    }

    void CryptMoverComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        CryptMoverRequestBus::Handler::BusDisconnect(GetEntityId());
    }

    void CryptMoverComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        const AZ::Vector3 targetLocation = m_shouldMove
            ? m_startLocation + m_moveOffset
            : m_startLocation;

        const AZ::Vector3 currentLocation = GetEntity()->GetTransform()->GetLocalTranslation();
        const AZ::Vector3 deltaMove = (targetLocation-currentLocation).GetNormalizedSafe() * (m_moveOffset.GetLength() / m_moveTime) * deltaTime;

        if (currentLocation.GetDistanceSq(targetLocation) > deltaMove.GetLengthSq())
        {
            GetEntity()->GetTransform()->SetLocalTranslation(currentLocation + deltaMove);
        }
        else
        {
            GetEntity()->GetTransform()->SetLocalTranslation(targetLocation);
        }
    }

    int CryptMoverComponent::GetTickOrder()
    {
        return AZ::TICK_GAME;
    }
} // namespace CourseProject
