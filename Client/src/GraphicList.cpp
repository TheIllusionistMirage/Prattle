#include "../include/GraphicList.hpp"

namespace prattle
{
    GraphicList::GraphicList()
    {
        /* Load the texture files for the panel, the scrollers and the list item's graphic element. */

        // some ugly raw pointer usage since setting the texture of sf::RectangleShape works only this way.
        m_panelTexturePtr = new sf::Texture;

        if (!m_panelTexturePtr->loadFromFile("resources/images/custom/graphic_list_panel.png"))
            throw std::runtime_error("ERROR :: Unable to load texture file \'resources/images/custom/graphic_list_panel.png\'");

        if (!m_scrollTexture.loadFromFile("resources/images/custom/scroller.png"))
            throw std::runtime_error("ERROR :: Error in opening scroller texture.");

        m_itemTexturePtr = std::make_shared<sf::Texture>();

        if (!m_itemTexturePtr->loadFromFile("resources/images/custom/graphic_list_item.png"))
            throw std::runtime_error("ERROR :: Unable to open texture file.");


        /* Initialize list items. */
        // by default the maximum list item height is set to 15.
        m_itemHeight = 15;

        /* Initialize the panel. */
        // by default the maximum listbox size is set to 250 by 250.
        setDefaultMaxSize(sf::Vector2f{250, 250});
        // set default base size, position and texture
        m_boundary.setSize(m_maxBoundarySize);
        m_boundary.setPosition(sf::Vector2f{0, 0});
        m_boundary.setTexture(m_panelTexturePtr);

        /* Initialize the scrollers. */
        m_scrollerUp.setTexture(m_scrollTexture);
        m_scrollerDown.setTexture(m_scrollTexture);

        m_scrollerUp.setTextureRect(sf::IntRect{0, 0, 10, 10});
        m_scrollerDown.setTextureRect(sf::IntRect{0, 10, 10, 10});

        m_scrollerUp.setPosition(sf::Vector2f{m_boundary.getPosition().x +
                                 m_boundary.getSize().x - 25,
                                 m_boundary.getPosition().y + 5});

        m_scrollerDown.setPosition(sf::Vector2f{m_boundary.getPosition().x +
                                   m_boundary.getSize().x - 55,
                                   m_boundary.getPosition().y +
                                   m_boundary.getSize().y - 15});

        // since during default initialization there are
        // no list items, the scrollers are hidden.
        m_scrollerUpVisible = false;
        m_scrollerDownVisible = false;

        m_defaultMessage = tgui::Label::create();
        m_defaultMessage->setTextSize(12);
        m_defaultMessage->setTextColor(sf::Color{70, 66, 66});
        m_defaultMessage->setPosition(sf::Vector2f{getBounds().left + 5, getBounds().top + 5});
    }

    GraphicList::~GraphicList()
    {
        // deallocate the memory pointed to by the only raw pointer in the program.
        delete m_panelTexturePtr;
    }

    // the initialize() function is called from within
    // TGUI to initialize the font pointer with the
    // global font of the GUI.
    void GraphicList::initialize(tgui::Container *const parent)
    {
        Widget::initialize(parent);
        m_font = parent->getGlobalFont();
        m_defaultMessage->setTextFont(m_font);
    }

    // the draw function as interhited from tgui::Widget.
    // This is called each frame automatically by TGUI.
    void GraphicList::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // draw the panel...
        target.draw(m_boundary, states);

        // then draw the items...
        for (auto& i : m_items)
        {
            // draw a list item if it is inside the bounds of the panel
            if ((i->getPosition().y + i->getBounds().height <
                 m_boundary.getPosition().y + m_boundary.getSize().y) &&
                (i->getPosition().y > m_boundary.getPosition().y)
               )
                target.draw(*i, states);
        }

        if (m_defaultMessage->isVisible())
            target.draw(*m_defaultMessage, states);

        // and finally draw the scrollers
        // (if they are visible of course)

        if (m_scrollerUpVisible)
        {
            target.draw(m_scrollerUp, states);
        }

        if (m_scrollerDownVisible)
        {
            target.draw(m_scrollerDown, states);
        }
    }

    // this method returns true only if mouse is
    // inside the panel and automatically called
    // from within TGUI if a mouse hover happens.
    bool GraphicList::mouseOnWidget(float x, float y)
    {
        return m_boundary.getGlobalBounds().contains(x,y);
    }

    tgui::Widget::Ptr GraphicList::clone()
    {
        return std::make_shared<GraphicList>(* this);
    }

    // add a new list item to the list box.
    void GraphicList::addItem(const std::string& label)
    {
        // first create a new GraphicListItem object
        // and initialize it with default parameters

        if (m_defaultMessage->isVisible())
            m_defaultMessage->hide();

        m_items.push_back(std::make_shared<GraphicListItem>(label,
                                                            m_itemHeight,
                                                            sf::Color::Black,
                                                            m_itemTexturePtr,
                                                            1,
                                                            5));

//        if (m_pFont == nullptr)
//        std::cout << "n" << std::endl;
        //m_items.back()->initialize(m_parent);
        m_items.back()->getTextWidget()->setTextFont(m_font);
        //m_items.back()->setFont(m_font);

        // the y-coord of the position is multiplied by 20, well,
        // because each list item is 15 pixels tall and have a
        // vertical space of 5 pixels between two adjacent list items.
        m_items.back()->setPosition(sf::Vector2f{m_boundary.getGlobalBounds().left + 10,
                                                20 * m_items.size() +
                                                m_boundary.getGlobalBounds().top});

        // if after adding the new list item, the
        // total height of the list items exceed
        // the default max. size of the panel,
        // unhide the down scroller
        if (m_boundary.getSize().y < m_items.size() * 20)
            m_scrollerDownVisible = true;
    }

    // remove items when the index of the item to be removed is known
    void GraphicList::removeItem(unsigned int index)
    {
        for (unsigned int i = 0; i < m_items.size(); i++)
        {
            // Find the item with the given index, and erase it.
            // Then move the suceeding items vertically upward
            // by an amount of 20.
            if (i == index)
            {
                m_items.erase(m_items.begin() + i);

                for (auto j = i; j < m_items.size(); j++)
                {
                    m_items[j]->setPosition(sf::Vector2f{m_items[j]->getPosition().x,
                                            m_items[j]->getPosition().y - 20});
                }
            }
        }

        if (m_items.size() == 0)
            if (!m_defaultMessage->isVisible())
                m_defaultMessage->show();
    }

    // remove items when the label of the item to be removed is known
    void GraphicList::removeItem(const std::string& itemName)
    {
        for (unsigned int i = 0; i < m_items.size(); i++)
        {
            if (m_items[i]->getTextWidget()->getText() == itemName)
            {
                m_items.erase(m_items.begin() + i);

                for (auto j = i; j < m_items.size(); j++)
                {
                    m_items[j]->setPosition(sf::Vector2f{m_items[j]->getPosition().x,
                                                         m_items[j]->getPosition().y - 20});
                }
            }
        }

        if (m_items.size() == 0)
            if (!m_defaultMessage->isVisible())
                m_defaultMessage->show();
    }

    std::string GraphicList::getSelected()
    {
        if (m_selected != nullptr)
            return m_selected->getTextWidget()->getText();
        else
            return "";
    }

    // set the position of the list box.
    void GraphicList::setPosition(const tgui::Layout& position)
    {
        // The position is a Layout object is passed as the
        // parameter to be enable to use TGUI's excellent
        // relative position setting capabilities. See the
        // TGUI documentation entry for Layout for more details.

        auto p = position.getValue();

        m_boundary.setPosition(p);

        int t = 0;
        for (auto& i : m_items)
        {
            i->setPosition(sf::Vector2f{p.x + 10, p.y + 20 * t++});
        }

        m_scrollerUp.setPosition(sf::Vector2f{m_boundary.getPosition().x +
                                 m_boundary.getSize().x - 25,
                                 m_boundary.getPosition().y + 5});

        m_scrollerDown.setPosition(sf::Vector2f{m_boundary.getPosition().x +
                                   m_boundary.getSize().x - 25,
                                   m_boundary.getPosition().y +
                                   m_boundary.getSize().y - 15});

        m_defaultMessage->setPosition(sf::Vector2f{getBounds().left + 5, getBounds().top + 5});
    }

    sf::Vector2f GraphicList::getPosition()
    {
        return sf::Vector2f{m_boundary.getPosition().x, m_boundary.getPosition().y};
    }

    // Do stuff if te mouse wheel is moved. This
    // function is called from within TGUI in
    // case of a mouse wheel movement event is
    // registered by the GUI.
    void GraphicList::mouseWheelMoved(int delta, int x, int y)
    {
        // Since a wheel movement occurred either in
        // a upward or downward direction, the
        // scroller for the opposite direction must
        // now become active.

        // By default, it is assumed the scrolling
        // in both directions is possible in case
        // of a mouse wheel movement event.

        bool upScrollable = true;
        bool downScrollable = true;

        // At any given time either the upward or the
        // downward scroller or both are visible.
        if (m_scrollerUpVisible || m_scrollerDownVisible)
        {
            // If scrolling downwards...
            if (delta < 0)
            {
                // Even one unit of downward movement is eligible
                // to make an upward movement possible.
                m_scrollerUpVisible = true;

                // Check if the end of the list has been reached, i.e.,
                // the maximum possible scroll value is reached.
                if((m_items.back()->getPosition().y +
                    m_items.back()->getBounds().height) -
                    (m_boundary.getPosition().y +
                     m_boundary.getSize().y) < 0)
                {
                    // then further downward scrolling ain't possible.
                    downScrollable = false;
                    m_scrollerDownVisible = false;
                }

                // If the end of the list has not yet been reached...
                if (downScrollable)
                {
                    // Move all items upwards up 20 pixels.
                    for (unsigned int i = 0; i < m_items.size(); i++)
                    {
                        m_items[i]->setPosition(sf::Vector2f{m_items[i]->getPosition().x,
                                                m_items[i]->getPosition().y - 20});
                    }

                    // Check if the end of the list has been reached, i.e.,
                    // the maximum possible scroll value is reached.
                    if((m_items.back()->getPosition().y +
                        m_items.back()->getBounds().height) -
                        (m_boundary.getPosition().y +
                         m_boundary.getSize().y) < 0)
                    {
                        // then further downward scrolling ain't possible.
                        downScrollable = false;
                        m_scrollerDownVisible = false;
                    }
                }
            }
            // Else, if scrolling upwards...
            else
            {
                // Even one unit of upward movement is eligible
                // to make a downward movement possible.
                m_scrollerDownVisible = true;

                // Check if the top of the list has been reached, i.e.,
                // the minimum possible scroll value is reached.
                if(m_items[0]->getPosition().y - m_boundary.getPosition().y > 0)
                {
                    // then further upward scrolling ain't possible.
                    upScrollable = false;
                    m_scrollerUpVisible = false;
                }

                // If the top of the list has not yet been reached...
                if (upScrollable)
                {
                    // Move all items downwards up 20 pixels.
                    for (unsigned int i = 0; i < m_items.size(); i++)
                    {
                        m_items[i]->setPosition(sf::Vector2f{m_items[i]->getPosition().x,
                                                m_items[i]->getPosition().y + 20});
                    }

                    // Check if the top of the list has been reached, i.e.,
                    // the minimum possible scroll value is reached.
                    if(m_items[0]->getPosition().y - m_boundary.getPosition().y > 0)
                    {
                        // then further upward scrolling ain't possible.
                        upScrollable = false;
                        m_scrollerUpVisible = false;
                    }
                }
            }
        }
    }

    // Handle mouse movement inside the
    // bounds of the listbox. This method
    // is automatically called from within TGUI.
    void GraphicList::mouseMoved(float x, float y)
    {
        // If the mouse is hovering over a list item,
        // increase it's size a bit to have a hovering
        // effect. And when the mouse leaves the list
        // item on hich it was hovering, the size of
        // the list item is set back to the normal
        // default size.
        for (unsigned int i = 0; i < m_items.size(); i++)
        {
            if (m_items[i]->mouseOnWidget(x, y))
                m_items[i]->getTextWidget()->setTextSize(m_itemHeight + 2);
            else
                m_items[i]->getTextWidget()->setTextSize(m_itemHeight);
        }
    }

    // Handle stuff when the left mouse button
    // is pressed inside the list box. This
    // method is called from within TGUI.
    void GraphicList::leftMousePressed(float x, float y)
    {
        // Loop through the list items to find which
        // item was left clicked. Then set it's label
        // color to a different one.
        for (unsigned int i = 0; i < m_items.size(); i++)
        {
            if (m_items[i]->mouseOnWidget(x, y))
            {
                m_items[i]->getTextWidget()->setTextColor(sf::Color::White);
                m_selected = m_items[i];
            }
            else
                m_items[i]->getTextWidget()->setTextColor(sf::Color::Black);
        }

        if (m_scrollerDown.getGlobalBounds().contains(x, y) && m_scrollerDownVisible)
        {
            bool downScrollable = true;
            m_scrollerUpVisible = true;

            if (downScrollable)
            {
                for (unsigned int i = 0; i < m_items.size(); i++)
                {
                    (*m_items[i]).setPosition(sf::Vector2f{(*m_items[i]).getPosition().x, (*m_items[i]).getPosition().y - 20});
                }

                if((m_items.back()->getPosition().y + m_items.back()->getBounds().height) - (m_boundary.getPosition().y + m_boundary.getSize().y) == -15)
                {
                    downScrollable = false;
                    m_scrollerDownVisible = false;
                }
            }
        }

        if (m_scrollerUp.getGlobalBounds().contains(x, y) && m_scrollerUpVisible)
        {
            bool upScrollable = true;
            m_scrollerDownVisible = true;

            if (upScrollable)
            {
                for (unsigned int i = 0; i < m_items.size(); i++)
                {
                    (*m_items[i]).setPosition(sf::Vector2f{(*m_items[i]).getPosition().x, (*m_items[i]).getPosition().y + 20});
                }

                if((*m_items[0]).getPosition().y - m_boundary.getPosition().y == 20)
                {
                    upScrollable = false;
                    m_scrollerUpVisible = false;
                }
            }
        }
    }

    void GraphicList::setDefaultMaxSize(const sf::Vector2f& size)
    {
        m_maxBoundarySize = size;
    }

    void GraphicList::setFont(std::shared_ptr<sf::Font> font)
    {
        //m_pFont = font;
    }

    sf::FloatRect GraphicList::getBounds()
    {
        return m_boundary.getGlobalBounds();
    }

    unsigned int GraphicList::getItemCount()
    {
        return m_items.size();
    }

    void GraphicList::setDefaultMessage(const std::string& message)
    {
        m_defaultMessage->setTextSize(12);
        m_defaultMessage->setTextFont(m_font);
        m_defaultMessage->setTextColor(sf::Color{70, 66, 66});
        m_defaultMessage->setPosition(sf::Vector2f{getBounds().left + 5, getBounds().top + 5});

        if (getItemCount() == 0)
        {
            m_defaultMessage->setText(message);
            m_defaultMessage->show();
        }
        else
        {
            m_defaultMessage->hide();
        }
    }

    void GraphicList::setStatusOfItem(const std::string& listItem, unsigned int status)
    {
        for (auto& i : m_items)
        {
            if (i->getTextWidget()->getText() == listItem)
            {
                i->setStatus(status);
            }
        }
    }

    void GraphicList::clear()
    {
        m_selected = nullptr;
        m_items.clear();
        if (!m_defaultMessage->isVisible())
            m_defaultMessage->show();
    }
}
