{
  inputs.nixpkgs.follows = "hyprland/nixpkgs";
  inputs.hyprland.url = "github:Hyprwm/Hyprland";

  outputs =
    {
      self,
      nixpkgs,
      hyprland,
    }:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
      ];
      forEachSystem = nixpkgs.lib.genAttrs systems;
      pkgsForEach = nixpkgs.legacyPackages;
    in
    {
      packages = forEachSystem (system: {
        default = pkgsForEach.${system}.callPackage ./nix/package.nix { };
      });

      devShells = forEachSystem (system:
        with pkgsForEach.${system}; {
          default = mkShell.override {stdenv = gcc14Stdenv;} {
            name = "hyprland-plugins";
            nativeBuildInputs = [
              bear
              clang-tools
            ];
            buildInputs = [hyprland.packages.${system}.hyprland];
            inputsFrom = [hyprland.packages.${system}.hyprland];
          };
        });
      hydraJobs = self.packages;
    };
}
