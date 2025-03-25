#pragma once

#include "Event/LSEventObserver.h"

class LSEventTesting{
public:
    LSEventTesting();
    ~LSEventTesting();
private:

    void TestMemberFunc(const lse::LSMouseButtonReleaseEvent &event);
    std::shared_ptr<lse::LSEventObserver>mObserver;
};