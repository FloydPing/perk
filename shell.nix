with import <nixpkgs> {};
{}:
gccStdenv.mkDerivation {
  name = "perk";
  src = ./.;
  buildInputs = with pkgs; [ 
    gnumake 
    cmake 
    clang
    clang-tools
    gcc
    pkg-config
    openssl
  ] ++ (lib.optionals pkgs.stdenv.isLinux ([
    gdb
    #valgrind
    #linuxKernel.packages.linux_6_6.perf
  ]));
}
