//
// Created by erik9 on 5/8/2023.
//

#ifndef LIIINJECTOR_INJECTOR_HPP
#define LIIINJECTOR_INJECTOR_HPP

#include <string>
#include <memory>
#include <unordered_map>
#include <stdexcept>
#include <typeindex>
#include <functional>
#include "Injectable.h"

namespace LiiInjector
{
    class Injector
    {
    private:
        static Injector* const instance;
        std::unordered_map<std::string, std::unique_ptr<Injectable>> tagSingletons;
        std::unordered_map<std::type_index, std::unique_ptr<Injectable>> singletons;

        std::unordered_map<std::string, std::function<Injectable*()>> transientTag;
        std::unordered_map<std::type_index,std::function<Injectable*()>> transient;
    public:
        static Injector& GetInstance()
        {
            return *instance;
        }

        template<typename T>
        [[maybe_unused]] void RegisterSingleton(const std::string& tag)
        {
            static_assert(std::is_base_of<Injectable, T>::value, "T must be a child of Injectable");
            std::unique_ptr<T> singleton = std::unique_ptr<T>(new T());
            auto result = tagSingletons.try_emplace(tag, std::move(singleton));
            if (!result.second)
                throw std::runtime_error("Singleton already registered!");
        }

        template<typename T>
        [[maybe_unused]] void RegisterSingleton()
        {
            static_assert(std::is_base_of<Injectable, T>::value, "T must be a child of Injectable");
            std::unique_ptr<T> singleton = std::unique_ptr<T>(new T());
            auto result = singletons.try_emplace(typeid(T), std::move(singleton));
            if (!result.second)
                throw std::runtime_error("Singleton already registered!");
        }

        template<typename T>
        [[maybe_unused]] void RegisterSingleton(const std::function <std::unique_ptr<Injectable>()>& factoryFunction)
        {
            static_assert(std::is_base_of<Injectable, T>::value, "T must be a child of Injectable");
            auto result = singletons.try_emplace(typeid(T), factoryFunction());
            if (!result.second)
                throw std::runtime_error("Singleton already registered!");
        }

        template<typename T>
        [[maybe_unused]] void RegisterSingleton(const std::function <std::unique_ptr<Injectable>()>& function, const std::string& tag)
        {
            static_assert(std::is_base_of<Injectable, T>::value, "T must be a child of Injectable");
            auto result = tagSingletons.try_emplace(tag, function());

            if (!result.second)
                throw std::runtime_error("Singleton already registered!");
        }

        template<class T>
        T* ResolveSingleton(const std::string& tag)
        {
            static_assert(std::is_base_of<Injectable, T>::value, "T must be a child of Injectable");
            auto it = tagSingletons.find(tag);
            if (it == tagSingletons.end())
                throw std::runtime_error("Singleton not registered!");
            auto* result = dynamic_cast<T*>(it->second.get());
            if(result == nullptr)
                throw std::runtime_error("Singleton type mismatch!");
            return result;
        }

        template<class T>
        T* ResolveSingleton()
        {
            static_assert(std::is_base_of<Injectable, T>::value, "T must be a child of Injectable");
            auto it = singletons.find(typeid(T));
            if (it == singletons.end())
                throw std::runtime_error("Singleton not registered!");
            auto* result = dynamic_cast<T*>(it->second.get());
            if(result == nullptr)
                throw std::runtime_error("Singleton type mismatch!");
            return result;
        }

        template<typename T>
        [[maybe_unused]] void RegisterTransient()
        {
            static_assert(std::is_base_of<Injectable, T>::value, "T must be a child of Injectable");
            auto result = transient.try_emplace(typeid(T), [](){return new T();});

            if (!result.second)
                throw std::runtime_error("Type already registered!");
        }

        template<typename T>
        [[maybe_unused]] void RegisterTransient(const std::string& tag)
        {
            static_assert(std::is_base_of<Injectable, T>::value, "T must be a child of Injectable");
            auto result = transientTag.try_emplace(tag, [](){return new T();});

            if (!result.second)
                throw std::runtime_error("Tag already registered!");
        }

        template<typename T>
        [[maybe_unused]] void RegisterTransient(const std::function<Injectable*()>& factoryFunction)
        {
            static_assert(std::is_base_of<Injectable, T>::value, "T must be a child of Injectable");
            auto result = transient.try_emplace(typeid(T), factoryFunction);

            if (!result.second)
                throw std::runtime_error("Type already registered!");
        }

        template<typename T>
        [[maybe_unused]] void
        RegisterTransient(const std::function<Injectable * ()> &factoryFunction, const std::string &tag)
        {
            static_assert(std::is_base_of<Injectable, T>::value, "T must be a child of Injectable");
            auto result = transientTag.try_emplace(tag, factoryFunction);

            if (!result.second)
                throw std::runtime_error("tag already registered!");
        }

        template<typename T>
        std::unique_ptr<T> ResolveTransient()
        {
            static_assert(std::is_base_of<Injectable, T>::value, "T must be a child of Injectable");
            auto it = transient.find(typeid(T));
            if (it == transient.end())
                throw std::runtime_error("Type not registered!");
            auto result = dynamic_cast<T*>(it->second());
            if(result == nullptr)
                throw std::runtime_error("Type mismatch!");
            return std::unique_ptr<T>(result);
        }

        template<typename T>
        std::unique_ptr<T> ResolveTransient(const std::string& tag)
        {
            static_assert(std::is_base_of<Injectable, T>::value, "T must be a child of Injectable");
            auto it = transientTag.find(tag);
            if (it == transientTag.end())
                throw std::runtime_error("Type not registered!");
            auto result = dynamic_cast<T*>(it->second());
            if(result == nullptr)
                throw std::runtime_error("Type mismatch!");
            return std::unique_ptr<T>(result);
        }


    };
}

LiiInjector::Injector* const LiiInjector::Injector::instance = new LiiInjector::Injector();

#endif //LIIINJECTOR_INJECTOR_HPP
