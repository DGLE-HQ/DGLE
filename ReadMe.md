<p align="left"><img src="https://raw.github.com/DGLE-HQ/DGLE/master/misc/dgle2_logo.png"></p>

## Introduction

[DGLE](http://dglengine.org) is a powerful independent cross-platform engine for 2D/3D games and real-time visualizations. Young, strong and crazy!

### Project mission
The goal of the project is to provide developers with flexible and extendable cross-platform easy-to-learn professional technology, capable of building any 2D/3D
games, real-time visualizations, scientific applications etc. It should be easy to make great projects only by using editors and scripts or go deeper and use
your programming skills of your favorite language to create really exciting projects. Users can also add new formats and functionality by creating new plugins
for engine and share them with others over the web or just get plugins already made by others. When project is complete it can be easily build for every popular
platform.

### What does DGLE abbreviation means?
This abbreviation is some kind of legacy and it is a long story...
Now officially this means: *Dynamite Groovy Lightweight Engine* that is more correct, concerning to the spirit of the project.
Sometimes engine is named *DGLEngine* or *DGLE engine*, both variants are also correct.

### More information
Visit [official website](http://dglengine.org) for more details about this project and also plugins, articles, tutorials, examples and support.

## Features

* Full cross-platform.
* Plugin based architecture with micro-core principles.
* Flexible and highly extensible via plugins.
* Multi render support: OpenGL, OpenGL ES, Direct3D.
* Set of visual editors with scripting support for rapid application development.
* Provides all necessary API for development high quality 2D and 3D real-time applications.
* Flexible but clear abstraction levels allow you programming without any restrictions from the API.
* Made by experienced in game development professionals for you with love!

## Supported Platforms

* Windows Desktop (XP and later)
* Linux (_in development_)
* Mac OS X (_in development_)
* iOS (_planned_)
* Android (_planned_)
* Windows RT (_planned_)

## Supported Programming Languages

* C++
* C# (any .Net compatible language as well)
* Delphi and Free Pascal
* Lua (for scripting purpose only)

## Getting Started

Simple C++ *"HelloWorld"* application for Windows.
This code will setup engine, create window and render *"Hello, World!"* text on screen via default font.
You can copy-paste this code to your *'main.cpp'* and you are done! Also don't forget to copy engine headers
(you can find them in *"include/cpp"* folder) and library *'DGLE.dll'* (you can find it in *"bin/win"* folder)
in suitable locations to be found by compiler and executable.

```cpp
#include "DGLE.h"

using namespace DGLE;

DGLE_DYNAMIC_FUNC

IEngineCore *pEngineCore = NULL;
IBitmapFont *pFont = NULL;

void DGLE_API Init(void *pParametr)
{
	IResourceManager *resMan;
	pEngineCore->GetSubSystem(ESS_RESOURCE_MANAGER, (IEngineSubSystem *&)resMan);
	resMan->GetDefaultResource(EOT_BITMAP_FONT, (IEngBaseObj *&)pFont);
}

void DGLE_API Render(void *pParametr)
{
	pFont->Draw2D(0, 0, "Hello World!");
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if ( GetEngine("DGLE.dll", pEngineCore) )
	{
		pEngineCore->InitializeEngine(NULL, "My First App");
		pEngineCore->AddProcedure(EPT_INIT, &Init);
		pEngineCore->AddProcedure(EPT_RENDER, &Render);
		pEngineCore->StartEngine();
		FreeEngine();
	}
	else
		MessageBoxA(NULL, "Couldn't load \"DGLE.dll\"!", "My First App", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
	
	return 0;
}
```

You can find more examples in the *"src/examples"* folder. Also for other supported languages.

You can find build instructions [here](https://raw.github.com/DGLE-HQ/DGLE/master/docs/HowToBuild.txt).

Also if you want to participate in the project development process you must read and understand [Development Rules](https://raw.github.com/DGLE-HQ/DGLE/master/docs/DevelopmentRules.txt).

## Documentation

You can find engine API documentation and articles in the [wiki](https://github.com/DGLE-HQ/DGLE/wiki).

## License

Licensed under the [LGPL license](http://en.wikipedia.org/wiki/GNU_Lesser_General_Public_License).

## Latest SDK version

Compiled and ready to use latest cross-platform SDK always could be found [here](https://github.com/DGLE-HQ/DGLE/releases/latest).

See [ChangeLog](https://raw.github.com/DGLE-HQ/DGLE/master/ChangeLog.txt) for full list of changes.

## Credits

### Project Leader and Lead Programmer

[Andrey Korotkov aka DRON](http://dronprogs.org)

### Additional Programming

Alexey Shaydurov aka Ash (3D graphics programming)

Kuznecov Stanislav aka STAlkerS (plugins and tools development)

Mikhail Shestakov aka Mike (C# GUI and tools development, also png and jpg loaders)

Firstin Anton aka First (Delphi/C# headers and minor additional programming)

Semen Kotlyarov aka Kimer (plugins and tools development)

Ilya Sivkov aka Sin (DCP packs and minor additional programming)

Svyatoslav Lobov aka FascioRoma (GUI components development)

Vlad Fomin aka phomm (Delphi/C# headers and examples)

### Special Thanks

artyks aka Ksenya Artemova for logo design

Woolf for project support

MadRain for Pyro particles

John Moff, R.I.P. friend

All DeeProSoft employees for project support

## Contacts

Visit [http://dglengine.org](http://dglengine.org) for more information.

Follow us on [Twitter](https://twitter.com/dglengine), [Facebook](https://www.facebook.com/dglengine) or [VKontakte](http://vk.com/dglengine).

Feel free to mail us [hello@dglengine.org](mailto:hello@dglengine.org) with any questions.