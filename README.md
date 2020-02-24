# FOSv2

FOS is [ASU][asu]'s version, _modified by Dr. Ahmed Salah_, of [MIT][mit]'s [JOS][jos] educational operating system.  
**FOSv2** is an improvement of the current version of FOS.

[asu]: http://cis.asu.edu.eg/
[mit]: http://www.mit.edu/
[jos]: https://pdos.csail.mit.edu/6.828/2018/overview.html

<!-- TOC depthFrom:2 -->

- [1. What's Different?](#1-whats-different)
- [2. Set up Environment](#2-set-up-environment)
    - [2.1. Windows - Cygwin](#21-windows---cygwin)
    - [2.2. Windows - WSL](#22-windows---wsl)
    - [2.3. Linux](#23-linux)
- [3. Setup Workspace](#3-setup-workspace)
- [4. Debugging](#4-debugging)
- [5. Contribute](#5-contribute)

<!-- /TOC -->

## 1. What's Different?

1. **No Eclipse!** — Can be used with any text editor, _defaults to Visual Studio Code_.
1. **Uses QEMU instead of Bochs** — QEMU is much faster, and needs less configuration files, also has better options.
1. **Independent source code** — Separate the OS's code from its environment dependencies and build tools.
1. **No Leftovers** — Remove JOS's leftovers from source code.
1. **Smaller size** — Remove unused packages and files from the environment, 50% smaller than FOSv1.
1. **Open source** — Allow contributions, improvements and bugs fixes, to make FOS better.

![screenshot](https://user-images.githubusercontent.com/41103290/75132023-0e3f9d80-56de-11ea-9daf-e578bdcdd750.png)

## 2. Set up Environment

FOS needs an Linux environment to run as expected. For Windows users, the methods below try to mimic a Linux environment on Windows, choose any of them. For Linux users, just setup your environment as described [below](#23-linux).

### 2.1. Windows - Cygwin

1. **Linux-like Environment:**

   - [Download][dl-cygwin-32] and Install [**Cygwin**][cygwin] **_32-bit_**.
   - Copy **`setup-x86.exe`** to **`C:\cygwin`**.
   - Open a terminal in **`C:\cygwin`** and type:  
     <br />
     ```cmd
     ./setup-x86.exe -q -P gdb,make,perl
     ```

1. **Toolchain:**

   - [Download][dl-toolchain] **Binutils** & **GCC** _(Windows x86_64 64-bit)_.
   - Extract the archives, then move the contents of the **`i386-elf-binutils\`** & **`i386-elf-gcc\`** to **`C:\cygwin\opt\cross\`** _(merge them together)_.

1. **Emulator:**

   - [Download][dl-qemu] and Install [**QEMU**][qemu].
   - While installing, in the "Choose components" page uncheck all system emulations **except `i386`**.

1. **Update `PATH`:**

   - Add the following to your `PATH` environment variable:

     ```path
     C:\Program Files\qemu
     C:\cygwin\bin
     C:\cygwin\opt\cross\bin
     ```

[cygwin]: https://cygwin.com/
[dl-cygwin-32]: https://cygwin.com/install.html
[dl-toolchain]: https://github.com/nativeos/i386-elf-toolchain/releases
[qemu]: https://www.qemu.org/
[dl-qemu]: https://qemu.weilnetz.de/w64/2020/

### 2.2. Windows - WSL

> Requires Windows 10 build 16215+

The [Windows Subsystem for Linux][wsl] lets developers run a GNU/Linux environment _including most command-line tools, utilities, and applications_ directly on Windows, unmodified, without the overhead of a virtual machine.

1. Open Control Panel > Programs > Turn Windows Features on or off > Check "Windows Subsystem for Linux".
1. From **Microsoft Store**, download **Ubuntu**.
1. Launch **Ubuntu** from your start menu.
1. Set up your environment as described below in the **Linux** [section](#23-linux). _(Install missing tools if any command wasn't found.)_

[wsl]: https://docs.microsoft.com/en-us/windows/wsl/about

### 2.3. Linux

```bash
# Required Packages
sudo apt-get update
sudo apt-get install build-essential qemu-system-i386 gdb

# Create directory
sudo mkdir /opt/cross
cd /opt/cross

# Install Binutils
sudo wget https://github.com/nativeos/i386-elf-toolchain/releases/download/preview/i386-elf-binutils-linux-x86_64.tar.bz2
sudo tar xjf i386-elf-binutils-linux-x86_64.tar.bz2
sudo rm i386-elf-binutils-linux-x86_64.tar.bz2

# Install GCC
sudo wget https://github.com/nativeos/i386-elf-toolchain/releases/download/preview/i386-elf-gcc-linux-x86_64.tar.bz2
sudo tar xjf i386-elf-gcc-linux-x86_64.tar.bz2
sudo rm i386-elf-gcc-linux-x86_64.tar.bz2

# Merge tools
sudo rsync -a i386-elf-gcc/ i386-elf-binutils/ .
sudo rm -r i386-elf-gcc/ i386-elf-binutils/

# Update your PATH by updating your ~/.bashrc file.
export PATH="$PATH:/opt/cross/bin"
```

## 3. Setup Workspace

1. [Download][dl-vscode] and Install [**Visual Studio Code**][vscode].
1. [Download][dl-repo] or Clone\* this repo and Open it in VS Code  
   <br/>
   ```bash
   git clone https://github.com/YoussefRaafatNasry/fos-v2.git
   cd fos-v2/
   code .
   ```
1. **Install all** recommended extensions _from the bottom right corner_.
1. Run the **build task** → <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>B</kbd>.

[vscode]: https://code.visualstudio.com/
[dl-vscode]: https://code.visualstudio.com/
[dl-repo]: https://github.com/YoussefRaafatNasry/fos-v2/archive/master.zip

> \*Cloning is recommended to get the latest changes using `git pull`

## 4. Debugging

1. Add breakpoints to your code.
1. Start Debugging → <kbd>F5</kbd>.
1. Fix your bugs!

## 5. Contribute

- See the project's upcoming changes [here][project].
- Submit a [PR][compare] with your requested changes. Contributions are always welcomed.
- Submit an [Issue][new-issue] if there's something wrong or unclear.

[project]: https://github.com/YoussefRaafatNasry/fos-v2/projects/1/
[compare]: https://github.com/YoussefRaafatNasry/fos-v2/compare/
[new-issue]: https://github.com/YoussefRaafatNasry/fos-v2/issues/new/
