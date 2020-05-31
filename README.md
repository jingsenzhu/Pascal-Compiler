# Pascal-Compiler
A simple pascal compiler using flex and bison as frontend and LLVM as backend. Project for Compiler Principle course in ZJU.



## Build and use

1. Install flex and bison

   ```shell
   sudo apt install -y flex bison
   ```

2. Download LLVM 9.0.0

   - **Note: DO NOT use the LLVM installed from apt!**

   - Download from LLVM official website:

     - Website address: https://releases.llvm.org/download.html#9.0.0

     - If you're Ubuntu 14/16/18/19.04, you can download the pre-built binaries:

       - Take Ubuntu 18.04 as example:

         ```sh
         wget https://releases.llvm.org/9.0.0/clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz
         ```

       - If the Internet speed in the virtual machine is too slow, you can download it in Windows first and drag it into virtual machine

     - If your system doesn't support pre-built binaries, you should download the source code yourself and compile it

   - Extract the tar.xz:

     ```sh
     tar xvf clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz
     ```

   - Install prerequisites

     ```sh
     sudo apt install -y cmake zlib1g-dev libedit-dev libxml2-dev
     ```

3. Suppose your LLVM 9.0.0 binary is at `path/to/clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04`, the directory for this project is at `~/Pascal-Compiler`
   With all the prerequisites installed, you can build this project

   ```sh
   cd ~/Pascal-Compiler
   mkdir build
   export LLVM_DIR=path/to/clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04/lib/cmake/llvm
   cmake ..
   make
   ```

4. Enjoy! (-o is optional)

   ```
   ./spc [-O] [-ir/-S/-c] <source pascal file> [-o <output file>]
   ```

   Args decription:

   - -ir: produce LLVM IR code
   - -S: produce assembler code
   - -c: produce obj file
   - -o \<output file\>: Optional, specify the output file. If not specified, the compiler will generate a file with the same name as the pascal source file
   - -O: Optional, enable LLVM optimizations

