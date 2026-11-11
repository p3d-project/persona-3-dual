#include "VmSystem.hpp"
#include "VmNatives.hpp"

#include "core/logger.hpp"

constexpr int MAX_STEPS_PER_FRAME = 100;

void VmSystem::Init()
{
}

void VmSystem::Shutdown()
{
}

void VmSystem::Update(ae::q20_12_t)
{
    if (ctx.status == VmStatus::STOPPED || ctx.status == VmStatus::ERROR)
        return;

    ctx.status = VmStatus::RUNNING;
    ctx.steps = 0;

    while (ctx.steps < MAX_STEPS_PER_FRAME)
    {
        ctx.status = step();
        ctx.steps++;

        if (ctx.status != VmStatus::RUNNING)
            break;
    }
}

VmStatus VmSystem::step()
{
    if (ctx.script == nullptr)
    {
        return VmStatus::STOPPED;
    }

    const VmInstruction& instruction = ctx.script[ctx.pc++];

    switch (instruction.opcode)
    {
    case VmOpcode::CALL_NATIVE:
    {
        if (instruction.operand >= gVmNativesCount || instruction.operand < 0)
        {
            logMessage(LogSeverity::ERROR, "VM: Invalid native function index");
            return VmStatus::ERROR;
        }

        const VmNativeDesc& descriptor = gVmNatives[instruction.operand];
        if (!ctx.nativeCallPending)
        {
            if (ctx.stack.size() < descriptor.paramCount)
            {
                logMessage(LogSeverity::ERROR, "VM: Not enough arguments on stack for native call");
                return VmStatus::ERROR;
            }

            ctx.nativeCallArgsIndex = static_cast<uint16_t>(ctx.stack.size() - descriptor.paramCount);
        }

        VmNativeResult result = descriptor.func(ctx);
        if (result == VmNativeResult::PENDING)
        {
            ctx.nativeCallPending = true;
            ctx.pc--;
            return VmStatus::YIELD;
        }

        ctx.nativeCallPending = false;
        ctx.stack.truncate(ctx.nativeCallArgsIndex);
        if (descriptor.returnsValue)
        {
            ctx.stack.push(ctx.nativeCallReturnValue, ctx.nativeCallReturnType);
        }
        break;
    }

    case VmOpcode::PUSH_INT:
    {
        ctx.stack.pushInt(static_cast<int32_t>(instruction.operand));
        break;
    }

    case VmOpcode::PUSH_FLOAT:
    {
        ctx.stack.pushFloat(ae::q20_12_t{static_cast<int32_t>(instruction.operand)});
        break;
    }
    case VmOpcode::ADD:
    {
        if (ctx.stack.size() < 2)
        {
            logMessage(LogSeverity::ERROR, "VM: Not enough values on stack for ADD");
            return VmStatus::ERROR;
        }
        VmStackType type1 = ctx.stack.topType(0);
        VmStackType type2 = ctx.stack.topType(1);
        if (type1 != type2)
        {
            logMessage(LogSeverity::ERROR, "VM: Type mismatch for ADD");
            return VmStatus::ERROR;
        }

        if (type1 == VmStackType::INTEGER)
        {
            int32_t b = ctx.stack.popInt();
            int32_t a = ctx.stack.popInt();
            ctx.stack.pushInt(a + b);
        }
        else if (type1 == VmStackType::FLOAT)
        {
            ae::q20_12_t b = ctx.stack.popFloat();
            ae::q20_12_t a = ctx.stack.popFloat();
            ctx.stack.pushFloat(a + b);
        }
        break;
    }

    case VmOpcode::END:
    {
        return VmStatus::STOPPED;
    }

    default:
        return VmStatus::ERROR;
    }

    return VmStatus::RUNNING;
}

void VmSystem::setScript(const VmInstruction* script)
{
    ctx.reset(script);
    ctx.status = VmStatus::RUNNING;
}
