#if !defined(APP_H)
#define APP_H

#include <wx/wx.h>

// Define a custom application class
class App : public wxApp {
  public:
    virtual bool OnInit();
};

#endif // APP_H
