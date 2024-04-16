
#pragma once

#include <AzCore/Component/ComponentBus.h>

#include <LmbrCentral/Scripting/TagComponentBus.h>

namespace CourseProject
{
    constexpr LmbrCentral::Tag GrabbedTag = LmbrCentral::Tag("Grabbed");

    class CryptGrabberRequests
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(CourseProject::CryptGrabberRequests, "{5BFD96AC-357E-4679-BE76-D2A588BBEE10}");

        // Put your public request methods here.
        virtual void Grab() = 0;

        virtual void Release() = 0;

        virtual bool IsGrabbing() const = 0;

        virtual bool IsDebugEnabled() const = 0;

        virtual void EnableDebug(bool enable) = 0;
        
        // Put notification events here. Examples:
        // void RegisterEvent(AZ::EventHandler<...> notifyHandler);
        // AZ::Event<...> m_notifyEvent1;
        
    };

    using CryptGrabberRequestBus = AZ::EBus<CryptGrabberRequests>;

} // namespace CourseProject
