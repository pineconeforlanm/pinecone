//
// Created by 28192 on 2024/9/14.
//

#ifndef PINECONE_SINGLETON_H
#define PINECONE_SINGLETON_H


#include <memory>

namespace pinecone::util {
    template<class T>
    class Singleton {
    public:
        static auto get_instance() -> T * {
            static T s_ptr;
            return &s_ptr;
        }
    };

    template<class T>
    class SingletonPtr {
    public:
        static auto get_instance() -> std::shared_ptr<T> {
            static auto s_ptr(std::make_shared<T>());
            return s_ptr;
        }
    };

}

#endif //PINECONE_SINGLETON_H
