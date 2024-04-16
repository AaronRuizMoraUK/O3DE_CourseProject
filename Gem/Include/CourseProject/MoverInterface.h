
#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace CourseProject
{
    class MoverRequests
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(CourseProject::MoverRequests, "{E8225712-4782-4FF4-8074-E88C5A7A78F1}");

        // Put your public request methods here.
        
        // Put notification events here. Examples:
        // void RegisterEvent(AZ::EventHandler<...> notifyHandler);
        // AZ::Event<...> m_notifyEvent1;
        
    };

    using MoverRequestBus = AZ::EBus<MoverRequests>;

} // namespace CourseProject
