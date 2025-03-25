#include"Event/LSEventDispatcher.h"
#include"spdlog/stopwatch.h"    //提供程序的CPU耗时
#include"LSLog.h"

namespace lse{
    LSEventDispatcher LSEventDispatcher::s_Instance{};

    LSEventDispatcher::~LSEventDispatcher(){
        mObserverHandlerMap.clear();
    }

    void LSEventDispatcher::Dispatch(LSEvent &event){
        if(mObserverHandlerMap.find(event.GetEventType()) == mObserverHandlerMap.end()){
            return;
        }
        auto observers = mObserverHandlerMap[event.GetEventType()];
        if(observers.empty()){
            return;
        }

        spdlog::stopwatch stopwatch;
        stopwatch.reset();
        for (const auto &observer: observers){
            if(observer.observer){
                observer.func(event);
            }
        }
        LOG_T("observer count: {0}, timing: {1:.2}ms", observers.size(), stopwatch.elapsed().count() * 1000);

    }
}