#ifndef PAGE_H_
#define PAGE_H_

#include "../widgets/container.h"
#include "../common/style.h"
#include "../common/typeinfo.h"

class Page : public Container {
public:
  Page(){};

  void init(lv_obj_t* handle);

  virtual void assign(lv_obj_t* newHandle);

protected:
  virtual void onInitializing();
  virtual void onChildAdded(Widget& target, const TypeInfo& type);

protected:
  Style pageStyle;
};

#endif