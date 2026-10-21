#include <hyprland/src/render/decorations/IHyprWindowDecoration.hpp>
#include <hyprutils/math/Box.hpp>
#include "desktop/DesktopTypes.hpp"
#include "helpers/memory/Memory.hpp"
#include "render/Texture.hpp"

class Hyprwin95Bar {
public:
  WP<Hyprwin95Bar> self;
  virtual void     draw(Render::CRenderContext&, PHLMONITOR, const float&,
                        const Render::SWindowRenderPresentation&);

private:
  PHLWINDOWREF pWindow;
  SBoxExtents  seExtents;
  CBox bAssignedBox;
  SP<Render::ITexture> pTextTex;

  void renderPass(Render::CRenderContext&, PHLMONITOR, float const&);
  void renderButtons(Render::CRenderContext&, CBox*, const float, const float);
  void renderButtonsText(Render::CRenderContext&, CBox*, const float, const float);
};
