
#pragma once

#include <AzCore/Component/Component.h>

#include <CourseProject/CourseProjectBus.h>

namespace CourseProject
{
    class CourseProjectSystemComponent
        : public AZ::Component
        , protected CourseProjectRequestBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(CourseProjectSystemComponent);

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        CourseProjectSystemComponent();
        ~CourseProjectSystemComponent();

    protected:
        ////////////////////////////////////////////////////////////////////////
        // CourseProjectRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
    };
}
