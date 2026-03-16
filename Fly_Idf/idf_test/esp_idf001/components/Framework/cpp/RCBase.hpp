#pragma once

class RCBase {
public:
    virtual ~RCBase() {}
    virtual const char* name() const = 0;
    virtual bool init() = 0;
    virtual int read_channels(float* out, int max_chans) = 0;
    virtual void deinit() = 0;
};
