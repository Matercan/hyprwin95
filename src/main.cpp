#include <hyprland/src/config/values/types/ColorValue.hpp>
#include <hyprland/src/config/values/types/IntValue.hpp>
#include <hyprland/src/config/values/types/StringValue.hpp>
#include <hyprutils/memory/SharedPtr.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprutils/memory/UniquePtr.hpp>
#include <hyprland/src/desktop/rule/windowRule/WindowRule.hpp>

#include <stdexcept>

#include "globals.hpp"

APICALL EXPORT std::string PLUGIN_API_VERSION() {
  return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
  PHANDLE = handle;

  const std::string COMPOSITOR_HASH = __hyprland_api_get_hash();
  const std::string CLIENT_HASH     = __hyprland_api_get_client_hash();

  if (COMPOSITOR_HASH != CLIENT_HASH) {
    HyprlandAPI::addNotification(PHANDLE, "[Hyprwin95] Mismatched headers! Can't proceed.",
                                 CHyprColor(1.0, 0.2, 0.2, 1.0), 5000);
    throw std::runtime_error("[Hyprwin95] Version mismatch");
  }

  globalState = makeUnique<GlobalState>();
  globalState->borderColorRuleIdx =
    Desktop::Rule::windowEffects()->registerEffect("hyprwin95:border_color");
  globalState->barColorRuleIdx =
    Desktop::Rule::windowEffects()->registerEffect("hyprwin95::bar_color");
  globalState->titleColorRuleidx =
    Desktop::Rule::windowEffects()->registerEffect("hyprwin95::title_color");

  globalState->config.barColor = makeShared<Config::Values::CColorValue>(
    "plugin::hyprwin95::bar_color", "Change the bar color", 0xf1cff2ff);
  globalState->config.borderColor = makeShared<Config::Values::CColorValue>(
    "plugin::hyprwin95::border_color", "Change the border color", 0x5752b0);
  globalState->config.activeButtonColor = makeShared<Config::Values::CColorValue>(
    "plugin::hyprwin95::active_button_color",
    "Change the color of the button when the mouse if over it", 0xab334cff);
  globalState->config.textColor = makeShared<Config::Values::CColorValue>(
    "plugin::hyprwin95::text_color", "Change the color of the text", 0x5e3b9bff);
  globalState->config.barHeight =
    makeShared<Config::Values::CIntValue>("plugin::hyprwin95::bar_height", "Height of the bar", 10);
  globalState->config.barTextSize = makeShared<Config::Values::CIntValue>(
    "plugin::hyprwin95::bar_text_size", "Size of bar text", 8);
  globalState->config.barPadding = makeShared<Config::Values::CIntValue>(
    "plugin::hyprwin95::bar_padding", "Difference between bar and buttons", 2);
  globalState->config.buttonPadding = makeShared<Config::Values::CIntValue>(
    "plugin::hyprwin95::button_padding", "Difference between button border and texture", 0);
  globalState->config.borderSize = makeShared<Config::Values::CIntValue>(
    "plugin::hyprwin95::border_size", "Size of the border", 1);
  globalState->config.barTextFont = makeShared<Config::Values::CStringValue>(
    "plugin::hyprwin95::bar_text_font", "Text font of the bar", "WIN95FA");

  HyprlandAPI::addConfigValueV2(PHANDLE, globalState->config.barColor);
  HyprlandAPI::addConfigValueV2(PHANDLE, globalState->config.borderColor);
  HyprlandAPI::addConfigValueV2(PHANDLE, globalState->config.activeButtonColor);
  HyprlandAPI::addConfigValueV2(PHANDLE, globalState->config.textColor);
  HyprlandAPI::addConfigValueV2(PHANDLE, globalState->config.barHeight);
  HyprlandAPI::addConfigValueV2(PHANDLE, globalState->config.barTextSize);
  HyprlandAPI::addConfigValueV2(PHANDLE, globalState->config.barPadding);
  HyprlandAPI::addConfigValueV2(PHANDLE, globalState->config.buttonPadding);
  HyprlandAPI::addConfigValueV2(PHANDLE, globalState->config.borderSize);
  HyprlandAPI::addConfigValueV2(PHANDLE, globalState->config.barTextFont);

  return {"Hyprwin95", "Adds Windows 95 Borders to Hyprland", "Matercan", "0.0.1"};
}

APICALL EXPORT void PLUGIN_EXIT() {
  return;
}
