{
  mkShell,
  gcc,
  gnumake,
  pkg-config
}:

mkShell {
  name = "voxelmint-dev";
  strictDeps = true;

  nativeBuildInputs = [
    gnumake
    pkg-config
    gcc
  ];

  buildInputs = [];

  shellHook = /* bash */ ''
    zsh
  '';
}
