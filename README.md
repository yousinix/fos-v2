# FOSv2

[FOS][fos-v1] is an educational OS for Ain Shams University Operating Systems Course CSW355, forked and refactored from [MIT Operating Systems Lab 6.828][mit-6.828]. It was created by [Dr. Mahmoud Hossam][dr-m-h] and currently maintained by [Dr. Ahmed Salah][dr-a-s].  
**FOSv2** is the next version of FOS. It provides a much faster and better environment using other editing and virtualization tools.

[fos-v1]: https://github.com/mahossam/FOS-Ain-Shams-University-Educational-OS
[dr-m-h]: https://github.com/mahossam/
[dr-a-s]: mailto:ahmed_salah@cis.asu.edu.eg
[mit-6.828]: http://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-828-operating-system-engineering-fall-2012/

<!-- TOC depthFrom:2 -->

- [1. What's Different?](#1-whats-different)
- [2. Set up Environment](#2-set-up-environment)
    - [2.1. Windows - Cygwin](#21-windows---cygwin)
    - [2.2. Windows - WSL](#22-windows---wsl)
    - [2.3. Linux](#23-linux)
- [3. Setup Workspace](#3-setup-workspace)
- [4. Debugging](#4-debugging)
- [5. Solve FOSv1 Assignments on FOSv2](#5-solve-fosv1-assignments-on-fosv2)
- [6. Contribute](#6-contribute)

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

   - [Download][dl-toolchain] the `i386-elf-toolchain` for Windows.
   - Extract **`i386-elf-toolchain-windows.rar`** in **`C:\cygwin\opt\cross\`** _(create this folder if it doesn't exist)_.

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
[dl-toolchain]: https://github.com/YoussefRaafatNasry/fos-v2/releases/tag/toolchain
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
sudo apt-get install build-essential qemu-system-i386 gdb libfl-dev

# Create directory
sudo mkdir /opt/cross
cd /opt/cross

# Install Toolchain
sudo wget https://github.com/YoussefRaafatNasry/fos-v2/releases/download/toolchain/i386-elf-toolchain-linux.tar.bz2
sudo tar xjf i386-elf-toolchain-linux.tar.bz2
sudo rm i386-elf-toolchain-linux.tar.bz2

# Update your PATH in your ~/.bashrc file.
echo 'export PATH="$PATH:/opt/cross/bin"' >> ~/.bashrc
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

## 5. Solve FOSv1 Assignments on FOSv2

FOSv2 has changed [`kern/Makefrag`][kern-makefrag] in [`8e2612dc`][8e2612dc-diff]. This changes can cause errors after adding assignments' files _(which usually conatin a `kern/Makefrag` file made for FOSv1)_. To solve this issue:

1. Copy the assignments' files into your **FOSv2** project _(choose overwrite if prompted)_.
1. Open the new `kern/Makefrag` and replace [these lines][old-makefrag-lines] by [those ones][new-makefrag-lines].  
   _(or simply replace all occurences of `$(OBJDIR)/kern/bochs.img` by `$(IMAGE)`)_

[kern-makefrag]: https://github.com/YoussefRaafatNasry/fos-v2/blob/master/kern/Makefrag
[8e2612dc-diff]: https://github.com/YoussefRaafatNasry/fos-v2/commit/8e2612dc7704dd7716caf6cf9bb42ebd63dd8154#diff-92c5d30afff461ec71534ce1894d6fce755d04937f8b29837eb45ac01b6a7564
[old-makefrag-lines]: https://github.com/YoussefRaafatNasry/fos-v2/blob/8639fae15e1de34f8ce40ce98dfff434158c0a90/kern/Makefrag#L71-L79
[new-makefrag-lines]: https://github.com/YoussefRaafatNasry/fos-v2/blob/5389c176351af7a089853d4c84aa1ea537efe296/kern/Makefrag#L71-L79

## 6. Contribute

- See the project's upcoming changes [here][project].
- Submit a [PR][compare] with your requested changes. Contributions are always welcomed.
- Submit an [Issue][new-issue] if there's something wrong or unclear.

[project]: https://github.com/YoussefRaafatNasry/fos-v2/projects/1/
[compare]: https://github.com/YoussefRaafatNasry/fos-v2/compare/
[new-issue]: https://github.com/YoussefRaafatNasry/fos-v2/issues/new/
