// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Composite : public Component {
public:
  List<Ptr<Component> > children;
};

} // namespace dg
