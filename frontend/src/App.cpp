#include "../include/App.h"
#include "../include/MainFrame.h"
#include "../include/IconProvider.h"

bool App::OnInit() {
    wxArtProvider::Push(new IconProvider());
    MainFrame *frame = new MainFrame();
    frame->Show(true);
    return true;
}