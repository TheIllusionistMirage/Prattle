#ifndef GRAPHICLIST_HPP
#define GRAPHICLIST_HPP

#include <TGUI/TGUI.hpp>
#include "../include/GraphicListItem.hpp"

namespace prattle
{
    /**

        GraphicList -

        GraphicList is a list object that allows apending a graphical element
        in the beginning of the list items. It uses a vector of GraphicListItem
        for achieving this. This class is specifically directed for use in
        Prattle, s simple SFML chat application, using TGUI for the GUI. So
        it may not be suitable for a generalized use.

    */

    class GraphicList : public tgui::Widget
    {
        public:

            // shared_ptr objects are used for creating GraphicList
            // objects and adding them to the GUI.

            typedef std::shared_ptr<GraphicList> Ptr;
            typedef std::shared_ptr<const GraphicList> ConstPtr;

        public:

            // All code is pretty straightforward.

            GraphicList();
            ~GraphicList();

            // the initialize() function is called from within
            // TGUI to initialize the font pointer with the
            // global font of the GUI.
            //void initialize(tgui::Container *const parent);
            //void initialize(std::shared_ptr<sf::Font> font);

            // the draw function as interhited from tgui::Widget.
            // This is called each frame automatically by TGUI.
            virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

            // this method returns true only if mouse is
            // inside the panel and automatically called
            // from within TGUI if a mouse hover happens.
            bool mouseOnWidget(float x, float y);

            virtual Widget::Ptr clone() override;;

            // add a new list item to the list box.
            void addItem(const std::string& label);

            // remove items when the index of the item to be removed is known
            void removeItem(unsigned int index);

            // remove items when the label of the item to be removed is known
            void removeItem(const std::string& itemName);

            void select(const unsigned int& index);

            void deselect(const unsigned int& index);

            void deselectAll();

            std::string getSelected();

            // set the position of the list box.
            virtual void setPosition(const tgui::Layout2d& position);// override;

            sf::Vector2f getPosition();

            // Do stuff if te mouse wheel is moved. This
            // function is called from within TGUI in
            // case of a mouse wheel movement event is
            // registered by the GUI.
            virtual void mouseWheelMoved(int delta, int x, int y) override;

            // Handle mouse movement inside the
            // bounds of the listbox. This method
            // is automatically called from within TGUI.
            virtual void mouseMoved(float x, float y) override;

            // Handle stuff when the left mouse button
            // is pressed inside the list box. This
            // method is called from within TGUI.
            virtual void leftMousePressed(float x, float y) override;

            void setDefaultMaxSize(const sf::Vector2f& size);

            void setFont(std::shared_ptr<sf::Font> font);

            sf::FloatRect getBounds();

            unsigned int getItemCount();

            void setDefaultMessage(const std::string& message);

            void setStatusOfItem(const std::string& listItem, unsigned int status);

            unsigned int getStatusOfItem(const std::string& listItem);

            void setNotifOfItem(const std::string& listItem, const std::string& notif);

            std::string getNotifOfItem(const std::string& listItem);

            void clear();

        private:

            sf::RectangleShape                m_boundary;           // The rectangular base of the list.

            sf::Texture*                      m_panelTexturePtr;    // The texture of the rectangular base.

            std::vector<GraphicListItem::Ptr> m_items;              // The vector of GraphicListItem objects
                                                                    // which are individual list elements.

            std::shared_ptr<sf::Texture>      m_itemTexturePtr;     // The pointer to the texture for the
                                                                    // ist item's graphic image.

            sf::Vector2f                      m_maxBoundarySize;    // The maximum size of the list object. If
                                                                    // objects are added beyond the size of the
                                                                    // maximum size, then scroll options show up.

            sf::Texture                       m_scrollTexture;      // The texture for the scrollers.

            sf::Sprite                        m_scrollerUp;         // The sprite for the upward scroller.

            sf::Sprite                        m_scrollerDown;       // The sprite for the downward scroller.

            bool                              m_scrollerUpVisible;  // Boolean to store the visibility of upward scroller.

            bool                              m_scrollerDownVisible;// Boolean to store the visibility of downward scroller.

            float                             m_itemHeight;         // The height of each list item.

            std::shared_ptr<GraphicListItem>  m_selected;           // The currently selected list item.

            //std::shared_ptr<sf::Font>         m_font;               // The global font fetched from the tgui::Gui object.

            tgui::Label::Ptr                  m_defaultMessage;
    };
}

#endif // GRAPHICLIST_HPP
