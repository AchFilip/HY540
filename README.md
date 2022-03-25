# HY540
Repository for the class "Advanced Topics of Languages Development" of the Computer Science in University of Crete. 

Students involved in the project:
- Achilleas Filippidis [![LinkedIn](images/linkedin-icon.png)](https://www.linkedin.com/in/achilleas-filippidis-7127131b3/)
- Antonis Agapakis
- Antonis Prinianakis [![LinkedIn](images/linkedin-icon.png)](https://www.linkedin.com/in/antonis-prinianakis-b999a821a/)
- Zack Pervolarakis [![LinkedIn](images/linkedin-icon.png)](https://www.linkedin.com/in/zack-pervolarakis-2b05061a5/)

# Install and Run
Everything was compiled in Windows 10/11 OS using **MSYS2** , **Flex** and **YACC/BISON** <br>
Follow the steps below for the installation process:
## Installing MSYS2
1. Download the installer from [here](https://www.msys2.org/)
2. Run the installer
3. After installing it, open the **MSYS2** app. You will need to run a couple of commands.
4. First run ```pacman -Syu```
5. **Reopen** the app and run ```pacman -Su```
6. After updating the packages, you will need to install makefile and mingw-w64 GCC. <br>
Run ```pacman -S --needed base-devel mingw-w64-x86_64-toolchain```
7. Add both mingw64/bin and usr/bin paths in the Environment Variables.

## Installing Flex and YACC/BISON
1. Download the **Setup** from [here](http://gnuwin32.sourceforge.net/packages/flex.htm) and run it.
2. Download the **Setup** from [here](http://gnuwin32.sourceforge.net/packages/bison.htm) and run it.
3. After installing both of them , add the ../bin path to the Environment Variables

After doing both of the above steps, you are ready to compile and test our alpha compiler :)