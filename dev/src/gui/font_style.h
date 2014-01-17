// Copyright (C) 2010 by Jiho Choi. All rights reserved.
// [License]

#pragma once

namespace Gui {
enum FontStyle {
    FontStyle_Small = 0,
    FontStyle_Medium = 1,
    FontStyle_Large = 2,
    FontStyle_Num = 3,
};

class FontDesc {
public:
  String Name_;
  String Size_;
};

class FontStyleManager {
public:
  FontDesc Descs_[ FontStyle_Num ];
};

extern FontStyleManager FontStyleManager_;
}
