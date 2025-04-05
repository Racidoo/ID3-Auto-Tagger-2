#include "../include/App.h"
#include "../include/MainFrame.h"

bool App::OnInit() {
    MainFrame *frame = new MainFrame();
    frame->Show(true);
    return true;
}