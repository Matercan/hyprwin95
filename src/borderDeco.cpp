#include "config/shared/Types.hpp"
#include "desktop/DesktopTypes.hpp"
#include "globals.hpp"
#include "hyprutils/memory/UniquePtr.hpp"
#include "render/decorations/DecorationPositioner.hpp"
#include "render/pass/BorderPassElement.hpp"
#include "borderDeco.hpp"

#include <hyprland/src/render/decorations/CHyprBorderDecoration.hpp>
#include <hyprland/src/desktop/view/window/Window.hpp>
#include <hyprland/src/render/decorations/IHyprWindowDecoration.hpp>
#include <hyprland/src/render/Renderer.hpp>

using namespace Render::GL;

static CHyprColor configColor(Config::INTEGER color) {
  return CHyprColor{sc<uint64_t>(color)};
}

Hyprwin95Border::Hyprwin95Border(PHLWINDOW pwindow) : IHyprWindowDecoration(pwindow) {
  this->pWindow        = pwindow;
  this->lastWindowPos  = pWindow->position(Desktop::View::IGeometric::GEOMETRIC_CURRENT);
  this->lastWindowSize = pWindow->size(Desktop::View::IGeometric::GEOMETRIC_CURRENT);

  const auto PMONITOR = pWindow->m_monitor.lock();
  if (PMONITOR) PMONITOR->m_scheduledRecalc = true;
}

Hyprwin95Border::~Hyprwin95Border() {
  damageEntire();
  std::erase(globalState->borders, pSelf);
}

void Hyprwin95Border::updateRules() {}

void Hyprwin95Border::onConfigReloaded() {

  damageEntire();
}
SDecorationPositioningInfo Hyprwin95Border::getPositioningInfo() {
  const auto PHWINDOW = pWindow.lock();

  SDecorationPositioningInfo info;
  info.policy   = DECORATION_POSITION_STICKY;
  info.reserved = true;
  info.priority = 9990;
  info.edges =
    DECORATION_EDGE_BOTTOM | DECORATION_EDGE_LEFT | DECORATION_EDGE_TOP | DECORATION_EDGE_RIGHT;

  if (fLastThickness == 0) {
    double size         = globalState->config.borderSize->value();
    info.desiredExtents = {{size, size}, {size, size}};
    fLastThickness      = size;
  } else if (!PHWINDOW->m_ruleApplicator->decorate().valueOrDefault()) {
    int size            = 0;
    info.desiredExtents = {{size, size}, {size, size}};
  } else {
    info.desiredExtents = {{fLastThickness, fLastThickness}, {fLastThickness, fLastThickness}};
  }

  return info;
}
void Hyprwin95Border::onPositioningReply(const SDecorationPositioningReply& reply) {
  bAssignedGeomtry = reply.assignedGeometry;
}

void Hyprwin95Border::updateWindow(PHLWINDOW pWindow) {
  this->lastWindowPos  = pWindow->position(Desktop::View::IGeometric::GEOMETRIC_CURRENT);
  this->lastWindowSize = pWindow->size(Desktop::View::IGeometric::GEOMETRIC_CURRENT);

  damageEntire();
};
void Hyprwin95Border::damageEntire() {
  CBox dm = blastRelativeBox.copy().translate(lastWindowPos).expand(2);
  g_pHyprRenderer->damageBox(dm);
}

eDecorationType Hyprwin95Border::getDecorationType() {
  return eDecorationType::DECORATION_CUSTOM;
}
uint64_t Hyprwin95Border::getDecorationFlags() {
  return DECORATION_PART_OF_MAIN_WINDOW;
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

void Hyprwin95Border::draw(Render::CRenderContext& ctx, PHLMONITOR pMonitor, float const& a,
                           const Render::SWindowRenderPresentation& renderRepresentation) {
  if (!Desktop::View::validMapped(pWindow)) return;

  const auto PHLWINDOW = pWindow.lock();

  if (!PHLWINDOW->m_ruleApplicator->decorate().valueOrDefault()) return;

  CBorderPassElement::SBorderData data;
  data.window     = getOwner();
  data.a          = a;
  data.box        = blastRelativeBox.translate(lastWindowPos).expand(2);
  data.borderSize = fLastThickness;
  data.grad1      = configColor(globalState->config.borderColor->value());

  g_pHyprRenderer->addPassElement(ctx, makeUnique<CBorderPassElement>(data));
}
