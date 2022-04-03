
#pragma once
#include "Object.h"
#include "TreeTags.h"s

class TreeVisitor {
 public:
  virtual void VisitVarDecl(const Object& node) {}
  virtual void VisitFuncDef(const Object& node) {}
  virtual void VisitBlock(const Object& node) {}
  virtual void VisitIf(const Object& node) {}
  virtual void VisitWhile(const Object& node) {}
  virtual void VisitExpr(const Object& node) {}
  virtual void VisitStmt(const Object& node) {}
  // ... all the rest visitor methods...

  virtual TreeVisitor* Clone(void) const = 0;
  TreeVisitor(void);
  TreeVisitor(const TreeVisitor&) = default;
};
