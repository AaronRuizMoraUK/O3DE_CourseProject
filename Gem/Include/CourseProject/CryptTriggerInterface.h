
#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace CourseProject
{
    class CryptTriggerRequests
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(CourseProject::CryptTriggerRequests, "{96E950E9-7C2A-4F96-9BA0-4F6B11622E15}");

        // Put your public request methods here.
        
        // Put notification events here. Examples:
        // void RegisterEvent(AZ::EventHandler<...> notifyHandler);
        // AZ::Event<...> m_notifyEvent1;
        
    };

    using CryptTriggerRequestBus = AZ::EBus<CryptTriggerRequests>;

} // namespace CourseProject
