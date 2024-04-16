
#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace CourseProject
{
    class CryptGrabberRequests
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(CourseProject::CryptGrabberRequests, "{5BFD96AC-357E-4679-BE76-D2A588BBEE10}");

        // Put your public request methods here.
        
        // Put notification events here. Examples:
        // void RegisterEvent(AZ::EventHandler<...> notifyHandler);
        // AZ::Event<...> m_notifyEvent1;
        
    };

    using CryptGrabberRequestBus = AZ::EBus<CryptGrabberRequests>;

} // namespace CourseProject
