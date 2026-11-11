#pragma once

#include "types/VmTypes.hpp"

class VmStack
{
  public:
    void push(VmValue value, VmStackType type);

    void pushInt(int32_t value);
    int32_t popInt();

    void pushFloat(ae::q20_12_t value);
    ae::q20_12_t popFloat();

    size_t size() const;
    VmValue& top(size_t offset = 0);
    VmStackType topType(size_t offset = 0) const;

    void truncate(size_t size);

  private:
    etl::vector<VmValue, MAX_STACK_SIZE> values;
    etl::vector<VmStackType, MAX_STACK_SIZE> types;

    size_t index(size_t offset) const
    {
        return values.size() - 1 - offset;
    }
};
