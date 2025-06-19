# epic-lfhcal-tbana software repo

This software repo is meant to serve as basis for the test beam root based analysis of the LFHCal test beams and test beam setups.
The repo contains the initial code used in 2023 under OldStructure & the newer version of the code under NewStructure <br>

A detailed introduction to this package in particular the NewStructure can be found [here](https://friederikebock.gitbook.io/epiclfhcaltb-ana)
It is highly recommended to follow that guide.

Credit for the software development goes primarily to Vincent Andrieux (@vandrie), Tristan Protzman (@tlprotzman) and Friederike Bock (@FriederikeBock).

## Compilation
To build the software, we recommend using cMake.
1) Make sure you have the latest version of the decoder `git pull --recurse-submodules`.  If it is your first time using the submodule you should run `git submodule update --init --recursive`

Option 1:
2) Set up the compilation in the same directory.  From the `NewStructure` folder, run `cmake .`
3) Compile the code with `make`

Option 2:
2) Set up a dedicated build directory.  From the `NewStructure` folder, run `cmake -B build .`
3) Compile the code.  Switch to the build directory with `cd build` and compile with `make`


For convenience a linker script is provided, which can be used to recreate the software structure with soft links. Execute it in the directory where you would like to work after having added your user name and path to the software repo

```console
bash $PATHTOSCRIPT/prepareAnalysisDirectory.sh $USERNAME [new/old]
```
