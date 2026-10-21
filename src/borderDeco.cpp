#include "config/shared/Types.hpp"
#include "desktop/DesktopTypes.hpp"
#include "globals.hpp"
#include "render/decorations/DecorationPositioner.hpp"
#include "borderDeco.hpp"

#include <hyprland/src/desktop/view/window/Window.hpp>
#include <hyprland/src/render/decorations/IHyprWindowDecoration.hpp>
#include <hyprland/src/render/Renderer.hpp>

using namespace Render::GL;

static CHyprColor configColor(Config::INTEGER color) {
  return CHyprColor{sc<uint64_t>(color)};
}

Hyprwin95Border::Hyprwin95Border(PHLWINDOW pwindow) : IHyprWindowDecoration(pwindow) {
  this->pWindow = pwindow;

  const auto PMONITOR = pWindow->m_monitor.lock();
  if (PMONITOR) PMONITOR->m_scheduledRecalc = true;
}

Hyprwin95Border::~Hyprwin95Border() {
  std::erase(globalState->borders, pSelf);
}

void                       Hyprwin95Border::updateRules() {}
void                       Hyprwin95Border::onConfigReloaded() {}
SDecorationPositioningInfo Hyprwin95Border::getPositioningInfo() {
  return SDecorationPositioningInfo{};
}
void Hyprwin95Border::onPositioningReply(const SDecorationPositioningReply& reply) {}
void Hyprwin95Border::draw(Render::CRenderContext& ctx, PHLMONITOR pMonitor, float const& a,
                           const Render::SWindowRenderPresentation& renderRepresentation) {}
eDecorationType Hyprwin95Border::getDecorationType() {
  return eDecorationType::DECORATION_CUSTOM;
}
void     Hyprwin95Border::updateWindow(PHLWINDOW pWindow) {};
void     Hyprwin95Border::damageEntire() {}
uint64_t Hyprwin95Border::getDecorationFlags() {
  return 0;
}
eDecorationLayer Hyprwin95Border::getDecorationLayer() {
  return eDecorationLayer::DECORATION_LAYER_OVER;
}
std::string Hyprwin95Border::getDisplayName() {
  return "Hyprwin95";
}
PHLWINDOW Hyprwin95Border::getOwner() {
  return pWindow.lock();
}
