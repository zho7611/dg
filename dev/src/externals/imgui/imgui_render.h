//
// Copyright (c) 2009-2010 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// Warranty.  In no event will the authors be held liable for any damages
// Arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// Including commercial applications, and to alter it and redistribute it
// Freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

// Source altered and distributed from https://github.com/AdrienHerubel/imgui
// Source altered and distributed by Jiho Choi

#pragma once

bool imguiRenderGLInit();
void imguiRenderGLDestroy();
void imguiRenderGLDraw(int width, int height);
void imguiRenderBegin();
void imguiRenderEnd();
