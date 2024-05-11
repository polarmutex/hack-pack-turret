{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    arduino-nix.url = "github:bouk/arduino-nix";
    arduino-library-index = {
      url = "github:bouk/arduino-indexes/library_index";
      flake = false;
    };
    arduino-package-index = {
      url = "github:bouk/arduino-indexes/package_index";
      flake = false;
    };
    arduino-package-rp2040-index = {
      url = "github:bouk/arduino-indexes/package_rp2040_index";
      flake = false;
    };
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
    arduino-nix,
    arduino-package-index,
    arduino-package-rp2040-index,
    arduino-library-index,
    ...
  } @ attrs: let
    overlays = [
      (arduino-nix.overlay)
      (arduino-nix.mkArduinoPackageOverlay (arduino-package-index + "/package_index.json"))
      (arduino-nix.mkArduinoPackageOverlay (arduino-package-rp2040-index + "/package_rp2040_index.json"))
      (arduino-nix.mkArduinoLibraryOverlay (arduino-library-index + "/library_index.json"))
    ];
  in (flake-utils.lib.eachDefaultSystem (
    system: let
      pkgs = (import nixpkgs) {
        inherit system overlays;
      };
    in rec {
      packages.arduino-cli = pkgs.wrapArduinoCLI {
        libraries = with pkgs.arduinoLibraries; [
          (arduino-nix.latestVersion Servo)
          (arduino-nix.latestVersion IRremote)
          # (arduino-nix.latestVersion ADS1X15)
          # (arduino-nix.latestVersion Ethernet_Generic)
          # (arduino-nix.latestVersion SCL3300)
          # (arduino-nix.latestVersion TMCStepper)
          # (arduino-nix.latestVersion pkgs.arduinoLibraries."Adafruit PWM Servo Driver Library")
        ];

        packages = with pkgs.arduinoPackages; [
          platforms.arduino.avr."1.6.23"
          # platforms.rp2040.rp2040."2.3.3"
        ];
      };
      devShells.default = pkgs.mkShell {
        buildInputs = [
          self.packages."${system}".arduino-cli
        ];
      };
    }
  ));
}
