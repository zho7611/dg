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

#include "engine_first.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>

using namespace dg;

// Some math headers don't have PI defined.
static const float PI = 3.14159265f;

void imguifree(void* ptr, void* userptr);
void* imguimalloc(size_t size, void* userptr);

#define STBTT_malloc(x,y)    imguimalloc(x,y)
#define STBTT_free(x,y)      imguifree(x,y)
#define STB_TRUETYPE_IMPLEMENTATION
#include "../../externals/stb_truetype/stb_truetype.h"

void imguifree(void* ptr, void* /*userptr*/) {
  free(ptr);
}

void* imguimalloc(size_t size, void* /*userptr*/) {
  return malloc(size);
}

static const unsigned TEMP_COORD_COUNT = 100;
static float g_tempCoords[TEMP_COORD_COUNT*2];
static float g_tempNormals[TEMP_COORD_COUNT*2];
static float g_tempVertices[TEMP_COORD_COUNT * 12 + (TEMP_COORD_COUNT - 2) * 6];
static float g_tempTextureCoords[TEMP_COORD_COUNT * 12 + (TEMP_COORD_COUNT - 2) * 6];
static float g_tempColors[TEMP_COORD_COUNT * 24 + (TEMP_COORD_COUNT - 2) * 12];

static const int CIRCLE_VERTS = 8*4;
static float g_circleVerts[CIRCLE_VERTS*2];

static stbtt_bakedchar g_cdata[96]; // ASCII 32..126 is 95 glyphs
//static GLuint g_ftex = 0;
//static GLuint g_whitetex = 0;
//static GLuint g_vao = 0;
//static GLuint g_vbos[3] = {0, 0, 0};
//static GLuint g_program = 0;
//static GLuint g_programViewportLocation = 0;
//static GLuint g_programTextureLocation = 0;

static Texture* g_font_tex = NULL;
static VertexBuffer* g_vb = NULL;
static IndexBuffer* g_ib = NULL;
static ImguiShaderEffect* g_shader;

inline unsigned int RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
  return (r) | (g << 8) | (b << 16) | (a << 24);
}

struct CustomVertexFormat {
  Vector4 position;
  Vector2 texcoord;
  Vector4 color;
};

static void drawPolygon(const float* coords, unsigned numCoords, float r, unsigned int col) {
  if (numCoords > TEMP_COORD_COUNT) numCoords = TEMP_COORD_COUNT;
  for (unsigned i = 0, j = numCoords-1; i < numCoords; j=i++) {
    const float* v0 = &coords[j*2];
    const float* v1 = &coords[i*2];
    float dx = v1[0] - v0[0];
    float dy = v1[1] - v0[1];
    float d = sqrtf(dx*dx+dy*dy);
    if (d > 0) {
      d = 1.0f/d;
      dx *= d;
      dy *= d;
    }
    g_tempNormals[j*2+0] = dy;
    g_tempNormals[j*2+1] = -dx;
  }
  float colf[4] = { (float) (col&0xff) / 255.f, (float) ((col>>8)&0xff) / 255.f, (float) ((col>>16)&0xff) / 255.f, (float) ((col>>24)&0xff) / 255.f};
  float colTransf[4] = { (float) (col&0xff) / 255.f, (float) ((col>>8)&0xff) / 255.f, (float) ((col>>16)&0xff) / 255.f, 0};
  for (unsigned i = 0, j = numCoords-1; i < numCoords; j=i++) {
    float dlx0 = g_tempNormals[j*2+0];
    float dly0 = g_tempNormals[j*2+1];
    float dlx1 = g_tempNormals[i*2+0];
    float dly1 = g_tempNormals[i*2+1];
    float dmx = (dlx0 + dlx1) * 0.5f;
    float dmy = (dly0 + dly1) * 0.5f;
    float   dmr2 = dmx*dmx + dmy*dmy;
    if (dmr2 > 0.000001f) {
      float   scale = 1.0f / dmr2;
      if (scale > 10.0f) scale = 10.0f;
      dmx *= scale;
      dmy *= scale;
    }
    g_tempCoords[i*2+0] = coords[i*2+0]+dmx*r;
    g_tempCoords[i*2+1] = coords[i*2+1]+dmy*r;
  }

  int vSize = numCoords * 12 + (numCoords - 2) * 6;
  int uvSize = numCoords * 2 * 6 + (numCoords - 2) * 2 * 3;
  int cSize = numCoords * 4 * 6 + (numCoords - 2) * 4 * 3;
  float * v = g_tempVertices;
  float * uv = g_tempTextureCoords;
  memset(uv, 0, uvSize * sizeof(float));
  float * c = g_tempColors;
  memset(c, 1, cSize * sizeof(float));

  float * ptrV = v;
  float * ptrC = c;
  for (unsigned i = 0, j = numCoords-1; i < numCoords; j=i++) {
    *ptrV = coords[i*2];
    *(ptrV+1) = coords[i*2 + 1];
    ptrV += 2;
    *ptrV = coords[j*2];
    *(ptrV+1) = coords[j*2 + 1];
    ptrV += 2;
    *ptrV = g_tempCoords[j*2];
    *(ptrV+1) = g_tempCoords[j*2 + 1];
    ptrV += 2;
    *ptrV = g_tempCoords[j*2];
    *(ptrV+1) = g_tempCoords[j*2 + 1];
    ptrV += 2;
    *ptrV = g_tempCoords[i*2];
    *(ptrV+1) = g_tempCoords[i*2 + 1];
    ptrV += 2;
    *ptrV = coords[i*2];
    *(ptrV+1) = coords[i*2 + 1];
    ptrV += 2;

    *ptrC = colf[0];
    *(ptrC+1) = colf[1];
    *(ptrC+2) = colf[2];
    *(ptrC+3) = colf[3];
    ptrC += 4;
    *ptrC = colf[0];
    *(ptrC+1) = colf[1];
    *(ptrC+2) = colf[2];
    *(ptrC+3) = colf[3];
    ptrC += 4;
    *ptrC = colTransf[0];
    *(ptrC+1) = colTransf[1];
    *(ptrC+2) = colTransf[2];
    *(ptrC+3) = colTransf[3];
    ptrC += 4;
    *ptrC = colTransf[0];
    *(ptrC+1) = colTransf[1];
    *(ptrC+2) = colTransf[2];
    *(ptrC+3) = colTransf[3];
    ptrC += 4;
    *ptrC = colTransf[0];
    *(ptrC+1) = colTransf[1];
    *(ptrC+2) = colTransf[2];
    *(ptrC+3) = colTransf[3];
    ptrC += 4;
    *ptrC = colf[0];
    *(ptrC+1) = colf[1];
    *(ptrC+2) = colf[2];
    *(ptrC+3) = colf[3];
    ptrC += 4;
  }

  for (unsigned i = 2; i < numCoords; ++i) {
    *ptrV = coords[0];
    *(ptrV+1) = coords[1];
    ptrV += 2;
    *ptrV = coords[(i-1)*2];
    *(ptrV+1) = coords[(i-1)*2+1];
    ptrV += 2;
    *ptrV = coords[i*2];
    *(ptrV+1) = coords[i*2 + 1];
    ptrV += 2;

    *ptrC = colf[0];
    *(ptrC+1) = colf[1];
    *(ptrC+2) = colf[2];
    *(ptrC+3) = colf[3];
    ptrC += 4;
    *ptrC = colf[0];
    *(ptrC+1) = colf[1];
    *(ptrC+2) = colf[2];
    *(ptrC+3) = colf[3];
    ptrC += 4;
    *ptrC = colf[0];
    *(ptrC+1) = colf[1];
    *(ptrC+2) = colf[2];
    *(ptrC+3) = colf[3];
    ptrC += 4;
  }
  g_shader->SetTextureByName(TXT("GlobalImguiTexture"), g_renderer.default_textures[Renderer::DefaultTextures_White]);
  g_shader->CommitChanges();

  //glBindTexture(GL_TEXTURE_2D, g_whitetex);
  //glBindVertexArray(g_vao);
  //glBindBuffer(GL_ARRAY_BUFFER, g_vbos[0]);
  //glBufferData(GL_ARRAY_BUFFER, vSize*sizeof(float), v, GL_STATIC_DRAW);
  //glBindBuffer(GL_ARRAY_BUFFER, g_vbos[1]);
  //glBufferData(GL_ARRAY_BUFFER, uvSize*sizeof(float), uv, GL_STATIC_DRAW);
  //glBindBuffer(GL_ARRAY_BUFFER, g_vbos[2]);
  //glBufferData(GL_ARRAY_BUFFER, cSize*sizeof(float), c, GL_STATIC_DRAW);
  //glDrawArrays(GL_TRIANGLES, 0, (numCoords * 2 + numCoords - 2)*3);
  int num_triangles = (numCoords*2+numCoords-2);
  {
    CustomVertexFormat* buff = NULL;
    g_vb->Lock(0, num_triangles*3*g_vb->stride(), (uint8_t*&)buff, GpuLockType_Discard);
    for (int idx = 0; idx < num_triangles*3; ++idx) {
      buff[idx].position = Vector4(v[idx*2], v[idx*2+1], 0.0f, 1.0f);
      buff[idx].texcoord = Vector2(uv[idx*2], uv[idx*2+1]);
      buff[idx].color = Vector4(c[idx*4], c[idx*4+1], c[idx*4+2], c[idx*4+3]);
    }
    g_vb->Unlock();
  }
  g_vb->SubmitVertexBuffer();
  g_ib->SubmitIndexBuffer();
  RC_DrawIndexedPrimitive(DrawPrimitiveType_TriangleList, num_triangles);
}

static void drawRect(float x, float y, float w, float h, float fth, unsigned int col) {
  float verts[4*2] = {
    x+0.5f, y+0.5f,
    x+w-0.5f, y+0.5f,
    x+w-0.5f, y+h-0.5f,
    x+0.5f, y+h-0.5f,
  };
  drawPolygon(verts, 4, fth, col);
}

/*
static void drawEllipse(float x, float y, float w, float h, float fth, unsigned int col)
{
        float verts[CIRCLE_VERTS*2];
        const float* cverts = g_circleVerts;
        float* v = verts;

        for (int i = 0; i < CIRCLE_VERTS; ++i)
        {
                *v++ = x + cverts[i*2]*w;
                *v++ = y + cverts[i*2+1]*h;
        }

        drawPolygon(verts, CIRCLE_VERTS, fth, col);
}
*/

static void drawRoundedRect(float x, float y, float w, float h, float r, float fth, unsigned int col) {
  const unsigned n = CIRCLE_VERTS/4;
  float verts[(n+1)*4*2];
  const float* cverts = g_circleVerts;
  float* v = verts;

  for (unsigned i = 0; i <= n; ++i) {
    *v++ = x+w-r + cverts[i*2]*r;
    *v++ = y+h-r + cverts[i*2+1]*r;
  }

  for (unsigned i = n; i <= n*2; ++i) {
    *v++ = x+r + cverts[i*2]*r;
    *v++ = y+h-r + cverts[i*2+1]*r;
  }

  for (unsigned i = n*2; i <= n*3; ++i) {
    *v++ = x+r + cverts[i*2]*r;
    *v++ = y+r + cverts[i*2+1]*r;
  }

  for (unsigned i = n*3; i < n*4; ++i) {
    *v++ = x+w-r + cverts[i*2]*r;
    *v++ = y+r + cverts[i*2+1]*r;
  }
  *v++ = x+w-r + cverts[0]*r;
  *v++ = y+r + cverts[1]*r;

  drawPolygon(verts, (n+1)*4, fth, col);
}


static void drawLine(float x0, float y0, float x1, float y1, float r, float fth, unsigned int col) {
  float dx = x1-x0;
  float dy = y1-y0;
  float d = sqrtf(dx*dx+dy*dy);
  if (d > 0.0001f) {
    d = 1.0f/d;
    dx *= d;
    dy *= d;
  }
  float nx = dy;
  float ny = -dx;
  float verts[4*2];
  r -= fth;
  r *= 0.5f;
  if (r < 0.01f) r = 0.01f;
  dx *= r;
  dy *= r;
  nx *= r;
  ny *= r;

  verts[0] = x0-dx-nx;
  verts[1] = y0-dy-ny;

  verts[2] = x0-dx+nx;
  verts[3] = y0-dy+ny;

  verts[4] = x1+dx+nx;
  verts[5] = y1+dy+ny;

  verts[6] = x1+dx-nx;
  verts[7] = y1+dy-ny;

  drawPolygon(verts, 4, fth, col);
}

bool imguiRenderGLInit() {
  const char* fontpath = "res/engine/DroidSans.ttf";
  for (int i = 0; i < CIRCLE_VERTS; ++i) {
    float a = (float)i/(float)CIRCLE_VERTS * PI*2;
    g_circleVerts[i*2+0] = cosf(a);
    g_circleVerts[i*2+1] = sinf(a);
  }

  // Load font.
  FILE* fp = fopen(fontpath, "rb");
  if (!fp) return false;
  fseek(fp, 0, SEEK_END);
  int size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  unsigned char* ttfBuffer = (unsigned char*)malloc(size);
  if (!ttfBuffer) {
    fclose(fp);
    return false;
  }

  fread(ttfBuffer, 1, size, fp);
  fclose(fp);
  fp = 0;

  unsigned char* bmap = (unsigned char*)malloc(512*512);
  if (!bmap) {
    free(ttfBuffer);
    return false;
  }

  stbtt_BakeFontBitmap(ttfBuffer,0, 15.0f, bmap,512,512, 32,96, g_cdata);
  //// Can free ttf_buffer at this point

  //glGenTextures(1, &g_ftex);
  //glBindTexture(GL_TEXTURE_2D, g_ftex);
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512,512, 0, GL_RED, GL_UNSIGNED_BYTE, bmap);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  g_font_tex = new Texture;
  g_font_tex->AddRef();
  g_font_tex->Init(GpuUsage_None, Size2(512, 512), GpuFormat_A8, 0);
  g_font_tex->BindResource();
  uint8_t* buff = NULL;
  int pitch = 0;
  {
    Texture::ScopedLock locker(*g_font_tex, buff, pitch);
    for (int y = 0; y < 512; ++y) {
      for (int x = 0; x < 512; ++x) {
        buff[y*pitch + x] = bmap[y*512+x];
      }
    }
  }
  g_shader = LoadResource<ImguiShaderEffect>(g_engine_config->imgui_effect_path_.Get());
  if (!g_shader) {
    // Imgui shader not found
    return false;
  }
  g_shader->AddRef();

  g_shader->SetTextureByName(TXT("GlobalImguiTexture"), g_font_tex);
  g_shader->CommitChanges();
  //RC_SetTexture(TXT("GlobalImguiTexture"), 0, g_font_tex->device_texture_handle_);

  //// Can free ttf_buffer at this point
  //unsigned char white_alpha = 255;
  //glGenTextures(1, &g_whitetex);
  //glBindTexture(GL_TEXTURE_2D, g_whitetex);
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1, 1, 0, GL_RED, GL_UNSIGNED_BYTE, &white_alpha);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  VertexDecl decl;
  decl.elements().Clear();
  VertexElement pos(VertexElement::DataType_Float4, VertexElement::Usage_Position);
  VertexElement tex(VertexElement::DataType_Float2, VertexElement::Usage_TexCoord, 0);
  VertexElement col(VertexElement::DataType_Float4, VertexElement::Usage_TexCoord, 1);
  decl.elements().PushBack(pos);
  decl.elements().PushBack(tex);
  decl.elements().PushBack(col);
  const int kMaxVerts = 1024;
  g_vb = new VertexBuffer;
  g_vb->AddRef();
  g_vb->InitVertexBufferWithDecl(&decl, kMaxVerts, GpuUsage_Dynamic, GpuPool_Default);
  g_vb->BindResource();
  g_ib = new IndexBuffer;
  g_ib->AddRef();
  MeshBuilder::InitIndexBuffer(*g_ib, kMaxVerts);
  g_ib->BindResource();
  //glGenVertexArrays(1, &g_vao);
  //glGenBuffers(3, g_vbos);

  //glBindVertexArray(g_vao);
  //glEnableVertexAttribArray(0);
  //glEnableVertexAttribArray(1);
  //glEnableVertexAttribArray(2);

  //glBindBuffer(GL_ARRAY_BUFFER, g_vbos[0]);
  //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
  //glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
  //glBindBuffer(GL_ARRAY_BUFFER, g_vbos[1]);
  //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
  //glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
  //glBindBuffer(GL_ARRAY_BUFFER, g_vbos[2]);
  //glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*4, (void*)0);
  //glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
  //g_program = glCreateProgram();

  //const char * vs =
  //    "#version 150\n"
  //    "uniform vec2 Viewport;\n"
  //    "in vec2 VertexPosition;\n"
  //    "in vec2 VertexTexCoord;\n"
  //    "in vec4 VertexColor;\n"
  //    "out vec2 texCoord;\n"
  //    "out vec4 vertexColor;\n"
  //    "void main(void)\n"
  //    "{\n"
  //    "    vertexColor = VertexColor;\n"
  //    "    texCoord = VertexTexCoord;\n"
  //    "    gl_Position = vec4(VertexPosition * 2.0 / Viewport - 1.0, 0.f, 1.0);\n"
  //    "}\n";
  //GLuint vso = glCreateShader(GL_VERTEX_SHADER);
  //glShaderSource(vso, 1, (const char **)  &vs, NULL);
  //glCompileShader(vso);
  //glAttachShader(g_program, vso);

  //const char * fs =
  //    "#version 150\n"
  //    "in vec2 texCoord;\n"
  //    "in vec4 vertexColor;\n"
  //    "uniform sampler2D Texture;\n"
  //    "out vec4  Color;\n"
  //    "void main(void)\n"
  //    "{\n"
  //    "    float alpha = texture(Texture, texCoord).r;\n"
  //    "    Color = vec4(vertexColor.rgb, vertexColor.a * alpha);\n"
  //    "}\n";
  //GLuint fso = glCreateShader(GL_FRAGMENT_SHADER);

  //glShaderSource(fso, 1, (const char **) &fs, NULL);
  //glCompileShader(fso);
  //glAttachShader(g_program, fso);

  //glBindAttribLocation(g_program,  0,  "VertexPosition");
  //glBindAttribLocation(g_program,  1,  "VertexTexCoord");
  //glBindAttribLocation(g_program,  2,  "VertexColor");
  //glBindFragDataLocation(g_program, 0, "Color");
  //glLinkProgram(g_program);
  //glDeleteShader(vso);
  //glDeleteShader(fso);

  //glUseProgram(g_program);
  //g_programViewportLocation = glGetUniformLocation(g_program, "Viewport");
  //g_programTextureLocation = glGetUniformLocation(g_program, "Texture");

  //glUseProgram(0);


  free(ttfBuffer);
  free(bmap);

  return true;
}

void imguiRenderGLDestroy() {
  DG_SAFE_RELEASE(g_font_tex);
  DG_SAFE_RELEASE(g_vb);
  DG_SAFE_RELEASE(g_ib);
  DG_SAFE_RELEASE(g_shader);

  //if (g_ftex) {
  //  glDeleteTextures(1, &g_ftex);
  //  g_ftex = 0;
  //}

  //if (g_vao) {
  //  glDeleteVertexArrays(1, &g_vao);
  //  glDeleteBuffers(3, g_vbos);
  //  g_vao = 0;
  //}

  //if (g_program) {
  //  glDeleteProgram(g_program);
  //  g_program = 0;
  //}

}

static void getBakedQuad(stbtt_bakedchar *chardata, int pw, int ph, int char_index,
    float *xpos, float *ypos, stbtt_aligned_quad *q) {
  stbtt_bakedchar *b = chardata + char_index;
  int round_x = STBTT_ifloor(*xpos + b->xoff);
  int round_y = STBTT_ifloor(*ypos - b->yoff);

  q->x0 = (float)round_x;
  q->y0 = (float)round_y;
  q->x1 = (float)round_x + b->x1 - b->x0;
  q->y1 = (float)round_y - b->y1 + b->y0;

  q->s0 = b->x0 / (float)pw;
  q->t0 = b->y0 / (float)pw;
  q->s1 = b->x1 / (float)ph;
  q->t1 = b->y1 / (float)ph;

  *xpos += b->xadvance;
}

static const float g_tabStops[4] = {150, 210, 270, 330};

static float getTextLength(stbtt_bakedchar *chardata, const char* text) {
  float xpos = 0;
  float len = 0;
  while (*text) {
    int c = (unsigned char)*text;
    if (c == '\t') {
      for (int i = 0; i < 4; ++i) {
        if (xpos < g_tabStops[i]) {
          xpos = g_tabStops[i];
          break;
        }
      }
    } else if (c >= 32 && c < 128) {
      stbtt_bakedchar *b = chardata + c-32;
      int round_x = STBTT_ifloor((xpos + b->xoff) + 0.5);
      len = round_x + b->x1 - b->x0 + 0.5f;
      xpos += b->xadvance;
    }
    ++text;
  }
  return len;
}

static void drawText(float x, float y, const char *text, int align, unsigned int col) {
  //if (!g_ftex) return;
  if (!text) return;

  if (align == IMGUI_ALIGN_CENTER)
    x -= getTextLength(g_cdata, text)/2;
  else if (align == IMGUI_ALIGN_RIGHT)
    x -= getTextLength(g_cdata, text);

  float r = (float) (col&0xff) / 255.f;
  float g = (float) ((col>>8)&0xff) / 255.f;
  float b = (float) ((col>>16)&0xff) / 255.f;
  float a = (float) ((col>>24)&0xff) / 255.f;

  // Assume orthographic projection with units = screen pixels, origin at top left
  //glBindTexture(GL_TEXTURE_2D, g_ftex);
  g_shader->SetTextureByName(TXT("GlobalImguiTexture"), g_font_tex);
  g_shader->CommitChanges();

  const float ox = x;

  while (*text) {
    int c = (unsigned char)*text;
    if (c == '\t') {
      for (int i = 0; i < 4; ++i) {
        if (x < g_tabStops[i]+ox) {
          x = g_tabStops[i]+ox;
          break;
        }
      }
    } else if (c >= 32 && c < 128) {
      stbtt_aligned_quad q;
      getBakedQuad(g_cdata, 512,512, c-32, &x,&y,&q);

      float v[12] = {
        q.x0, q.y0,
        q.x1, q.y1,
        q.x1, q.y0,
        q.x0, q.y0,
        q.x0, q.y1,
        q.x1, q.y1,
      };
      float uv[12] = {
        q.s0, q.t0,
        q.s1, q.t1,
        q.s1, q.t0,
        q.s0, q.t0,
        q.s0, q.t1,
        q.s1, q.t1,
      };
      float c[24] = {
        r, g, b, a,
        r, g, b, a,
        r, g, b, a,
        r, g, b, a,
        r, g, b, a,
        r, g, b, a,
      };
      //glBindVertexArray(g_vao);
      //glBindBuffer(GL_ARRAY_BUFFER, g_vbos[0]);
      //glBufferData(GL_ARRAY_BUFFER, 12*sizeof(float), v, GL_STATIC_DRAW);
      //glBindBuffer(GL_ARRAY_BUFFER, g_vbos[1]);
      //glBufferData(GL_ARRAY_BUFFER, 12*sizeof(float), uv, GL_STATIC_DRAW);
      //glBindBuffer(GL_ARRAY_BUFFER, g_vbos[2]);
      //glBufferData(GL_ARRAY_BUFFER, 24*sizeof(float), c, GL_STATIC_DRAW);
      //glDrawArrays(GL_TRIANGLES, 0, 6);
      int num_triangles = 2;
      {
        CustomVertexFormat* buff = NULL;
        g_vb->Lock(0, num_triangles*3*g_vb->stride(), (uint8_t*&)buff, GpuLockType_Discard);
        for (int idx = 0; idx < num_triangles*3; ++idx) {
          buff[idx].position = Vector4(v[idx*2], v[idx*2+1], 0.0f, 1.0f);
          buff[idx].texcoord = Vector2(uv[idx*2], uv[idx*2+1]);
          buff[idx].color = Vector4(c[idx*4], c[idx*4+1], c[idx*4+2], c[idx*4+3]);
        }
        g_vb->Unlock();
      }
      g_vb->SubmitVertexBuffer();
      g_ib->SubmitIndexBuffer();
      RC_DrawIndexedPrimitive(DrawPrimitiveType_TriangleList, num_triangles);
    }
    ++text;
  }

  //glEnd();
  //glDisable(GL_TEXTURE_2D);
}

Profiler s_imgui_profiler(TXT("imgui_begin"), DG_MAKE_COLOR_ARGB(0xffff0000), false);

void imguiRenderBegin() {
  s_imgui_profiler.Start();
  g_shader->PreRender();
}

void imguiRenderEnd() {
  g_shader->PostRender();
  s_imgui_profiler.Stop();
}

void imguiRenderGLDraw(int width, int height) {
  const imguiGfxCmd* q = imguiGetRenderQueue();
  int nq = imguiGetRenderQueueSize();

  const float s = 1.0f/8.0f;

  RenderStates saved_states = g_renderer.states;
  Viewport viewport;
  viewport.top_left_x = 0; viewport.top_left_y = 0;
  viewport.width = width; viewport.height = height;
  viewport.min_depth = 0.0f; viewport.max_depth = 1.0f;
  RC_SetViewport(viewport);
  //glViewport(0, 0, width, height);
  //glUseProgram(g_program);
  //glActiveTexture(GL_TEXTURE0);
  //glUniform2f(g_programViewportLocation, (float) width, (float) height);
  //glUniform1i(g_programTextureLocation, 0);

  RC_SetRenderState(RenderStateType_CullMode, CullModeType_CW);
  
  //glDisable(GL_SCISSOR_TEST);
  RC_SetRenderState(RenderStateType_ScissorTestEnable, 0);
  g_shader->SetVector4ByName(TXT("Viewport"), Vector4(float(width), float(height), 0, 0));
  g_shader->SetTextureByName(TXT("GlobalImguiTexture"), g_font_tex);
  g_shader->CommitChanges();

  for (int i = 0; i < nq; ++i) {
    const imguiGfxCmd& cmd = q[i];
    if (cmd.type == IMGUI_GFXCMD_RECT) {
      if (cmd.rect.r == 0) {
        drawRect((float)cmd.rect.x*s+0.5f, (float)cmd.rect.y*s+0.5f,
            (float)cmd.rect.w*s-1, (float)cmd.rect.h*s-1,
            1.0f, cmd.col);
      } else {
        drawRoundedRect((float)cmd.rect.x*s+0.5f, (float)cmd.rect.y*s+0.5f,
            (float)cmd.rect.w*s, (float)cmd.rect.h*s,
            (float)cmd.rect.r*s, 1.0f, cmd.col);
      }
    } else if (cmd.type == IMGUI_GFXCMD_LINE) {
      drawLine(cmd.line.x0*s, cmd.line.y0*s, cmd.line.x1*s, cmd.line.y1*s, cmd.line.r*s, 1.0f, cmd.col);
    } else if (cmd.type == IMGUI_GFXCMD_TRIANGLE) {
      if (cmd.flags == 1) {
        const float verts[3*2] = {
          (float)cmd.rect.x*s+0.5f, (float)cmd.rect.y*s+0.5f,
          (float)cmd.rect.x*s+0.5f+(float)cmd.rect.w*s-1, (float)cmd.rect.y*s+0.5f+(float)cmd.rect.h*s/2-0.5f,
          (float)cmd.rect.x*s+0.5f, (float)cmd.rect.y*s+0.5f+(float)cmd.rect.h*s-1,
        };
        drawPolygon(verts, 3, 1.0f, cmd.col);
      }
      if (cmd.flags == 2) {
        const float verts[3*2] = {
          (float)cmd.rect.x*s+0.5f, (float)cmd.rect.y*s+0.5f+(float)cmd.rect.h*s-1,
          (float)cmd.rect.x*s+0.5f+(float)cmd.rect.w*s/2-0.5f, (float)cmd.rect.y*s+0.5f,
          (float)cmd.rect.x*s+0.5f+(float)cmd.rect.w*s-1, (float)cmd.rect.y*s+0.5f+(float)cmd.rect.h*s-1,
        };
        drawPolygon(verts, 3, 1.0f, cmd.col);
      }
    } else if (cmd.type == IMGUI_GFXCMD_TEXT) {
      drawText(cmd.text.x, cmd.text.y, cmd.text.text, cmd.text.align, cmd.col);
    } else if (cmd.type == IMGUI_GFXCMD_SCISSOR) {
      if (cmd.flags) {
        //glEnable(GL_SCISSOR_TEST);
        //glScissor(cmd.rect.x, cmd.rect.y, cmd.rect.w, cmd.rect.h);
        RC_SetRenderState(RenderStateType_ScissorTestEnable, 1);
        int rect_x = MyMax(0, (int)cmd.rect.x);
        int rect_y = MyMax(0, (int)cmd.rect.y);
        int rect_w = MyMax(0, (int)cmd.rect.w);
        int rect_h = MyMax(0, (int)cmd.rect.h);
        if (cmd.rect.x > width) rect_x = width;
        if (cmd.rect.y > height) rect_y = height;
        if (rect_x + rect_w > width) rect_w = width - rect_x;
        if (rect_y + rect_y > height) rect_h = height - rect_y;
        RC_SetScissorRect(rect_x, rect_y, rect_w, rect_h);
      } else {
        //glDisable(GL_SCISSOR_TEST);
        RC_SetRenderState(RenderStateType_ScissorTestEnable, 0);
      }
    }
  }
  //glDisable(GL_SCISSOR_TEST);
  RC_SetRenderState(RenderStateType_ScissorTestEnable, 0);
  g_renderer.states = saved_states;
  g_renderer.states.Apply();
}