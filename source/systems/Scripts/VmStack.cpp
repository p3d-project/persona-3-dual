#include "VmStack.hpp"

void VmStack::push(VmValue value, VmStackType type)
{
    values.push_back(value);
    types.push_back(type);
}

void VmStack::pushInt(int32_t value)
{
    push({.i = value}, VmStackType::INTEGER);
}

int32_t VmStack::popInt()
{
    int32_t value = top().i;
    values.pop_back();
    types.pop_back();
    return value;
}

void VmStack::pushFloat(ae::q20_12_t value)
{
    push({.q = value}, VmStackType::FLOAT);
}

ae::q20_12_t VmStack::popFloat()
{
    ae::q20_12_t value = top().q;
    values.pop_back();
    types.pop_back();
    return value;
}

size_t VmStack::size() const
{
    return values.size();
}

VmValue& VmStack::top(size_t offset)
{
    return values[index(offset)];
}

VmStackType VmStack::topType(size_t offset) const
{
    return types[index(offset)];
}

void VmStack::truncate(size_t size)
{
    values.resize(size);
    types.resize(size);
}
