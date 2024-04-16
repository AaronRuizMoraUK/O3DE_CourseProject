
#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace CourseProject
{
    class CryptMoverRequests
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(CourseProject::CryptMoverRequests, "{0FF72FC5-37B4-4B97-8C5B-8061E435F274}");

        // Put your public request methods here.
        virtual bool GetShouldMove() const = 0;
        virtual void SetShouldMove(bool shouldMove) = 0;
        
        // Put notification events here. Examples:
        // void RegisterEvent(AZ::EventHandler<...> notifyHandler);
        // AZ::Event<...> m_notifyEvent1;
        
    };

    using CryptMoverRequestBus = AZ::EBus<CryptMoverRequests>;

} // namespace CourseProject
