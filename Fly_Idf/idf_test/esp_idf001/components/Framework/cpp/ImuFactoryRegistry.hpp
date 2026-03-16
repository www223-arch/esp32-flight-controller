#pragma once
#include <string>
#include <functional>
#include <memory>
#include "ImuBase.hpp"

using ImuCreator = std::function<std::unique_ptr<ImuBase>(void)>;

class ImuFactoryRegistry {
public:
    static ImuFactoryRegistry& instance();
    void register_factory(const std::string& name, ImuCreator c);
    std::unique_ptr<ImuBase> create(const std::string& name);

private:
    ImuFactoryRegistry();
    ~ImuFactoryRegistry();
    ImuFactoryRegistry(const ImuFactoryRegistry&) = delete;
    ImuFactoryRegistry& operator=(const ImuFactoryRegistry&) = delete;
    struct Impl;
    Impl* pimpl_ = nullptr;
};
