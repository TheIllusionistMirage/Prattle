#ifndef GRAPHICLISTITEM_HPP
#define GRAPHICLISTITEM_HPP

#include <TGUI/TGUI.hpp>

namespace prattle
{
    /**

        GraphicLisItem -

        A graphical list item class which is later used to create the
        GraphicList and the GraphicTab classes.

        GraphicListItem is an extension of tgui::Widget that provides the
        possibility to use a graphic sprite prefixed to a tgui::Label.
        This class' usage is very specifically directed for Prattle, a
        simple SFML chat application (even though by adding a few lines
        of code here and there, you can generalize it even more).

    **/

    class GraphicListItem : public tgui::Widget
    {
        public:
            typedef std::shared_ptr<GraphicListItem> Ptr;
            typedef std::shared_ptr<const GraphicListItem> ConstPtr;

        public:
            GraphicListItem();
            GraphicListItem(const std::string& label,
                            const unsigned int& labelSize,
                            const sf::Color& labelColor,
                            std::shared_ptr<sf::Texture> itemTexture ,
                            const unsigned short& status,
                            const float& spacing
                           );
            //void initialize(tgui::Container *const parent);
            //void initialize(std::shared_ptr<sf::Font> font);
            virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
            bool mouseOnWidget(float x, float y) const override;
            virtual Widget::Ptr clone() const override;
            virtual void setPosition(const tgui::Layout2d& position);// override;
            sf::Vector2f getPosition();
            void setStatus(unsigned short status);  // 0 for the online sprite and 1 for the offline sprite
            unsigned int getStatus();  // 0 for the online sprite and 1 for the offline sprite
            tgui::Label::Ptr getNotifWidget();
            tgui::Label::Ptr getTextWidget();
            sf::FloatRect getBounds();
            void loadTexture(const std::string& file);
            void setFont(std::shared_ptr<sf::Font> font);
            void setNotif(const std::string& notif = "");
            std::string getNotif();
            void setActive(const std::string& username,
                           bool active = true);
            bool isActive();

        private:
            tgui::Label::Ptr             m_itemText;    // The label widget from TGUI.
            tgui::Label::Ptr             m_notifText;   // The label widget from TGUI.
            std::shared_ptr<sf::Texture> m_itemTexture; // The texture file for the graphic.
            sf::Sprite                   m_itemSprite;  // The Sprite of the graphic.
            sf::FloatRect                m_bounds;      // The rextangular boundary of the GraphicListItem object.
            float                        m_spacing;     // The amoutn of space between the graphic and the label.
            float                        m_verSpace;    // Needed to display the text at correct postition.
            unsigned short               m_status;      // The status if the graphic image. In our very specific case
                                                        // it is used to calculate the texture rect of the graphic sprite.
            //std::shared_ptr<sf::Font>    m_font;
            bool                         m_active;
    };
}

#endif // GRAPHICLISTITEM_HPP
