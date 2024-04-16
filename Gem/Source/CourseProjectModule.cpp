
#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include "CourseProjectSystemComponent.h"
#include "MoverComponent.h"
#include "CryptMoverComponent.h"
#include "CryptGrabberComponent.h"
#include "CryptTriggerComponent.h"

#include <CourseProject/CourseProjectTypeIds.h>

namespace CourseProject
{
    class CourseProjectModule
        : public AZ::Module
    {
    public:
        AZ_RTTI(CourseProjectModule, CourseProjectModuleTypeId, AZ::Module);
        AZ_CLASS_ALLOCATOR(CourseProjectModule, AZ::SystemAllocator);

        CourseProjectModule()
            : AZ::Module()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                MoverComponent::CreateDescriptor(),
                CryptMoverComponent::CreateDescriptor(),
                CryptGrabberComponent::CreateDescriptor(),
                CryptTriggerComponent::CreateDescriptor(),

                CourseProjectSystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<CourseProjectSystemComponent>(),
            };
        }
    };
}// namespace CourseProject

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME), CourseProject::CourseProjectModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_CourseProject, CourseProject::CourseProjectModule)
#endif
