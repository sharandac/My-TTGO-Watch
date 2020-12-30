#ifndef IR_CONFIG_H
#define IR_CONFIG_H

#include "config.h"
#include "quickglui/quickglui.h"
#include "IRButton.h"

#define MAX_IR_BUTTONS 16

class IRConfig : public BaseJsonConfig
{
public:
  IRConfig();

  InfraButton* add(const char* name);
  void del(const char* name);
  InfraButton* get(const char* name);

  InfraButton* get(int id) { return buttons[id]; }
  int totalCount() { return count; }

  void sendListNames(BluetoothJsonResponse& target);
  void sendButtonEdit(BluetoothJsonResponse& target, const char* name);

public:
  int defBtnWidth = 95;
  int defBtnHeight = 33;
  int defSpacing = 3;

protected:
  virtual bool onSave(JsonDocument& document);
  virtual bool onLoad(JsonDocument& document);
  virtual size_t getJsonBufferSize() { return 16000; }

protected:
  InfraButton *buttons[MAX_IR_BUTTONS];
  int count = 0;
};

#endif

