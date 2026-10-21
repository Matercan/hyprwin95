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

      devShells = forEachSystem (
        system:
        let
          pkgs = pkgsForEach.${system};
          hl = hyprland.packages.${system}.hyprland;
        in
        {
          default = pkgs.mkShell.override { stdenv = pkgs.gcc16Stdenv; } {
            name = "hyprland-plugins";
            nativeBuildInputs = [
              pkgs.bear
              pkgs.clang-tools
            ];
            inputsFrom = [ hl ];
            buildInputs = [ hl.dev ];
          };
        }
      );
    };
}
