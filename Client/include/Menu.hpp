#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include "../include/GraphicList.hpp"

namespace prattle
{
    class Menu : public tgui::Container
    {
        public:
            enum class Item
            {
                FriendPanel,
                SearchPanel,
                SettingsPanel,
                AboutPanel
            };

        public:
            typedef std::shared_ptr<Menu> Ptr;
            typedef std::shared_ptr<const Menu> ConstPtr;

            Menu();
            //void initialize(std::shared_ptr<sf::Font> font);
            virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
            virtual Widget::Ptr clone() const override;
            virtual bool mouseOnWidget(float x, float y) const override;
            virtual void mouseMoved(float x, float y) override;
            virtual void mouseLeftWidget() override;
            virtual void leftMousePressed(float x, float y) override;
            virtual void leftMouseReleased(float x, float y) override;
            virtual void mouseWheelMoved(int delta, int x, int y) override;
            virtual void textEntered(sf::Uint32 key) override;
            virtual void keyPressed(const sf::Event::KeyEvent& event) override;
            //void update();
            bool isFriendListButtonPressed();
            virtual void setPosition(const tgui::Layout2d& position);// override;
            sf::Vector2f getPosition();
            sf::Vector2f getSize();

            void setFont(std::shared_ptr<sf::Font> font);

            void initList(const std::vector<std::string>& friends);

            void resetList();

            std::string getSelectedFriend();

            unsigned int getStatus(const std::string& friendName);

            GraphicList::Ptr getFriendlist();

            tgui::Widget::Ptr getMenuItem(Item itemType);

            sf::FloatRect getItemBounds(const unsigned int& itemIndex);

            std::string getSearchFieldText();

            void showSearchResults(const std::vector<std::string>& results);

            void closeSearchPanel();

            std::string getSelectedResult();

            void resetSearchPanel();

            void setNotif(const std::string& friendLabel, const std::string& notif = "");

            std::string getNotif(const std::string& friendLabel);

            //std::string getSelectedTab();

        private:

            tgui::Theme::Ptr m_theme;
            sf::Texture m_menuItemTexture;
            std::vector<sf::Sprite> m_menuItemSprites;
            sf::FloatRect m_bounds;
            float m_spacing;
            GraphicList::Ptr m_friendList;
            struct Filter
            {   sf::RectangleShape m_filter;
                bool               m_active;
            } m_inactiveFilter;

            // the search box
            tgui::Panel::Ptr       m_searchPanel;
            tgui::EditBox::Ptr     m_searchField;
            tgui::Button::Ptr      m_searchButton;
            tgui::Label::Ptr       m_resultMessage;
            tgui::ListBox::Ptr     m_results;
            tgui::Button::Ptr      m_addFriendButton;

            // global font
            //std::shared_ptr<sf::Font> m_font;
    };
}

#endif // MENU_HPP
