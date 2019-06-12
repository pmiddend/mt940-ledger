let pkgs = import <nixpkgs> {};
in pkgs.stdenv.mkDerivation rec {
  name = "mt940-ledger-${version}";
  version = "20160328";

  src = ./.;

  buildInputs = [ pkgs.readline ];

  installPhase = ''
    mkdir -p $out/bin
    install mt940-ledger $out/bin
  '';
}
