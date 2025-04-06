#include "../include/SettingsWindow.h"

SettingsWindow::SettingsWindow(wxWindow *_parent, wxWindowID _winid,
                               const wxPoint &_pos, const wxSize &_size)
    : wxScrolledWindow(_parent, _winid, _pos, _size),
      mainSizer(new wxBoxSizer(wxVERTICAL)) {}

SettingsWindow::~SettingsWindow() {}
