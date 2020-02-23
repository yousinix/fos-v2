# FOSv2

FOS is [ASU][asu]'s version, _modified by Dr. Ahmed Salah_, of [MIT][mit]'s [JOS][jos] educational operating system.  
**FOSv2** is an improvement of the current version of FOS.

[asu]: http://cis.asu.edu.eg/
[mit]: http://www.mit.edu/
[jos]: https://pdos.csail.mit.edu/6.828/2018/overview.html

## What's Different?

1. **No Eclipse!** — Can be used with any text editor, _defaults to Visual Studio Code_.
1. **Uses QEMU instead of Bochs** — QEMU is much faster, and needs less configuration files, also has better options.
1. **Independent source code** — Separate the OS's code from its environment dependencies and build tools.
1. **No Leftovers** — Remove JOS's leftovers from source code.
1. **Smaller size** — Remove unused packages and files from the environment, 50% smaller than FOSv1.
1. **Open source** — Allow contributions, improvements and bugs fixes, to make FOS better.

## Set up Windows Environment

### Option 1: Cygwin

1. **Emulator:**

   - [Download][dl-qemu] and Install [**QEMU**][qemu].
   - While installing, in the "Choose components" page uncheck all system emulations **except `i386`**.

1. **Linux-like Environment:**

   - [Download][dl-cygwin-32] and Install [**Cygwin**][cygwin] **_32-bit_**.
   - Copy **`setup-x86.exe`** to **`C:\cygwin`**.
   - Open a terminal in **`C:\cygwin`** and type:  
     <br />
     ```cmd
     ./setup-x86.exe -q -P make, perl
     ```

1. **Toolchain:**

   - [Download][dl-toolchain] **Binutils** & **GCC** _(Windows x86_64 64-bit)_.
   - Extract the archives, then move the contents of the **`i386-elf-binutils\`** & **`i386-elf-gcc\`** to **`C:\cygwin\opt\cross\`** _(merge them together)_.

1. **Update `PATH`:**

   - Add the following to your `PATH` environment variable:

     ```path
     C:\Program Files\qemu
     C:\cygwin\bin
     C:\cygwin\opt\cross\bin
     ```

1. **Text Editor:** _`OPTIONAL`_

   - [Download][dl-vscode] and Install [**Visual Studio Code**][vscode].
   - [Download][dl-extension] **C/C++ extension** for intelliSense, debugging, and code browsing.
   - Open Visual Studio Code and run **build task** → <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>B</kbd>.

[qemu]: https://www.qemu.org/
[dl-qemu]: https://qemu.weilnetz.de/w64/2020/
[cygwin]: https://cygwin.com/
[dl-cygwin-32]: https://cygwin.com/install.html
[dl-toolchain]: https://github.com/nativeos/i386-elf-toolchain/releases
[vscode]: https://code.visualstudio.com/
[dl-vscode]: https://code.visualstudio.com/
[dl-extension]: https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools

> ### \*Option 2: WSL ![wip][wip-badge]
>
> WINDOWS SUBSYSTEM FOR LINUX

> ## \*Set up Linux Environment ![wip][wip-badge]
>
> INSTALL QEMU & TOOLCHAIN

> ## \*Debugging ![wip][wip-badge]
>
> USING GDB

## Contribute

- See the project's upcoming changes [here][project].
- Submit a [PR][compare] with your requested changes. Contributions are always welcomed.
- Submit an [Issue][new-issue] if there's something wrong or unclear.

[project]: https://github.com/YoussefRaafatNasry/fos-v2/projects/1/
[compare]: https://github.com/YoussefRaafatNasry/fos-v2/compare/
[new-issue]: https://github.com/YoussefRaafatNasry/fos-v2/issues/new/

> ## \*License ![wip][wip-badge]
>
> - JOS LICENSES
> - FOS LICENSES & CREDITS

[wip-badge]: https://img.shields.io/badge/WIP-red?style=flat-square
