#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include "../include/GraphicList.hpp"

namespace prattle
{
    class Menu : public tgui::Widget
    {
        public:
            typedef std::shared_ptr<Menu> Ptr;
            typedef std::shared_ptr<const Menu> ConstPtr;

            Menu();
            void initialize(tgui::Container *const parent);
            virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
            virtual Widget::Ptr clone() override;
            bool mouseOnWidget(float x, float y);
            virtual void mouseMoved(float x, float y) override;
            virtual void mouseLeftWidget() override;
            virtual void leftMousePressed(float x, float y) override;
            virtual void mouseWheelMoved(int delta, int x, int y) override;
            bool isFriendListButtonPressed();
            virtual void setPosition(const tgui::Layout& position) override;
            const sf::Vector2f getPosition();
            const sf::Vector2f getSize();

            void setFont(std::shared_ptr<sf::Font> font);

            void initList();

            const std::string getSelectedFriend();

            const sf::FloatRect getFriendlistBounds();

        private:

            sf::Texture m_menuItemTexture;
            std::vector<sf::Sprite> m_menuItemSprites;
            sf::FloatRect m_bounds;
            //std::shared_ptr<sf::Font> m_font;
            float m_spacing;
            GraphicList::Ptr m_friendList;
    };
}

#endif // MENU_HPP
