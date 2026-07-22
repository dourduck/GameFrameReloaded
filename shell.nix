{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    gcc
    lua
    yazi
    clang
    xorg.libX11
    glfw
    libGL
    libGLU
    mesa
  ];

  shellHook = ''
    echo "Welcome to dev shell!"
    export LD_LIBRARY_PATH=${pkgs.lib.makeLibraryPath [
      pkgs.libGL
      pkgs.libGLU
      pkgs.mesa
      pkgs.xorg.libX11
      pkgs.xorg.libXrandr
      pkgs.xorg.libXi
      pkgs.xorg.libXcursor
      pkgs.xorg.libXinerama
    ]}:$LD_LIBRARY_PATH
  '';
}
