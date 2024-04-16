
#include "CryptTriggerComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Console/ILogger.h>
#include <AzFramework/Physics/Common/PhysicsSimulatedBody.h>
#include <AzFramework/Physics/PhysicsSystem.h>
#include <LmbrCentral/Scripting/TagComponentBus.h>

#include "CourseProject/CryptMoverInterface.h"
#include "CourseProject/CryptGrabberInterface.h"

namespace CourseProject
{
    AZ_COMPONENT_IMPL(CryptTriggerComponent, "CryptTriggerComponent", "{E34200A8-C1CB-4F1F-8626-4E0938988748}");

    void CryptTriggerComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<CryptTriggerComponent, AZ::Component>()
                ->Version(1)
                ->Field("AcceptableEntityTag", &CryptTriggerComponent::m_acceptableEntityTag)
                ->Field("TriggersOnce", &CryptTriggerComponent::m_triggersOnce)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<CryptTriggerComponent>("CryptTriggerComponent", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "Course Project")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))

                    ->DataElement(AZ::Edit::UIHandlers::Default, &CryptTriggerComponent::m_acceptableEntityTag, "Acceptable Entity Tag", "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CryptTriggerComponent::m_triggersOnce, "Trigger Once", "")
                    ;
            }
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<CryptTriggerComponent>("CryptTrigger Component Group")
                ->Attribute(AZ::Script::Attributes::Category, "CourseProject Gem Group")
                ;

            behaviorContext->EBus<CryptTriggerRequestBus>("CryptTriggerRequestBus")
                ->Attribute(AZ::Script::Attributes::Category, "CourseProject Gem Group")
                ->Event("GetCryptMover", &CryptTriggerRequests::GetCryptMover)
                ->Event("SetCryptMover", &CryptTriggerRequests::SetCryptMover)
                ;
        }
    }

    void CryptTriggerComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("CryptTriggerComponentService"));
    }

    void CryptTriggerComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
    }

    void CryptTriggerComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void CryptTriggerComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    AZ::EntityId CryptTriggerComponent::GetCryptMover() const
    {
        return m_cryptMoverEntityId;
    }

    void CryptTriggerComponent::SetCryptMover(AZ::EntityId cryptMoverEntityId)
    {
        m_cryptMoverEntityId = cryptMoverEntityId;
        m_cryptMover = CryptMoverRequestBus::FindFirstHandler(cryptMoverEntityId);
    }

    void CryptTriggerComponent::Init()
    {
        m_onTriggerEnterHandler = AzPhysics::SimulatedBodyEvents::OnTriggerEnter::Handler([this](
            [[maybe_unused]] AzPhysics::SimulatedBodyHandle bodyHandle,
            const AzPhysics::TriggerEvent& triggerEvent)
            {
                OnTriggerEnter(triggerEvent);
            });

        m_onTriggerExitHandler = AzPhysics::SimulatedBodyEvents::OnTriggerExit::Handler([this](
            [[maybe_unused]] AzPhysics::SimulatedBodyHandle bodyHandle,
            const AzPhysics::TriggerEvent& triggerEvent)
            {
                OnTriggerExit(triggerEvent);
            });
    }

    void CryptTriggerComponent::Activate()
    {
        // During entity activation the simulated bodies are not created yet.
        // Connect to RigidBodyNotificationBus to listen when they get enabled to register the trigger handlers.
        Physics::RigidBodyNotificationBus::Handler::BusConnect(GetEntityId());

        CryptTriggerRequestBus::Handler::BusConnect(GetEntityId());
        AZ::TickBus::Handler::BusConnect();
    }

    void CryptTriggerComponent::Deactivate()
    {
        m_onTriggerEnterHandler.Disconnect();
        m_onTriggerExitHandler.Disconnect();

        AZ::TickBus::Handler::BusDisconnect();
        CryptTriggerRequestBus::Handler::BusDisconnect(GetEntityId());

        Physics::RigidBodyNotificationBus::Handler::BusDisconnect();
    }

    void CryptTriggerComponent::OnPhysicsEnabled(const AZ::EntityId& entityId)
    {
        if (auto* physicsSystem = AZ::Interface<AzPhysics::SystemInterface>::Get())
        {
            AZStd::pair<AzPhysics::SceneHandle, AzPhysics::SimulatedBodyHandle> foundBody =
                physicsSystem->FindAttachedBodyHandleFromEntityId(entityId);
            if (foundBody.first != AzPhysics::InvalidSceneHandle)
            {
                AzPhysics::SimulatedBodyEvents::RegisterOnTriggerEnterHandler(
                    foundBody.first, foundBody.second, m_onTriggerEnterHandler);
                AzPhysics::SimulatedBodyEvents::RegisterOnTriggerExitHandler(
                    foundBody.first, foundBody.second, m_onTriggerExitHandler);
            }
        }
    }

    void CryptTriggerComponent::OnPhysicsDisabled([[maybe_unused]] const AZ::EntityId& entityId)
    {
        m_onTriggerEnterHandler.Disconnect();
        m_onTriggerExitHandler.Disconnect();
    }

    void CryptTriggerComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        // Do nothing if the mover hasnt' been setup
        if (!m_cryptMover)
        {
            return;
        }

        // While there is an acceptable entity in the trigger, move the mover.
        if (m_acceptableEntityId.IsValid())
        {
            if (m_triggersOnce)
            {
                // Disconnect from all the buses and leave the mover moving.
                Deactivate();

                // Disable acceptable entity's physics and collisions
                Physics::RigidBodyRequestBus::Event(m_acceptableEntityId, &Physics::RigidBodyRequests::DisablePhysics);

                // Attach the acceptable entity to the mover so it moves with it.
                AZ::TransformBus::Event(m_acceptableEntityId, &AZ::TransformInterface::SetParent, m_cryptMoverEntityId);
            }

            // Start moving the mover.
            m_cryptMover->SetShouldMove(true);
        }
        else
        {
            // Stop moving the mover.
            m_cryptMover->SetShouldMove(false);
        }
    }

    void CryptTriggerComponent::OnTriggerEnter(const AzPhysics::TriggerEvent& triggerEvent)
    {
        if (!triggerEvent.m_otherBody)
        {
            return;
        }

        // Ignore self
        AZ::EntityId entityId = triggerEvent.m_otherBody->GetEntityId();
        if (entityId == GetEntityId())
        {
            return;
        }

        bool hasAcceptableTag = false;
        LmbrCentral::TagComponentRequestBus::EventResult(hasAcceptableTag, entityId, 
            &LmbrCentral::TagComponentRequests::HasTag, LmbrCentral::Tag(m_acceptableEntityTag));

        bool hasGrabbedTag = false;
        LmbrCentral::TagComponentRequestBus::EventResult(hasGrabbedTag, entityId, 
            &LmbrCentral::TagComponentRequests::HasTag, GrabbedTag);

        if (hasAcceptableTag && !hasGrabbedTag)
        {
            m_acceptableEntityId = entityId;
        }
    }

    void CryptTriggerComponent::OnTriggerExit(const AzPhysics::TriggerEvent& triggerEvent)
    {
        if (!triggerEvent.m_otherBody)
        {
            return;
        }

        if (triggerEvent.m_otherBody->GetEntityId() == m_acceptableEntityId)
        {
            m_acceptableEntityId.SetInvalid();
        }
    }
} // namespace CourseProject
