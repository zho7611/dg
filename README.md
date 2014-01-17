DalgiEngine - Sweet 3D Game Engine
==================================

DalgiEngine is a 3D game engine designed for compact and powerful game development. 

Screenshots
-----------
![Cammy Screenshot](http://zho.pe.kr/blog/shot/cammy.png)
![Huriza Screenshot](http://zho.pe.kr/blog/shot/huriza_s.png)

Features
--------

- Platforms
   - Windows for now, but designed for multi-platform
- Rendering
   - DX9/OpenGL 2.0 Renderer
   - Cross-platform Shader Effect (JSON format)
   - CSM based Soft Self Shadow
   - Post-processing Effect
- Basic UI System
- Animation and Skinning
- Base Library
   - Serialization and Package File System
   - UID, Error handling, Command-line options, Console Window, Critical Section, Timer, Thread, Memory Pool, Profiler, Smart Pointer, Reference Count and so on
- Tools
   - Test Framework
   - COLLADA Asset Import Tool
   - Viewer

License
-------
- GPL License

Before build
------------
- For Windows
   - Install DirectX SDK June 2010
   - Needs Visual Studio 2008 SP1

How to build
------------

Clone a copy of the main dg git repo by running:

```dos
git clone git://github.com/zho7611/dg
```

Open **dev/src/all.sln** via Visual Studio 2008

Run from menu : **Build > Rebuild Solution**

How to run viewer
-----------------

Run **bin\viewer.exe**

