# ps4_remote_pkg_installer-OOSDK

This is a OOSDK port of flat's ps4_remote_pkg_installer, all credit goes to them and their amazing work!

## Building

A visual studio project has been included for building on Windows. On Linux, a makefile has been included.

To build this project, the developer will need clang, which is provided in the [toolchain](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain). The `OO_PS4_TOOLCHAIN` environment variable will also need to be set to the root directory of the SDK installation.

__Windows__
Open the Visual Studio project and build, or run the batch file from command prompt or powershell with the following command:

```bash
.\build.bat .\x64\Debug "RPI" "%OO_PS4_TOOLCHAIN%\\RPI"
```

__Linux__
Run the makefile.

```bash
make
```

__MacOS__
Run the makefile.

```bash
brew install llvm
sudo OO_PS4_TOOLCHAIN=/opt/OpenOrbis-PS4-Toolchain make
```

## NOTES

- The default port is 12801
- Some freeBSD net functions where giving issue with printing debug info out, so i swapped these to the sceNet* versions as they seemed to work.

## Thanks

- [OpenOrbis/OpenOrbis-PS4-Toolchain](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain)
- [flatz/ps4_remote_pkg_installer](https://github.com/flatz/ps4_remote_pkg_installer)