
#include "CryptGrabberComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace CourseProject
{
    AZ_COMPONENT_IMPL(CryptGrabberComponent, "CryptGrabberComponent", "{D1095E74-2696-4006-B721-8AC291044DED}");

    void CryptGrabberComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<CryptGrabberComponent, AZ::Component>()
                ->Version(1)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<CryptGrabberComponent>("CryptGrabberComponent", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "Course Project")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ;
            }
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<CryptGrabberComponent>("CryptGrabber Component Group")
                ->Attribute(AZ::Script::Attributes::Category, "CourseProject Gem Group")
                ;
        }
    }

    void CryptGrabberComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("CryptGrabberComponentService"));
    }

    void CryptGrabberComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("CryptGrabberComponentService"));
    }

    void CryptGrabberComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void CryptGrabberComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    void CryptGrabberComponent::Activate()
    {
        CryptGrabberRequestBus::Handler::BusConnect(GetEntityId());
        AZ::TickBus::Handler::BusConnect();
    }

    void CryptGrabberComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        CryptGrabberRequestBus::Handler::BusDisconnect(GetEntityId());
    }

    void CryptGrabberComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
    }

    int CryptGrabberComponent::GetTickOrder()
    {
        return AZ::TICK_GAME;
    }
} // namespace CourseProject
