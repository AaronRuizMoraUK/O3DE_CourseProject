
#pragma once

#include <CourseProject/CourseProjectTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace CourseProject
{
    class CourseProjectRequests
    {
    public:
        AZ_RTTI(CourseProjectRequests, CourseProjectRequestsTypeId);
        virtual ~CourseProjectRequests() = default;
        // Put your public methods here
    };

    class CourseProjectBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using CourseProjectRequestBus = AZ::EBus<CourseProjectRequests, CourseProjectBusTraits>;
    using CourseProjectInterface = AZ::Interface<CourseProjectRequests>;

} // namespace CourseProject
