#include "Core/App.h"
#include "Core/IconProvider.h"
#include "Core/MainFrame.h"

bool App::OnInit() {
    wxArtProvider::Push(new IconProvider());
    MainFrame *frame = new MainFrame();
    frame->Show(true);
    return true;
}