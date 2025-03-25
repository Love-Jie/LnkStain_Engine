#pragma once

#include"LSMouseEvent.h"
#include"LSWindowEvent.h"

//事件的发布者，负责发布事件
namespace lse{
    class LSEventObserver; //事件的订阅者

    using Func_EventHandler = std::function<void(LSEvent& e)>;

    struct EventHandler{
        LSEventObserver *observer;
        Func_EventHandler func;
    };

    class LSEventDispatcher{
    public:
        LSEventDispatcher(const LSEventDispatcher&) = delete;
        LSEventDispatcher& operator=(const LSEventDispatcher&) = delete;
        ~LSEventDispatcher();

        static LSEventDispatcher* GetInstance() { return &s_Instance; };

        //用于发布时间
        void Dispatch(LSEvent &event);

        //添加订阅者的函数
        template<class T>
        void AddObserverHandler(LSEventObserver *observer,const std::function<void(const T&)>&func){
            if(!observer || !func){
                return;
            }
            auto eventFunc = [func](const LSEvent &e){
                const T &event = static_cast<const T&>(e);
                return func(event);
            };

            EventHandler handler = {};
            handler.observer = observer;
            handler.func = eventFunc;

            mObserverHandlerMap[T::GetStaticType()].emplace_back(handler);
        };

        void DestroyObserver(LSEventObserver *observer){
            for(auto &mapIt : mObserverHandlerMap){
                mapIt.second.erase(std::remove_if(mapIt.second.begin(),mapIt.second.end(),[observer](const EventHandler &handler){
                    return(handler.observer&&handler.observer==observer);
                }),mapIt.second.end());
            }
        };


    private:
        LSEventDispatcher() = default;

        //简单的单例模式
        static LSEventDispatcher s_Instance;

        //维护一个订阅着的map
        std::unordered_map<LSEventType,std::vector<EventHandler>> mObserverHandlerMap;
    };
}