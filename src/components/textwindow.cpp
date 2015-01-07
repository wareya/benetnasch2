#include "textwindow.hpp"

namespace Sys
{
    TextWindow::TextWindow(entityid_t myEntity) : Component(myEntity)
    {
        scrollback_size = 500;
        width_pixels = 370;
        height_pixels = 225;
        x_onscreen = 40;
        y_onscreen = 120;
        background = check_texture_pile("sprites/textwindow.png");
        TextWindows.add(this);
    }
    TextWindow::~TextWindow()
    {
        TextWindows.remove(this);
    }
    void TextWindow::force_purge_extra_scrollback()
    {
        while(lines.size() > scrollback_size)
            lines.pop_front();
    }
    void TextWindow::append_line(std::string newline)
    {
        lines.push_back(newline);
        force_purge_extra_scrollback();
    }
    void TextWindow::set_scrollback(unsigned numlines)
    {
        scrollback_size = numlines;
        force_purge_extra_scrollback();
        lines.shrink_to_fit();
    }
    Collection<TextWindow> TextWindows;
}
