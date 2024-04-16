
#include <AzCore/Serialization/SerializeContext.h>

#include "CourseProjectSystemComponent.h"

#include <CourseProject/CourseProjectTypeIds.h>

namespace CourseProject
{
    AZ_COMPONENT_IMPL(CourseProjectSystemComponent, "CourseProjectSystemComponent",
        CourseProjectSystemComponentTypeId);

    void CourseProjectSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<CourseProjectSystemComponent, AZ::Component>()
                ->Version(0)
                ;
        }
    }

    void CourseProjectSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("CourseProjectService"));
    }

    void CourseProjectSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("CourseProjectService"));
    }

    void CourseProjectSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void CourseProjectSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    CourseProjectSystemComponent::CourseProjectSystemComponent()
    {
        if (CourseProjectInterface::Get() == nullptr)
        {
            CourseProjectInterface::Register(this);
        }
    }

    CourseProjectSystemComponent::~CourseProjectSystemComponent()
    {
        if (CourseProjectInterface::Get() == this)
        {
            CourseProjectInterface::Unregister(this);
        }
    }

    void CourseProjectSystemComponent::Init()
    {
    }

    void CourseProjectSystemComponent::Activate()
    {
        CourseProjectRequestBus::Handler::BusConnect();
    }

    void CourseProjectSystemComponent::Deactivate()
    {
        CourseProjectRequestBus::Handler::BusDisconnect();
    }
}
