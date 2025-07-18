{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
   	 	# nativeBuildInputs is usually what you want -- tools you need to run
   	 	nativeBuildInputs = with pkgs.buildPackages; [ 
		gcc
		libGL
		xorg.libX11
		xorg.libX11.dev
  		raylib
		box2d
		gdb
    ];
    LD_LIBRARY_PATH="/home/sergey/projects/c/raylib/lib/:$LD_LIBRARY_PATH";

}
