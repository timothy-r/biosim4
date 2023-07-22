/usr/bin/ld: 
common/libbiosim4_common.a(gridBuilder.cpp.o): in function `BS::CreateGridBarrierVisitor::~CreateGridBarrierVisitor()':
gridBuilder.cpp:(.text._ZN2BS24CreateGridBarrierVisitorD2Ev[_ZN2BS24CreateGridBarrierVisitorD5Ev]+0x24): undefined reference to `BS::GridLocationVisitor::~GridLocationVisitor()'


/usr/bin/ld: common/libbiosim4_common.a(gridCircle.cpp.o): in function `BS::GridShape::GridShape()':
gridCircle.cpp:(.text._ZN2BS9GridShapeC2Ev[_ZN2BS9GridShapeC5Ev]+0x8): undefined reference to `vtable for BS::GridShape'

/usr/bin/ld: gridCircle.cpp:(.text._ZN2BS9GridShapeC2Ev[_ZN2BS9GridShapeC5Ev]+0xc): undefined reference to `vtable for BS::GridShape'

/usr/bin/ld: common/libbiosim4_common.a(gridCircle.cpp.o): in function `BS::GridCircle::~GridCircle()':
gridCircle.cpp:(.text._ZN2BS10GridCircleD2Ev[_ZN2BS10GridCircleD5Ev]+0x20): undefined reference to `BS::GridShape::~GridShape()'

/usr/bin/ld: common/libbiosim4_common.a(gridCircle.cpp.o):(.data.rel.ro._ZTIN2BS10GridCircleE[_ZTIN2BS10GridCircleE]+0x10): undefined reference to `typeinfo for BS::GridShape'

/usr/bin/ld: common/libbiosim4_common.a(gridRectangle.cpp.o): in function `BS::GridRectangle::~GridRectangle()':
gridRectangle.cpp:(.text._ZN2BS13GridRectangleD2Ev[_ZN2BS13GridRectangleD5Ev]+0x20): undefined reference to `BS::GridShape::~GridShape()'

/usr/bin/ld: common/libbiosim4_common.a(gridRectangle.cpp.o):(.data.rel.ro._ZTIN2BS13GridRectangleE[_ZTIN2BS13GridRectangleE]+0x10): undefined reference to `typeinfo for BS::GridShape'