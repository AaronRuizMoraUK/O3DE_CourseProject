
#include "CryptGrabberComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Console/ILogger.h>
#include <AzFramework/Physics/RigidBodyBus.h>
#include <AzFramework/Physics/PhysicsScene.h>
#include <AzFramework/Physics/CollisionBus.h>
#include <LmbrCentral/Scripting/TagComponentBus.h>

namespace CourseProject
{
    AZ_COMPONENT_IMPL(CryptGrabberComponent, "CryptGrabberComponent", "{D1095E74-2696-4006-B721-8AC291044DED}");

    void CryptGrabberComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<CryptGrabberComponent, AZ::Component>()
                ->Version(1)
                ->Field("MaxGrabDistance", &CryptGrabberComponent::m_maxGrabDistance)
                ->Field("GrabSphereRadius", &CryptGrabberComponent::m_grabSphereRadius)
                ->Field("HoldDistance", &CryptGrabberComponent::m_holdDistance)
                ->Field("DropDistance", &CryptGrabberComponent::m_dropDistance)
                ->Field("CollisionGroupId", &CryptGrabberComponent::m_collisionGroupId)
                ->Field("Debug", &CryptGrabberComponent::m_debug)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<CryptGrabberComponent>("CryptGrabberComponent", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "Course Project")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))

                    ->DataElement(AZ::Edit::UIHandlers::Default, &CryptGrabberComponent::m_maxGrabDistance, "Max Grab Distance", "")
                    ->Attribute(AZ::Edit::Attributes::Min, 0.0f)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CryptGrabberComponent::m_grabSphereRadius, "Grab Sphere Radius", "")
                    ->Attribute(AZ::Edit::Attributes::Min, 0.001f)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CryptGrabberComponent::m_holdDistance, "Hold Distance", "")
                    ->Attribute(AZ::Edit::Attributes::Min, 0.0f)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CryptGrabberComponent::m_dropDistance, "Drop Distance", "")
                    ->Attribute(AZ::Edit::Attributes::Min, 0.0f)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CryptGrabberComponent::m_collisionGroupId, "Collision Group", "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CryptGrabberComponent::m_debug, "Debug", "")
                    ;
            }
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<CryptGrabberComponent>("CryptGrabber Component Group")
                ->Attribute(AZ::Script::Attributes::Category, "CourseProject Gem Group")
                ;

            behaviorContext->EBus<CryptGrabberRequestBus>("CryptGrabberRequestBus")
                ->Attribute(AZ::Script::Attributes::Category, "CourseProject Gem Group")
                ->Event("Grab", &CryptGrabberRequests::Grab)
                ->Event("Release", &CryptGrabberRequests::Release)
                ->Event("IsGrabbing", &CryptGrabberRequests::IsGrabbing)
                ->Event("IsDebugEnabled", &CryptGrabberRequests::IsDebugEnabled)
                ->Event("EnableDebug", &CryptGrabberRequests::EnableDebug)
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

    void CryptGrabberComponent::Grab()
    {
        // Do nothing if it's grabbing an object already
        if (IsGrabbing())
        {
            return;
        }

        if (auto hitResult = CheckHit();
            hitResult.has_value())
        {
            m_grabbedEntityId = hitResult->m_entityId;

            if (m_debug)
            {
                AZLOG_INFO("Grabbing entity: %s", m_grabbedEntityId.ToString().c_str());
            }

            // Set grabbed entity as kinematic so the rigid body moves programatically
            Physics::RigidBodyRequestBus::Event(m_grabbedEntityId, &Physics::RigidBodyRequests::SetKinematic, true);

            // Add the tag "Grabbed" to the Hit Entity so it knows it's been grabbed.
            LmbrCentral::TagComponentRequestBus::Event(m_grabbedEntityId,
                &LmbrCentral::TagComponentRequests::AddTag, GrabbedTag);

            AZ::Transform grabbedEntityWorldTM = AZ::Transform::Identity();
            AZ::TransformBus::EventResult(grabbedEntityWorldTM, m_grabbedEntityId, &AZ::TransformInterface::GetWorldTM);

            AZ::Transform inverseTransform = GetEntity()->GetTransform()->GetWorldTM().GetInverse();
            m_grabbedEntityLocalTM = inverseTransform * grabbedEntityWorldTM;
            m_grabbedTargetLocalOffset = m_grabbedEntityLocalTM.GetTranslation() - inverseTransform.TransformPoint(hitResult->m_position);
        }
        else
        {
            if (m_debug)
            {
                AZLOG_INFO("No entity grabbed!");
            }
        }
    }

    void CryptGrabberComponent::Release()
    {
        if (IsGrabbing())
        {
            if (m_debug)
            {
                AZLOG_INFO("Releasing entity: %s", m_grabbedEntityId.ToString().c_str());
            }

            // Set entity as simulated rigid body (non kinematic)
            Physics::RigidBodyRequestBus::Event(m_grabbedEntityId, &Physics::RigidBodyRequests::SetKinematic, false);

            // Remove the tag "Grabbed" (if still present)
            LmbrCentral::TagComponentRequestBus::Event(m_grabbedEntityId,
                &LmbrCentral::TagComponentRequests::RemoveTag, GrabbedTag);

            m_grabbedEntityId.SetInvalid();
        }
        else
        {
            if (m_debug)
            {
                AZLOG_INFO("No entity to release!");
            }
        }
    }

    bool CryptGrabberComponent::IsGrabbing() const
    {
        return m_grabbedEntityId.IsValid();
    }

    bool CryptGrabberComponent::IsDebugEnabled() const
    {
        return m_debug;
    }

    void CryptGrabberComponent::EnableDebug(bool enable)
    {
        m_debug = enable;
    }

    void CryptGrabberComponent::Activate()
    {
        // Set the collision group based on the group Id that is selected
        Physics::CollisionRequestBus::BroadcastResult(
            m_collisionGroup, &Physics::CollisionRequests::GetCollisionGroupById, m_collisionGroupId);

        CryptGrabberRequestBus::Handler::BusConnect(GetEntityId());
        AZ::TickBus::Handler::BusConnect();
        AzFramework::EntityDebugDisplayEventBus::Handler::BusConnect(GetEntityId());
    }

    void CryptGrabberComponent::Deactivate()
    {
        AzFramework::EntityDebugDisplayEventBus::Handler::BusDisconnect(GetEntityId());
        AZ::TickBus::Handler::BusDisconnect();
        CryptGrabberRequestBus::Handler::BusDisconnect(GetEntityId());
    }

    void CryptGrabberComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        if (IsGrabbing())
        {
            bool hasGrabbedTag = false;
            LmbrCentral::TagComponentRequestBus::EventResult(hasGrabbedTag, m_grabbedEntityId,
                &LmbrCentral::TagComponentRequests::HasTag, GrabbedTag);

            AZ::Vector3 cameraLocation = GetEntity()->GetTransform()->GetWorldTranslation();
            AZ::Vector3 grabbedEntityLocation = AZ::Vector3::CreateZero();
            AZ::TransformBus::EventResult(grabbedEntityLocation, m_grabbedEntityId, &AZ::TransformInterface::GetWorldTranslation);

            // Check if the grabbed component must be released, that's when:
            // - The component doesn't have the tag "Grabbed" anymore
            // - The component gets too far away.
            if (!hasGrabbedTag ||
                cameraLocation.GetDistance(grabbedEntityLocation) > m_dropDistance)
            {
                Release();
            }
            else
            {
                // Update grabbed component location and rotation
                AZ::Vector3 currentLocalLocation = m_grabbedEntityLocalTM.GetTranslation();
                AZ::Vector3 targetLocalLocation = AZ::Vector3::CreateAxisY(m_holdDistance) + m_grabbedTargetLocalOffset;
                AZ::Vector3 deltaMove = (targetLocalLocation - currentLocalLocation).GetNormalizedSafe() * 8.0f * deltaTime;
                if (currentLocalLocation.GetDistanceSq(targetLocalLocation) > deltaMove.GetLengthSq())
                {
                    m_grabbedEntityLocalTM.SetTranslation(currentLocalLocation + deltaMove);
                }
                else
                {
                    m_grabbedEntityLocalTM.SetTranslation(targetLocalLocation);
                }
                AZ::TransformBus::Event(m_grabbedEntityId, &AZ::TransformInterface::SetWorldTM, 
                    GetEntity()->GetTransform()->GetWorldTM() * m_grabbedEntityLocalTM);
            }
        }
    }

    void CryptGrabberComponent::DisplayEntityViewport(
        [[maybe_unused]] const AzFramework::ViewportInfo& viewportInfo,
        AzFramework::DebugDisplayRequests& debugDisplay)
    {
        if (!m_debug || IsGrabbing())
        {
            return;
        }

        auto hitResult = CheckHit();

        if (hitResult.has_value())
        {
            debugDisplay.SetColor(AZ::Colors::Green);
            debugDisplay.DrawWireSphere(hitResult->m_position, 0.1f);
        }

        debugDisplay.PushMatrix(GetEntity()->GetTransform()->GetWorldTM());

        // Draw sphere at max grab distance. Red if it's colliding, blue if it's not.
        debugDisplay.SetColor(hitResult.has_value() ? AZ::Colors::Red : AZ::Colors::Blue);
        debugDisplay.DrawWireSphere(AZ::Vector3::CreateAxisY(m_maxGrabDistance), m_grabSphereRadius);

        debugDisplay.PopMatrix();
    }

    AZStd::optional<AzPhysics::SceneQueryHit> CryptGrabberComponent::CheckHit() const
    {
        AzPhysics::ShapeCastRequest request = AzPhysics::ShapeCastRequestHelpers::CreateSphereCastRequest(
            m_grabSphereRadius,
            GetEntity()->GetTransform()->GetWorldTM(),
            GetEntity()->GetTransform()->GetWorldTM().GetBasisY(),
            m_maxGrabDistance,
            AzPhysics::SceneQuery::QueryType::StaticAndDynamic,
            m_collisionGroup,
            nullptr);

        auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
        AzPhysics::SceneHandle sceneHandle = sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
        AzPhysics::SceneQueryHits hits;
        if (sceneInterface->QueryScene(sceneHandle, &request, hits))
        {
            return { hits.m_hits.front() };
        }
        else
        {
            return AZStd::nullopt;
        }
    }
} // namespace CourseProject
