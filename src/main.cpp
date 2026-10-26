#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/desktop/rule/windowRule/WindowRule.hpp>
#include <hyprland/src/desktop/rule/windowRule/WindowRuleEffectContainer.hpp>
#include <hyprland/src/desktop/state/WindowState.hpp>
#include <hyprland/src/desktop/view/window/Window.hpp>
#include <hyprland/src/desktop/view/window/WindowPresentation.hpp>
#include <hyprland/src/config/lua/bindings/LuaBindingsInternal.hpp>
#include <hyprland/src/config/lua/types/LuaConfigColor.hpp>

#include <hyprutils/memory/UniquePtr.hpp>
#include <hyprutils/memory/SharedPtr.hpp>
#include <hyprutils/utils/ScopeGuard.hpp>

#include <stdexcept>
#include <algorithm>

#include "config/lua/types/LuaConfigValue.hpp"
#include "desktop/DesktopTypes.hpp"
#include "event/EventBus.hpp"
#include "borderDeco.hpp"
#include "globals.hpp"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

APICALL EXPORT std::string PLUGIN_API_VERSION() {
  return HYPRLAND_API_VERSION;
}

static void onNewWindow(PHLWINDOW window) {
  if (true) {
    if (std::ranges::any_of(window->presentation().decorations(),
                            [](const auto& d) { return d->getDisplayName() == "Hyprwin95"; }))
      return;

    auto border = Hyprutils::Memory::makeShared<Hyprwin95Border>(window);
    globalState->borders.emplace_back(border);
    border->pSelf = border;
    HyprlandAPI::addWindowDecoration(PHANDLE, window, std::move(border));
  }
}

static void onUpdateRules(PHLWINDOW window) {
  for (auto& b : globalState->borders) {
    if (!b) continue;
    b->updateRules();
  }
}

static void onPreConfigReload() {
  globalState->buttons.clear();
}

static void onConfigReloaded() {
  for (auto& b : globalState->borders) {
    if (!b) continue;
    b->onConfigReloaded();
  }
}

int newLuaButton(lua_State* L) {
  if (!lua_istable(L, 1))
    return Config::Lua::Bindings::Internal::configError(
      L, "add_button: expected a table { bg_color, fg_color, size, icon, action }");

  Hyprwin95Button button;

  {
    Hyprutils::Utils::CScopeGuard x([L] { lua_pop(L, 1); });

    lua_getfield(L, 1, "bg_color");

    Config::Lua::CLuaConfigColor parser(0);
    auto                         err = parser.parse(L);

    if (err.errorCode != Config::Lua::PARSE_ERROR_OK)
      return Config::Lua::Bindings::Internal::configError(L,
                                                          "add_button: failed to parse bg_color");

    button.bgCol = parser.parsed();
  }

  {
    Hyprutils::Utils::CScopeGuard x([L] { lua_pop(L, 1); });

    lua_getfield(L, 1, "fg_color");

    Config::Lua::CLuaConfigColor parser(0);
    auto                         err = parser.parse(L);

    if (err.errorCode != Config::Lua::PARSE_ERROR_OK)
      return Config::Lua::Bindings::Internal::configError(L,
                                                          "add_button: failed to parse fg_color");

    button.fgCol = parser.parsed();
  }

  {
    Hyprutils::Utils::CScopeGuard x([L] { lua_pop(L, 1); });

    lua_getfield(L, 1, "size");

    if (!lua_isnumber(L, -1))
      return Config::Lua::Bindings::Internal::configError(L, "add_button: size must be an integer");

    button.size = lua_tointeger(L, -1);
  }

  {
    Hyprutils::Utils::CScopeGuard x([L] { lua_pop(L, 1); });

    lua_getfield(L, 1, "icon");

    if (!lua_isstring(L, -1))
      return Config::Lua::Bindings::Internal::configError(L, "add_button icon must be a string");

    button.icon = lua_tostring(L, -1);
  }

  {
    Hyprutils::Utils::CScopeGuard x([L] { lua_pop(L, 1); });

    lua_getfield(L, 1, "action");

    if (!lua_isstring(L, -1))
      return Config::Lua::Bindings::Internal::configError(L, "add_buton action must be a string");

    button.cmd = lua_tostring(L, -1);
  }

  globalState->buttons.push_back(std::move(button));

  // globalState->bar.buttonsDirty = true;

  return 0;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
  PHANDLE = handle;

  const std::string COMPOSITOR_HASH = __hyprland_api_get_hash();
  const std::string CLIENT_HASH     = __hyprland_api_get_client_hash();

  fprintf(stderr, "[hyprwin95] compositor=%s client=%s\n", COMPOSITOR_HASH.c_str(),
          CLIENT_HASH.c_str());

  if (COMPOSITOR_HASH != CLIENT_HASH) {
    HyprlandAPI::addNotification(PHANDLE, "[Hyprwin95] Mismatched headers! Can't proceed.",
                                 CHyprColor(1.0, 0.2, 0.2, 1.0), 5000);
    throw std::runtime_error("[Hyprwin95] Version mismatch");
  }

  globalState = makeUnique<GlobalState>();
  globalState->borderColorRuleIdx =
    Desktop::Rule::windowEffects()->registerEffect("hyprwin95:border_color");
  globalState->barColorRuleIdx =
    Desktop::Rule::windowEffects()->registerEffect("hyprwin95:bar_color");
  globalState->titleColorRuleidx =
    Desktop::Rule::windowEffects()->registerEffect("hyprwin95:title_color");

  globalState->config.barColor = makeShared<Config::Values::CColorValue>(
    "plugin:hyprwin95:bar_color", "Change the bar color", 0xf1cff2ff);
  globalState->config.borderColor = makeShared<Config::Values::CColorValue>(
    "plugin:hyprwin95:border_color", "Change the border color", 0x524baaff);
  globalState->config.activeButtonColor = makeShared<Config::Values::CColorValue>(
    "plugin:hyprwin95:active_button_color",
    "Change the color of the button when the mouse if over it", 0xab334cff);
  globalState->config.textColor = makeShared<Config::Values::CColorValue>(
    "plugin:hyprwin95:text_color", "Change the color of the text", 0x5e3b9bff);
  globalState->config.barHeight =
    makeShared<Config::Values::CIntValue>("plugin::hyprwin95::bar_height", "Height of the bar", 24);
  globalState->config.barTextSize =
    makeShared<Config::Values::CIntValue>("plugin:hyprwin95:bar_text_size", "Size of bar text", 8);
  globalState->config.barPadding = makeShared<Config::Values::CIntValue>(
    "plugin:hyprwin95:bar_padding", "Difference between bar and buttons", 2);
  globalState->config.buttonPadding = makeShared<Config::Values::CIntValue>(
    "plugin:hyprwin95:button_padding", "Difference between button border and texture", 0);
  globalState->config.borderSize =
    makeShared<Config::Values::CIntValue>("plugin:hyprwin95:border_size", "Size of the border", 2);
  globalState->config.barTextFont = makeShared<Config::Values::CStringValue>(
    "plugin:hyprwin95:bar_text_font", "Text font of the bar", "WIN95FA");

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

  HyprlandAPI::addLuaFunction(PHANDLE, "hyprwin95", "add_button", ::newLuaButton);

  static auto P = Event::bus()->m_events.window.open.listen([&](PHLWINDOW w) { onNewWindow(w); });
  static auto P3 =
    Event::bus()->m_events.window.updateRules.listen([&](PHLWINDOW w) { onUpdateRules(w); });
  static auto P4 = Event::bus()->m_events.config.preReload.listen([&] { onPreConfigReload(); });
  static auto P5 = Event::bus()->m_events.config.reloaded.listen([&] { onConfigReloaded(); });

  for (auto& w : Desktop::windowState()->windows()) {
    if (w->isHidden() || !validMapped(w)) continue;
    onNewWindow(w);
  }

  HyprlandAPI::reloadConfig();

  return {"Hyprwin95", "Adds Windows 95 Borders to Hyprland", "Matercan", "0.0.1"};
}

APICALL EXPORT void PLUGIN_EXIT() {
  return;
}
