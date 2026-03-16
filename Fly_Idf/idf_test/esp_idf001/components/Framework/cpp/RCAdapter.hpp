#pragma once
#include "RCBase.hpp"
#include "../drivers/rc/rc.h"

class RCAdapter : public RCBase {
public:
    explicit RCAdapter(rc_dev_t* dev);
    ~RCAdapter();
    const char* name() const override;
    bool init() override;
    int read_channels(float* out, int max_chans) override;
    void deinit() override;
private:
    rc_dev_t* dev_ = nullptr;
};
