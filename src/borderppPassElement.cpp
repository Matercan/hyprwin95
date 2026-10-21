#include "borderppPassElement.hpp"
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include "borderDeco.hpp"

Hyprwin95PPPassElement::Hyprwin95PPPassElement(const Hyprwin95PPPassElement::SBorderPPData& data_)
    : data(data_) {
  ;
}

std::vector<UP<IPassElement>> Hyprwin95PPPassElement::draw(Render::CRenderContext& ctx) {
  data.deco->drawPass(ctx, ctx.m_data.pMonitor.lock(), data.a);
  return {};
}

bool Hyprwin95PPPassElement::needsLiveBlur(Render::CRenderContext& ctx) {
  return false;
}

bool Hyprwin95PPPassElement::needsPrecomputeBlur(Render::CRenderContext& ctx) {
  return false;
}
