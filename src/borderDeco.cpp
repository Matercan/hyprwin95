#include "config/ConfigValue.hpp"
#include "config/shared/Types.hpp"
#include "config/shared/complex/ComplexDataTypes.hpp"
#include "debug/log/Logger.hpp"
#include "desktop/DesktopTypes.hpp"
#include "globals.hpp"
#include "hyprutils/cli/Logger.hpp"
#include "hyprutils/memory/UniquePtr.hpp"
#include "render/OpenGL.hpp"
#include "render/decorations/DecorationPositioner.hpp"

#include "borderppPassElement.hpp"
#include "borderDeco.hpp"

#include <hyprland/src/render/decorations/CHyprBorderDecoration.hpp>
#include <hyprland/src/desktop/view/window/WindowPresentation.hpp>
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
  LOG(Hyprutils::CLI::LOG_DEBUG, "[Hyprwin95] draw() called for window");
  if (!Desktop::View::validMapped(pWindow)) return;

  const auto PHLWINDOW = pWindow.lock();

  if (!PHLWINDOW->m_ruleApplicator->decorate().valueOrDefault()) return;

  Hyprwin95PPPassElement::SBorderPPData data;
  data.deco = this;
  data.a    = 1.0F;

  LOG(Hyprutils::CLI::LOG_DEBUG, "Adding pass element");
  g_pHyprRenderer->addPassElement(ctx, makeUnique<Hyprwin95PPPassElement>(data));
}

void Hyprwin95Border::drawPass(Render::CRenderContext& ctx, PHLMONITOR pMonitor, float const& a) {
  const auto PWINDOW = pWindow.lock();

  static auto PBORDERSIZE = CConfigValue<Config::INTEGER>("general:border_size");

  if (bAssignedGeomtry.width < seExtents.topLeft.x + 1 ||
      bAssignedGeomtry.height < seExtents.topLeft.y + 1)
    return;

  const auto PWORKSPACE = PWINDOW->m_workspace;
  const auto WORKSPACE_OFFSET =
    PWORKSPACE && !(PWINDOW->m_state & Desktop::View::WINDOW_STATE_PINNED)
      ? PWORKSPACE->m_renderOffset->value()
      : Vector2D();

  CBox fullBox = bAssignedGeomtry;
  fullBox.translate(g_pDecorationPositioner->getEdgeDefinedPoint(
    DECORATION_EDGE_BOTTOM | DECORATION_EDGE_LEFT | DECORATION_EDGE_TOP | DECORATION_EDGE_RIGHT,
    PWINDOW));

  fullBox.translate(PWINDOW->presentation().floatingOffset() - pMonitor->m_position +
                    WORKSPACE_OFFSET);

  if (fullBox.width < 1 || fullBox.height < 1) return;

  double fullThickness = globalState->config.borderSize->value() == 0
                           ? *PBORDERSIZE
                           : globalState->config.borderSize->value();

  fullBox.scale(pMonitor->m_scale).round();

  double horizontalSize = fullThickness * pMonitor->m_scale;
  double verticalSize   = fullThickness * pMonitor->m_scale * 0.5f;

  if (fullBox.width < 1 || fullBox.height < 1) return;

  g_pHyprOpenGL->scissor(ctx, nullptr);

  // Top border
  CBox topBox = fullBox;
  topBox.h    = horizontalSize;
  topBox.x += verticalSize * 0.5f;
  topBox.w -= verticalSize * 2;

  // Bottom Border
  topBox.x += verticalSize * 0.5f;
  CBox bottomBox = topBox;
  bottomBox.y    = fullBox.y + fullBox.h - horizontalSize;

  // Left border
  CBox leftBox = fullBox;
  leftBox.w    = verticalSize;
  leftBox.x    = topBox.x;
  leftBox.y += horizontalSize;
  leftBox.h -= horizontalSize * 2;

  // Right border
  CBox rightBox = leftBox;
  rightBox.x    = topBox.x + topBox.w - verticalSize;

  const auto color = CHyprColor(globalState->config.barColor->value());

  g_pHyprOpenGL->renderRect(ctx, topBox, color, {});
  g_pHyprOpenGL->renderRect(ctx, bottomBox, color, {});
  g_pHyprOpenGL->renderRect(ctx, rightBox, color, {});
  g_pHyprOpenGL->renderRect(ctx, leftBox, color, {});

  seExtents = {{verticalSize, horizontalSize}, {verticalSize, horizontalSize}};

  blastRelativeBox = CBox{0, 0, lastWindowSize.x, lastWindowSize.y}.addExtents(seExtents);

  if (fullThickness != fLastThickness) {
    fLastThickness = fullThickness;
    g_pDecorationPositioner->repositionDeco(this);
  }
}
