# DotA 2 RPC (dota2-rpc-client)

 [![Hits](https://hits.seeyoufarm.com/api/count/incr/badge.svg?url=https%3A%2F%2Fgithub.com%2Fandsfonseca%2Fdota2-rpc-client&count_bg=%2379C83D&title_bg=%23555555&icon=&icon_color=%23E7E7E7&title=hits&edge_flat=false)](#)

Allows you to observe the status of your DotA 2 (Defense of the Ancients 2) match within the Discord, through the Rich Presence service.

## What is?

This is a service that allows you to show your Discord friends information about the matches you play and watch within DotA 2.

<p align="center">
  <img src="https://user-images.githubusercontent.com/7833466/164252216-de275d6d-dcc3-42db-9a80-de4840a438cb.gif"> 
</p>

Share real-time information about what happens in your game with everyone.

## Features

| Feature | Preview |
|---------|---------|
| Check what your friends are watching.       |   <p align="center"><img src="https://user-images.githubusercontent.com/7833466/164255951-10e4536e-4050-4a42-b2d8-68397d8fa33e.gif"></p>      |
| Show off your heroes with your favorite skins.      | <p align="center"><img src="https://user-images.githubusercontent.com/7833466/164260982-7597cde7-02b3-4c3d-8836-e56796caa12a.gif"></p>        |
| Show your skills of accumulating money in the game       | <p align="center"><img src="https://user-images.githubusercontent.com/7833466/164261275-7f6ee3cf-a2a6-4c20-8a5f-d1be3b84714a.gif"></p>        |
| Show your items and your buffs.       | <p align="center"><img src="https://user-images.githubusercontent.com/7833466/164261286-c5f9df38-c542-40ec-afc8-7b8eefeaccaf.gif"><img src="https://user-images.githubusercontent.com/7833466/164261297-1e0628a8-6cbf-435a-883c-c8e113984c3a.gif"><img src="https://user-images.githubusercontent.com/7833466/164261312-f2a747cc-42b8-47a8-83bd-86ad5dd6d337.gif"><img src="https://user-images.githubusercontent.com/7833466/164261328-833cd472-7524-4327-8cb1-61db62c45d35.gif"></p>        |
| \[Alpha\] Modified map support.       | <p align="center"><img src="https://user-images.githubusercontent.com/7833466/164261343-60a31c39-4737-4ee0-a157-199e97fbb838.gif">        |


## Installation

Follow the step-by-step below to install your version of DotA 2 RPC

 * Download the latest software version on the [Releases page](https://github.com/andsfonseca/dota2-rpc-client/releases/);
 * Unzip the file in a folder of your preference. (There are many files, then be careful!🙃);
 * Open the file `install.windows.bat` and everything will be ready!

## Uninstallation
 
 * Open the `uninstall.windows.bat` file to remove the installation of your computer;
 * If you want, you can delete all the files.

## Advanced Users
 
This software is registered as Windows service. If you are looking for other options, here are other ways to use the application.
 
 ```shell
dota2-rpc-client.exe <flag>
```

|        Flag               |                                    Description                                    |
|:-------------------------:|:---------------------------------------------------------------------------------:|
| `-i`                      | Install the DotA 2 RPC as a service.                                              |
| `-r`                      | Uninstall the service, if installed.                                              |
| `-s`                      | If installed, start the service.                                                  |
| `-e`                      | If installed, stop the service.                                                   |
| `-f`                      | Execute the DotA 2 RPC like a Console Application.                                |

## Compilations Instructions

Want to improve the project or compile a version outside the official channel?

> See the [Contribution Section](#contribution) to read more instructions or clone this repository.

This project mainly uses 3 libraries to work. Are they:

 * [Discord SDK](https://discord.com/developers/docs/intro) - A Library for Integration with the Discord application;
 * [Drogon C++](https://github.com/drogonframework/drogon) - A web framework written with C++;
 * [SrvLib](https://github.com/Tomenz/SrvLib) - A library for creating services in Windows and Linux.

This is a project made with *CMake*, these libraries are downloaded during the first compilation of the software and placed in a folder called `third_party`.

With the repository downloaded, run the command:

```shell
 cmake --build {folder} --config Debug --target dota2-rpc-client -j {threads}
```
 
> If you choose not to use automatic compilation and want to download libraries on your own, you can use the `vcpkg` to install the *Discord SDK* and the *Drogon C++*.
> 
> An installation guide for the *Drogon C++* dependencies can be found on [this page](https://drogon.docsforge.com/master/installation/).

## Issues

Feel free to submit issues and enhancement requests.

## Contribution

1. Fork the project
2. Create a _branch_ for your modification (`git checkout -b my-new-resource`)
3. Do the _commit_ (`git commit -am 'Adding a new resource...'`)
4. _Push_ (`git push origin my-new-resource`)
5. Create a new _Pull Request_ 

## Notes

Images, names, and any entities related to DotA 2 are copyrighted by Valve Corporation. This software only uses the data shared by the game, but it is not related to the original game.
