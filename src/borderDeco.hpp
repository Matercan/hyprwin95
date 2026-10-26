#include "borderppPassElement.hpp"
#include "helpers/memory/Memory.hpp"
#include <hyprland/src/desktop/DesktopTypes.hpp>
#include <hyprland/src/render/pass/ClearPassElement.hpp>
#include <hyprland/src/render/decorations/DecorationPositioner.hpp>
#include <hyprland/src/render/decorations/IHyprWindowDecoration.hpp>
#include <hyprutils/math/Box.hpp>
#include <hyprutils/math/Vector2D.hpp>

#include <cstdint>
#include "globals.hpp"

class Hyprwin95Border : public IHyprWindowDecoration {
public:
  Hyprwin95Border(PHLWINDOW);
  virtual ~Hyprwin95Border();

  virtual SDecorationPositioningInfo getPositioningInfo() override;
  virtual void             onPositioningReply(const SDecorationPositioningReply&) override;
  virtual void             draw(Render::CRenderContext&, PHLMONITOR, float const&,
                                const Render::SWindowRenderPresentation&) override;
  virtual eDecorationType  getDecorationType() override;
  virtual void             updateWindow(PHLWINDOW) override;
  virtual void             damageEntire() override;
  virtual uint64_t         getDecorationFlags() override;
  virtual eDecorationLayer getDecorationLayer() override;
  virtual std::string      getDisplayName() override;
  virtual PHLWINDOW        getOwner();
  void                     updateRules();
  void                     onConfigReloaded();

  SP<Hyprwin95Border> pSelf;
  UP<Hyprwin95Bar>    bar;

private:
  void drawPass(Render::CRenderContext&, PHLMONITOR, float const&);

  SBoxExtents  seExtents;
  PHLWINDOWREF pWindow;

  CBox blastRelativeBox;
  CBox bAssignedGeomtry;

  Vector2D lastWindowPos;
  Vector2D lastWindowSize;

  double fLastWidth = 0;
  double fLastHeight = 0;

  friend class Hyprwin95PPPassElement;
};
