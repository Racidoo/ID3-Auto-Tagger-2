#pragma once

#include <memory>
#include <wx/wx.h>

#include "Interfaces/IMediaEntity.hpp"

class MediaDetailsPanel : public wxScrolledWindow {
  public:
    MediaDetailsPanel(wxWindow *_parent, std::shared_ptr<IMediaEntity> _source)
        : wxScrolledWindow(_parent), source(_source) {
        this->SetScrollRate(15, 15);
    }
    ~MediaDetailsPanel() = default;

  protected:
    std::shared_ptr<IMediaEntity> source;
};
