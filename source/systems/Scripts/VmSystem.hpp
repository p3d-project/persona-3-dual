#pragma once

#include "VmContext.hpp"
#include "types/VmTypes.hpp"
#include <aegis/system.hpp>

class VmSystem : public ae::System, public ae::Singleton<VmSystem>
{
  public:
    void Init() override;
    void Shutdown() override;
    void Update(ae::q20_12_t dt) override;

    void setActive(bool active)
    {
        isActive = active;
    }

    void setScript(const VmInstruction* script);

  private:
    friend class Singleton<VmSystem>;
    VmContext ctx;

    VmStatus step();
};
