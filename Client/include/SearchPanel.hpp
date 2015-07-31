/**

    A custom search panel based off TGUI's Container class

*/


#ifndef SEARCHPANEL_HPP
#define SEARCHPANEL_HPP

#include <TGUI/TGUI.hpp>

namespace prattle
{
    class SearchPanel : tgui::Container
    {
        public:
            SearchPanel();

            // the initialize() function is called from within
            // TGUI to initialize the font pointer with the
            // global font of the GUI.
            void initialize(tgui::Container *const parent);

            // the draw function as interhited from tgui::Widget.
            // This is called each frame automatically by TGUI.
            virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

            void setSearchString(const std::string& str);
            std::string getSearchString();

            std::vector<std::string> getResults();

        public:

        private:
            // TGUI widgets which form the search panel
            tgui::Panel::Ptr       m_searchPanel;
            tgui::EditBox::Ptr     m_searchField;
            tgui::Button::Ptr      m_searchButton;
            tgui::Label::Ptr       m_resultMessage;
            tgui::ListBox::Ptr     m_results;
            tgui::Button::Ptr      m_addFriendButton;
    };
}

#endif // SEARCHPANEL_HPP
