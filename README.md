# DotA 2 RPC (dota2-rpc-client)
![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/andsfonseca/dota2-rpc-client)
![GitHub milestone](https://img.shields.io/github/milestones/progress/andsfonseca/dota2-rpc-client/1)
 [![Hits](https://hits.seeyoufarm.com/api/count/incr/badge.svg?url=https%3A%2F%2Fgithub.com%2Fandsfonseca%2Fdota2-rpc-client&count_bg=%2379C83D&title_bg=%23555555&icon=&icon_color=%23E7E7E7&title=hits&edge_flat=false)](#)
 [![Website](https://img.shields.io/website?label=Website&url=https%3A%2F%2Fandsfonseca.github.io%2Fdota2-rpc-client%2F)](https://andsfonseca.github.io/dota2-rpc-client/)
 ![GitHub](https://img.shields.io/github/license/andsfonseca/dota2-rpc-client)

Allows you to observe the status of your DotA 2 (Defense of the Ancients 2) match within the Discord, through the Rich Presence service.

You can access the project website by [clicking here](https://andsfonseca.github.io/dota2-rpc-client/).
## What is?

This is a service that allows you to show your Discord friends information about the matches you play and watch within DotA 2.

<p align="center">
  <img src="https://user-images.githubusercontent.com/7833466/165001452-6b3ed84f-6420-45a2-9436-32da80ea9072.gif"> 
</p>

Share real-time information about what happens in your game with everyone.

## Features

| Feature | Preview |
|---------|---------|
| Check what your friends are watching.       |   <p align="center"><img src="https://user-images.githubusercontent.com/7833466/165001481-8a37881e-802c-4e6a-b40c-e4100b76b82f.gif"></p>      |
| Show off your heroes with your favorite skins.      | <p align="center"><img src="https://user-images.githubusercontent.com/7833466/165001946-910af221-c945-4a8f-84e0-8b9205627437.gif"><img src="https://user-images.githubusercontent.com/7833466/165001947-b3ea6991-2025-4cb5-b098-88cd9b7d3826.gif"><img src="https://user-images.githubusercontent.com/7833466/165001948-2370210a-2621-4549-af53-3d86d09bb545.gif"></p>        |
| Show your skills of accumulating money in the game       | <p align="center"><img src="https://user-images.githubusercontent.com/7833466/164261275-7f6ee3cf-a2a6-4c20-8a5f-d1be3b84714a.gif"></p>        |
| Show your items and your buffs.       | <p align="center"><img src="https://user-images.githubusercontent.com/7833466/165002021-4833bc4d-4ccb-432a-84ec-13b0a36b2c53.gif"><img src="https://user-images.githubusercontent.com/7833466/165002015-58d28c48-6fc1-4e2a-8753-fd6aa47c7c6c.gif"><img src="https://user-images.githubusercontent.com/7833466/165002020-d5c22994-fbff-4ba8-bcfa-992f7f222bd7.gif"><img src="https://user-images.githubusercontent.com/7833466/165002017-b50ccfd8-0e31-40e5-80ee-f8e64e81e1b2.gif"></p>        |
| Modified map support.       | <p align="center"><img src="https://user-images.githubusercontent.com/7833466/165002085-553f032c-35a9-455e-85aa-859e91ac59db.gif">        |
 | See your instructors.  | <p align="center"><img src="https://user-images.githubusercontent.com/7833466/165002839-aa520877-e38a-40d5-984c-9bfc8b9dbd65.gif">        |
| Multiple language support.       | <p align="center"><img src="https://user-images.githubusercontent.com/7833466/165002139-5eb77c4c-e5a7-4bf8-9839-0b8b4cd794f7.gif"><img src="https://user-images.githubusercontent.com/7833466/165002140-78e3af75-ba13-487d-8895-fe60289dedb1.gif">        |

## Installation (Windows only)

> See instructions for Linux below

Follow the step-by-step below to install your version of DotA 2 RPC

 * Download the latest software version on the [Releases page](https://github.com/andsfonseca/dota2-rpc-client/releases/);
 * Unzip the file in a folder of your preference. (There are many files, then be careful!🙃);
 * Open the file `install.windows.bat` and everything will be ready!

## Uninstallation (Windows only)
 
 * Open the `uninstall.windows.bat` file to remove the installation of your computer;
 * If you want, you can delete all the files.

## Advanced Users (Windows Only)
 
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

## Running on Linux (Experimental)

Just as a warning it's just a experimental version. If you find any bugs in this version, please report them. The Discord API and other functions commonly used on Windows may not have the same effect on Linux.

Unfortunately, this service has not been adapted to work with `daemon`. So installing and running the application must be done manually from the terminal.

Use this command to install
```shell
./dota2-rpc-client -i
```

And run the application using

```shell
./dota2-rpc-client
```
## Settings

You can choose what can be displayed and other application settings.

Just modify the [config.json](src/templates/config/config.json) file.

```json
{
    "HOST": "127.0.0.1",
    "PORT" : 52424,
    "LOCALE" : "System",
    "DISPLAY_OPTIONS":
    {
        "AEGIS" : true,
        "AGHANIM" : true,
        "GOLD_AND_LAST_HIT": true,
        "KILL_DEATH_ASSIST" : true,
        "SMOKE": true
    },
    "STEAM_FOLDER": ""
}
```

Below is a brief description of each property.

|        Property                      |                                    Description                                    |
|:------------------------------------ |:----------------------------------------------------------------------------------|
| `HOST`                               | The url the application will use. (Default: `127.0.0.1`)                          |
| `PORT`                               | The port the application will listen on. (Default: `52424`)                       |
| `LOCALE`                             | Language used. (Default: `System`, Avaiable options : [LocaleString](lang/), `System` and `Discord` ) |
| `DISPLAY_OPTIONS`                    | Discord display options.                                                          |
| `DISPLAY_OPTIONS:AEGIS`              | Whether or not to display the Aegis effect (Default: `true`)                    |
| `DISPLAY_OPTIONS:AGHANIM`            | Whether or not to display the Aghanim effect (Default: `true`)                    |
| `DISPLAY_OPTIONS:GOLD_AND_LAST_HIT`  | Whether or not to display the Gold, last hits and denies (Default: `true`)        |
| `DISPLAY_OPTIONS:KILL_DEATH_ASSIST`  | Whether or not to display the K/D/A (Default: `true`)                             |
| `DISPLAY_OPTIONS:SMOKE`              | Whether or not to display the smoke effect (Default: `true`)                      |
| `STEAM_FOLDER`                       | Steam folder location (Optional, default: Default folder commonly used by steam)  |

> If you installed the cfg file manually, when changing the `host` and `port` you will need to change them also in the `gamestate_integration_rpc.cfg` file. See this [issue #1](/../../issues/1).

## Compilation Instructions

Want to improve the project or compile a version outside the official channel?

> See the [Contribution Section](#contribution) to read more instructions or clone this repository.

This project mainly uses 4 libraries to work. Are they:

 * [Discord SDK](https://discord.com/developers/docs/intro) - A Library for Integration with the Discord application;
 * [Drogon C++](https://github.com/drogonframework/drogon) - A web framework written with C++;
 * [SrvLib](https://github.com/Tomenz/SrvLib) - A library for creating services in Windows and Linux created by [@tomenz](https://github.com/tomenz).
 * [NFD](https://github.com/btzy/nativefiledialog-extended) - A small C library with that portably invokes native file open, folder select and file save dialogs.  Created by [@btzy](https://github.com/btzy) and [@mlabbe](https://github.com/mlabbe).

This is a project made with *CMake*, these libraries are downloaded during the first compilation of the software and placed in a folder called `third_party`.

With the repository downloaded, run the command:

```shell
 cmake --build {folder} --config Debug --target dota2-rpc-client -j {threads}
```
 
> If you choose not to use automatic compilation and want to download libraries on your own, you can use the `vcpkg` to install the *Discord SDK* and the *Drogon C++*.
> 
> An installation guide for the *Drogon C++* dependencies can be found on [this page](https://drogon.docsforge.com/master/installation/).

## Web Page Compilation Instructions

Would you like to change the project webpage? You can access the files from the `web` directory.

The following frameworks and templates were used:

 * [Angular 13](https://angular.io/) - A component-based framework for building scalable web applications;
 * [Paradigm Shift](https://html5up.net/paradigm-shift) - A open-source web template created by [@ajlkn](https://github.com/ajlkn)

Instead of using static pages, a framework was used to create the site. So to compile, follow the instructions [available here](https://angular.io/guide/setup-local).

With everything configured, use the command below to run:

```shell
 ng serve
```
 
## Language Contribution
 
Would you like to translate the app into your language? Create a translation file based on the [en-us.json](lang/en-us.json) file. Then create an _issue_ or a _pull request_ to submit your modification.
 
 
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
