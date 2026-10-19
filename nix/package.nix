{
  lib,
  stdenv,
  pkg-config,
  gnumake,
  hyprutils,
  hyprlang,
  hyprgraphics,
  aquamarine,
  hyprcursor,
  pixman,
  libdrm,
  hyprland,
  libinput,
  eudev,      
  wayland,    
  libxkbcommon,
  libglvnd,
  cairo,
  xcbutil,
  mesa
}:
stdenv.mkDerivation {
  pname = "hyprwin95";
  version = "0.0.1";
  
  src = let
    fs = lib.fileset;
    s = ../.;
  in 
  fs.toSource {
    root = s;
    fileset = fs.unions [
      (s + /src)
      (s + /Makefile)
    ];
  };

  nativeBuildInputs = [
    gnumake
    pkg-config
  ];

  buildInputs = [
    xcbutil
    cairo
    hyprutils
    hyprlang
    hyprgraphics
    hyprcursor
    aquamarine
    pixman
    libdrm
    hyprland
    libinput
    eudev
    wayland
    libxkbcommon
    libglvnd
    mesa
  ];

  buildPhase = ''
    runHook preBuild
    make
    runHook postBuild
  '';

  installPhase = ''
    runHook preInstall
    
    mkdir -p $out/lib
    cp lib/hyprwin95.so $out/lib/
    
    runHook postInstall
  '';

  meta = {
    description = "A Hyprland plugin to add windows 95 borders";
    license = lib.licenses.gpl3;
    maintainers = with lib.maintainers; [ Matercan ];
    platforms = lib.platforms.linux;
  };
}
