#pragma once
#include "globals.hpp"
#include <hyprland/src/render/pass/PassElement.hpp>

class Hyprwin95PPPassElement : public IPassElement {
public:
  struct SBorderPPData {
    Hyprwin95Border* deco = nullptr;
    float            a    = 1.F;
  };

  Hyprwin95PPPassElement(const SBorderPPData& data_);
  virtual ~Hyprwin95PPPassElement() = default;

  virtual std::vector<UP<IPassElement>> draw(Render::CRenderContext& ctx) override;
  virtual bool                          needsLiveBlur(Render::CRenderContext& ctx) override;
  virtual bool                          needsPrecomputeBlur(Render::CRenderContext& ctx) override;

  virtual const char* passName() override {
    return "Hyprwin95PPPassElement";
  }

  virtual ePassElementType type() override {
    return EK_CUSTOM;
  }

private:
  SBorderPPData data;
};
