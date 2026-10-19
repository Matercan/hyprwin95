{
  mkShell,
  hyprcursor,
  gcc,
  gnumake,
  pkg-config,
  hyprgraphics,
  hyprlang,
  hyprutils,
  aquamarine,
  pixman,
  libdrm,
  hyprland,
  libinput,
  eudev,
  wayland,
  xcbutil,
  libxkbcommon,
  libglvnd,
  cairo,
  mesa
}:

mkShell {
  name = "hyprwin95-dev";
  strictDeps = true;

  nativeBuildInputs = [
    gnumake
    pkg-config
    gcc
  ];

  buildInputs = [
    xcbutil
    cairo
    hyprlang
    hyprutils
    hyprgraphics
    hyprcursor
    aquamarine
    libglvnd
    mesa
    pixman
    libdrm
    hyprland
    libinput
    eudev
    wayland
    libxkbcommon
  ];
}
