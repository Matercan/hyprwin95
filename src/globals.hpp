#pragma once

#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/render/Texture.hpp>
#include <hyprland/src/helpers/memory/Memory.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/config/values/types/BoolValue.hpp>
#include <hyprland/src/config/values/types/IntValue.hpp>
#include <hyprland/src/config/values/types/StringValue.hpp>
#include <hyprland/src/config/values/types/ColorValue.hpp>

#include <optional>
#include <vector>

inline HANDLE PHANDLE = nullptr;

struct Hyprwin95Button {
  std::string          cmd   = "";
  CHyprColor           fgCol = CHyprColor(0, 0, 0, 0);
  CHyprColor           bgcol = CHyprColor(0, 0, 0, 0);
  float                size  = 10;
  std::string          icon  = "";
  SP<Render::ITexture> iconTex;
  std::optional<float> m_fIconScale;
};

class Hyprwin95Bar;

struct GlobalState {
  std::vector<Hyprwin95Button> buttons;
  WP<Hyprwin95Bar>             bar;
  uint32_t                     barColorRuleIdx   = 0;
  uint32_t                     titleColorRuleidx = 0;
  uint32_t                     borderColorRuleIdx = 0;

  struct {
    SP<Config::Values::CColorValue> barColor, textColor, activeButtonColor, borderColor;
    SP<Config::Values::CIntValue> barHeight, barTextSize, barPadding, buttonPadding, borderSize;
    SP<Config::Values::CStringValue> barTextFont;
  } config;
};

inline UP<GlobalState> globalState;
