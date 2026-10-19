#include <stdexcept>
#include <hyprland/src/plugins/PluginAPI.hpp>

#include "globals.hpp"

APICALL EXPORT std::string PLUGIN_API_VERSION() {
  return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
  PHANDLE = handle;

  const std::string COMPOSITOR_HASH = __hyprland_api_get_hash();
  const std::string CLIENT_HASH = __hyprland_api_get_client_hash();

  if (COMPOSITOR_HASH != CLIENT_HASH) {
    HyprlandAPI::addNotification(PHANDLE, "[Hyprwin95] Mismatched headers! Can't proceed.", CHyprColor(1.0, 0.2, 0.2, 1.0), 5000);
    throw std::runtime_error("[Hyprwin95] Version mismatch");
  }

  // Bulk of the stuff

  return { "Hyprwin95", "Adds Windows 95 Borders to Hyprland" , "Matercan", "0.0.1" };
}

APICALL EXPORT void PLUGIN_EXIT() {
  return;
}
