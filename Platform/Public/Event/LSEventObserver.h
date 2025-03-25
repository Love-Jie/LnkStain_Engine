#pragma once

#include"LSEventDispatcher.h"

namespace lse{
    class LSEventObserver{
    public:
        LSEventObserver() = default;
        ~LSEventObserver() {
            LSEventDispatcher::GetInstance()->DestroyObserver(this);
        }

        template<class T>
        void OnEvent(const std::function<void(const T &)> &func){
            LSEventDispatcher::GetInstance()->AddObserverHandler(this,func);
        }
    };

}