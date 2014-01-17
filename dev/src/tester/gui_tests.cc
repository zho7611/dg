// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#include "../gui/gui_first.h"
#include "tester.h"

void TestPresent(float wait_secs);
bool TestRenderTarget(const Cstr* testname);

TEST(GuiLabel) {
  TEST_DEPENDS_ON(PrepareRender);
  g_renderer.BeginScene();
  Canvas canvas;
  canvas.target_ = g_renderer.view_state.target;
  canvas.clear_color_ = DG_MAKE_COLOR_INT(255, 255, 255, 0);
  canvas.ClearCanvas();
  canvas.PreRender();
  {
    {
      Label label;
      FontFace* font_face = FontFace::GetFontFace(TXT("font_c"));
      // Init the label
      {
        label.text.Init(font_face, TXT("TestLabel1"));
        label.SetLocalRect(0, 0, 100, 32);
        label.border = new Border;
        label.border->style = Border::Style_Line;
        label.border->color = DG_MAKE_COLOR_INT(255, 255, 255, 255);
      }
      // Draw the label
      {
        Point2 coord(100, 100);
        canvas.origins_.Push(coord);
        {
          label.DrawGui(canvas);
        }
        canvas.origins_.Pop();
      }
      // Draw the label
      {
        label.border->style = Border::Style_Null;
        Point2 coord(200, 200);
        canvas.origins_.Push(coord);
        {
          label.DrawGui(canvas);
          label.border->style = Border::Style_Line;
          label.border->color = DG_MAKE_COLOR_INT(128, 128, 128, 255);
          coord.Set(300, 100);
          canvas.origins_.Push(coord);
          {
            label.text_color = DG_MAKE_COLOR_INT(255, 0, 255, 0);
            label.text.Clear();
            label.text.Init(font_face, TXT("TestLabel4"));
            label.DrawGui(canvas);
          }
          canvas.origins_.Pop();
        }
        canvas.origins_.Pop();
      }
      // Draw the label
      {
        label.border->style = Border::Style_Default;
        label.SetLocalRect(10, 10, 100, 128);
        Point2 coord(50, 50);
        canvas.origins_.Push(coord);
        {
          label.DrawGui(canvas);
        }
        canvas.origins_.Pop();
      }
      label.text.Clear();
    }
  }
  canvas.PostRender();
  g_renderer.EndScene();
  TestPresent();
  EXPECT_TRUE(TestRenderTarget(TXT("gui_label")));
}

TEST(GuiImageBorder) {
  TEST_DEPENDS_ON(GuiLabel);
  g_renderer.BeginScene();
  Canvas canvas;
  canvas.target_ = g_renderer.view_state.target;
  canvas.PreRender();
  canvas.clear_color_ = 0x00;
  canvas.ClearCanvas();
  {
    ImageBorder border;
    border.ReadFromPath(TXT("res/generated/image_border.bin"));
    border.color = 0xFFFFFFFF;
    // Draw the border
    {
      Point2 coord(20, 20);
      canvas.origins_.Push(coord);
      Rect rect(0, 0, 64, 32);
      border.SetLocalRect(rect);
      border.DrawGui(canvas);
      {
        coord.Set(40, 20+32);
        rect.Set(0, 0, 128, 64);
        canvas.origins_.Push(coord);
        border.SetLocalRect(rect);
        border.DrawGui(canvas);
        canvas.origins_.Pop();
      }
      rect.Set(20 + 20, 32 + 32, 256, 32);
      border.SetLocalRect(rect);
      border.DrawGui(canvas);
      canvas.origins_.Pop();
    }
  }
  canvas.PostRender();
  g_renderer.EndScene();
  TestPresent();
  EXPECT_TRUE(TestRenderTarget(TXT("gui_image_border")));
}
