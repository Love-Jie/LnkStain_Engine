#include "LSEventTesting.h"

#include "LSLog.h"

#define BIND_EVENT_FN(fn)           [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
#define REG_EVENT(type) mObserver->OnEvent<type>([](const type &event){ LOG_T(event.ToString()); });

static void TestStaticFunc(const lse::LSMouseButtonPressEvent &event){
    LOG_T(event.ToString());
}

void LSEventTesting::TestMemberFunc(const lse::LSMouseButtonReleaseEvent &event) {
    LOG_T(event.ToString());
}

LSEventTesting::LSEventTesting() {
    mObserver = std::make_shared<lse::LSEventObserver>();
    mObserver->OnEvent<lse::LSFramebufferResizeEvent>([](const lse::LSFramebufferResizeEvent &event){ LOG_T(event.ToString()); });
    mObserver->OnEvent<lse::LSMouseButtonPressEvent>(TestStaticFunc);
    mObserver->OnEvent<lse::LSMouseButtonReleaseEvent>(BIND_EVENT_FN(TestMemberFunc));
    REG_EVENT(lse::LSWindowFocusEvent);
    REG_EVENT(lse::LSWindowLostFocusEvent);
    REG_EVENT(lse::LSWindowMovedEvent);
    REG_EVENT(lse::LSWindowCloseEvent);
    REG_EVENT(lse::LSKeyPressEvent);
    REG_EVENT(lse::LSKeyReleaseEvent);
    //    REG_EVENT(ade::AdMouseMovedEvent);
    REG_EVENT(lse::LSMouseScrollEvent);
}

LSEventTesting::~LSEventTesting() {
    mObserver.reset();
}