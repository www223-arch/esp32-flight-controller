#pragma once
#include <memory>
#include "RCBase.hpp"

struct RCFactory {
    // create by name (runtime selection). Returns unique_ptr owning the adapter.
    static std::unique_ptr<RCBase> create(const char* name);
};
