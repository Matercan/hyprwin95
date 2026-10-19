{
  lib,
  stdenv,
  pkg-config,
  gnumake
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

  buildPhase = ''
    runHook preBuild

    make

    runHook postBuild
  '';

  buildInputs = [
    gnumake
  ];

  nativeBuildInputs = [
    gnumake
    pkg-config
  ];

  meta = {
    description = "A Hyprland plugin to add windows 95 borders";
    license = lib.licenses.gpl3;
    maintainers = with lib.maintainers; [ Matercan ];
    platforms = lib.platforms.linux;
  };
}
