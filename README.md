DalgiEngine - Sweet 3D Game Engine
==================================

DalgiEngine is a 3D game engine designed for compact and powerful game development.

Screenshots
-----------
![Cammy Screenshot](http://zho.pe.kr/blog/shot/cammy_s.png)
![Huriza Screenshot](http://zho.pe.kr/blog/shot/huriza_s.png)
![Viewer Screenshot](http://zho.pe.kr/blog/shot/viewer_s.png)

The models are not included in the git repository.

Features
--------

- Platforms
   - Windows for now, but designed for multi-platform
- Rendering
   - Support DX9 and OpenGL 2.0+
   - Cross-platform shader effect (JSON format)
   - Shadow mapping with CSM/soft/self-shadow
   - Post-processing effect (bloom/gamma-correction)
- Basic UI system
- Character skinning and animation
- Core library
   - Serialization and package file support
   - UID, console window, critical section, timer, thread, memory pool, profiling, smart pointer and so on
- Tools
   - Test framework for unit-test
   - COLLADA scene importer with skinning/animation
   - Viewer

License
-------
- GPL License

Before build
------------
- For Windows
   - Install DirectX SDK June 2010
   - Needs Visual Studio 2008 SP1
- Other platforms are not supported for now

How to build
------------
Clone a copy of the main dg git repo by running:

```dos
git clone git://github.com/zho7611/dg
```

Open **dev/src/all.sln** via Visual Studio 2008

Set the *viewer* project as "Startup Project"

Change solution configuration to "Debug" or "Release" for DX9 renderer

Run from menu : **Build > Rebuild Solution**

How to run the Viewer
-----------------

Run **bin\viewer.exe**

External libraries
------------------
Other external libraries are located in dev/src/externals folder

- [glLoadGen]( https://bitbucket.org/alfonse/glloadgen )
- [stb_image]( http://nothings.org/stb_image.c )
- [stb_image_write]( http://nothings.org/stb/stb_image_write.h )
- [stb_truetype]( http://nothings.org/stb/stb_truetype.h )
- [imgui]( https://github.com/AdrienHerubel/imgui )
