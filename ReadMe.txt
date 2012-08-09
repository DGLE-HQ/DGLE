<p align="left"><img src="https://github.com/megaDRONe86/DGLE2/blob/master/misc/dgle2_logo.png?raw=true"></p>

## Introduction

[DGLE2](http://dgle2.dronprogs.org) is a powerful cross platform engine for 2D/3D games and real-time visualizations. Young, strong and crazy!

The goal of the project is to provide developers with flexible & extandable cross platform easy-to-learn professional technology, capable of building any 2D/3D
games, real-time visualizations, scientific applications etc. It should be easy to make great projects only by using editors and scripts or go deeper and use
your programming skills of your favorite language to create really exciting projects. Users can also add new formats and functionality by creating new plugins
for engine and share them with others over the web or just get plugins already made by others. When project is complete it can be easily build for every popular
platform.

See [gallery](http://dgle2.dronprogs.org/dgle2_gallery) for screenshots of projects based on this engine.

Visit [official website](http://dgle2.dronprogs.org) for more details on this project and also plugins, articles, tutorials, samples and forum.

## Features

* Full cross platform.
* Plugin based architecture with micro-core principles.
* Flexible and highly extensible via plugins.
* Multi render support: OpenGL (1.x-2.x and 4.x), OpenGL ES (1.1, 2.0 and 3.0), Direct3D (9.0c and 11.1).
* Have set of visual editors for rapid application development.
* Provides all necessary API for development high quality  2D and 3D real-time applications.
* Made by GameDev professionals for you with LoVe!

## Supported Platforms

* Windows (XP and later)
* Linux (almost any distributive)
* MacOS (10.6 and later)
* iOS (4.3 and later)
* Android (2.3 and later)

## Supported Programming Languages

* C++
* C#
* Delphi
* Free Pascal
* Lua

## Getting Started

Simple C++ *"HelloWorld"* application for Windows.
This code will setup engine, create window and render *"Hello, World!"* text on screen via default font.
You can copy-paste this code to your 'main.cpp' and you are done! Also don't forget to copy engine headers
(you can find them in *"include/cpp"* folder) and library 'DGLE2.dll' (you can find it in *"bin/win"* folder)
in suitable locations to be found by compiler and executable.

```C++
#include "DGLE2.h"

using namespace DGLE2;

ENG_DYNAMIC_FUNC

IEngineCore	*pEngineCore = NULL;
IBitmapFont	*pFont = NULL;
IRender2D	*pRender2D = NULL;

void CALLBACK Init(void *pParametr)
{
	IResourceManager *resMan;
	pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)resMan);

	IRender *render;
	pEngineCore->GetSubSystem(ESS_RENDER, (IEngineSubSystem *&)render);
	render->GetRender2D(pRender2D);
	
	resMan->GetDefaultResource(EOT_BITMAP_FONT, (IEngBaseObj *&)pFont);
}

void CALLBACK Render(void *pParametr, uint uiDeltaTime)
{
	pRender2D->Begin2D();
	pFont->Draw2D(0, 0, "Hello, World!");
	pRender2D->End2D();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if ( GetEngine("DGLE2.dll", pEngineCore) )
	{
		pEngineCore->InitializeEngine(NULL, "My First App");
		pEngineCore->AddProcedure(EPT_INIT, &Init);
		pEngineCore->AddProcedure(EPT_RENDER, &Render);
		pEngineCore->StartEngine();
		FreeEngine();
	}
	else
		MessageBoxA(NULL, "Couldn't load \"DGLE2.dll\"!", "My First App", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
	
	return 0;
}
```

You can find more examples in the *"src/examples"* folder.

You can find build instructions [here](the build/HowTo.txt).

## Documentation

You can find engine API documentation [here](docs/api/index.html).

## License

Licensed under the [LGPL license](http://en.wikipedia.org/wiki/GNU_Lesser_General_Public_License).

## Release History

### Current version

0.3.0 beta (18.08.2012)

### Discription

This version is like a new and fresh start for the project.

### Major changes
 
* Total engine reengineering, a lot of stuff was made from scratch.
* Multi render support and totally new rendering subsystem.
* Isolation of platform depended code.
* Some old stuff (like plugins for browsers) was dropped off.
* No editors and almost no 3D in this version also no third-party plugins. Functionallity will start to return in nearest next releases.

*And much more!*

See [ChangeLog](ChangeLog.txt) for full list of versions and changes.

## Credits

### Project Leader and Lead Programmer

Andrey Korotkov aka DRON also known as [megaDRONe86](https://github.com/megaDRONe86).

[Follow me on Twitter](http://twitter.com/megadrone86).

### Additional Programming

Alexey Shaydurov aka Ash (3D graphics programming)

Kuznecov Stanislav aka STAlkerS (plugins and tools development)

Firstin Anton aka First (Delphi/C# headers and minor additional programming)

Semen Kotlyarov aka Kimer (plugins and tools development)

Ilya Sitnikov aka Sin (C# GUI components and DCP packs)

Mikhail Shestakov aka Mike (png and jpg loaders)

Svyatoslav Lobov aka FascioRoma (GUI components development)

### Special Thanks

artyks aka Ksenya Artemova for logo design

Woolf for project support

MadRain for Pyro particles

John Moff, R.I.P. friend

All DeeProSoft employees for project support

## Contacts

Visit [http://dgle2.dronprogs.org](http://dgle2.dronprogs.org) for more details.

Feel free to mail us [dgle2@dronprogs.org](mailto:dgle2@dronprogs.org) with any questions.