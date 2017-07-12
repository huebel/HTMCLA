#pragma once

#include "MemObjectType.h"

namespace htm {

class MemObject {
public:
    virtual MemObjectType GetMemObjectType() = 0;
    virtual void Employ() {} // Prepare the object to be used/reused
    virtual void Retire() {} // Prepare the object to be released
private:
    MemObject *mem_next;
friend
    class MemManager;
};

} // namespace htm
