#include "../include/Menu.hpp"

namespace prattle
{
    void Menu::initialize(tgui::Container *const parent)
    {
        Widget::initialize(parent);

        if (parent->getGlobalFont())
        {
            m_font = parent->getGlobalFont();
            m_friendList->initialize(parent);
            //m_friendList->setFont(m_font);

            //if(m_font == nullptr)
            //std::cout << "ASDASD" << std::endl;
        }
    }

    Menu::Menu() : m_friendList{std::make_shared<GraphicList>()}
    {
        if (!m_menuItemTexture.loadFromFile("resources/images/custom/menu_items.png"))
            throw std::runtime_error("ERROR :: Unable to open image file \'menu_items.png\'.");

        m_menuItemSprites.push_back(sf::Sprite{m_menuItemTexture});
        m_menuItemSprites.push_back(sf::Sprite{m_menuItemTexture});
        m_menuItemSprites.push_back(sf::Sprite{m_menuItemTexture});
        m_menuItemSprites.push_back(sf::Sprite{m_menuItemTexture});

        m_menuItemSprites[0].setTextureRect(sf::IntRect{0, 0, 60, 60});
        m_menuItemSprites[1].setTextureRect(sf::IntRect{60, 0, 60, 60});
        m_menuItemSprites[2].setTextureRect(sf::IntRect{120, 0, 60, 60});
        m_menuItemSprites[3].setTextureRect(sf::IntRect{180, 0, 60, 60});

        m_spacing = 10.f;

        m_menuItemSprites[0].setPosition(sf::Vector2f{0, 0});
        m_menuItemSprites[1].setPosition(sf::Vector2f{m_menuItemSprites[0].getPosition().x + m_menuItemSprites[0].getGlobalBounds().width + m_spacing, m_menuItemSprites[0].getPosition().y});
        m_menuItemSprites[2].setPosition(sf::Vector2f{m_menuItemSprites[1].getPosition().x + m_menuItemSprites[1].getGlobalBounds().width + m_spacing, m_menuItemSprites[1].getPosition().y});
        m_menuItemSprites[3].setPosition(sf::Vector2f{m_menuItemSprites[2].getPosition().x + m_menuItemSprites[2].getGlobalBounds().width + m_spacing, m_menuItemSprites[2].getPosition().y});

        m_bounds = sf::FloatRect{ m_menuItemSprites[0].getPosition().x,
                                  m_menuItemSprites[0].getPosition().y,
                                  m_menuItemSprites[0].getGlobalBounds().width + m_menuItemSprites[1].getGlobalBounds().width + m_menuItemSprites[2].getGlobalBounds().width + m_menuItemSprites[3].getGlobalBounds().width + 3 * m_spacing,
                                  m_menuItemSprites[0].getGlobalBounds().height };

        //m_friendList->setPosition(tgui::Layout{tgui::bindLeft(m_gui) + 50 + 5, tgui::bindTop(m_gui) + m_menu->getSize().y + 50 + 5});
        //m_friendList->setFont(m_gui.getGlobalFont());
//        m_friendList->setFont(m_font);
//
//        m_friendList->addItem("MyFriend1");
//        m_friendList->addItem("MyFriend2");
//        m_friendList->addItem("MyFriend3");
//        m_friendList->addItem("MyFriend4");
//        m_friendList->addItem("MyFriend5");
//        m_friendList->addItem("MyFriend6");
//        m_friendList->addItem("MyFriend7");
//        m_friendList->addItem("MyFriend8");
//        m_friendList->addItem("MyFriend9");
//        m_friendList->addItem("MyFriend10");
//        m_friendList->addItem("MyFriend11");
//        m_friendList->addItem("MyFriend12");
//        m_friendList->addItem("MyFriend13");
//        m_friendList->addItem("MyFriend14");
//        m_friendList->setPosition(tgui::Layout{tgui::bindLeft(m_gui) + 50 + 5, tgui::bindTop(m_gui) + m_menu->getSize().y + 50 + 5});
        //m_friendList->hide();

        //temp= std::make_shared<GraphicListItem>();
    }

    void Menu::initList()
    {
        m_friendList->setPosition(tgui::Layout{m_bounds.left + 5, m_bounds.height + 50 + 5});
        m_friendList->addItem("MyFriend1");
        m_friendList->addItem("MyFriend2");
        m_friendList->addItem("MyFriend3");
        m_friendList->addItem("MyFriend4");
        m_friendList->addItem("MyFriend5");
        m_friendList->addItem("MyFriend6");
        m_friendList->addItem("MyFriend7");
        m_friendList->addItem("MyFriend8");
        m_friendList->addItem("MyFriend9");
        m_friendList->addItem("MyFriend10");
        m_friendList->addItem("MyFriend11");
        m_friendList->addItem("MyFriend12");
        m_friendList->addItem("MyFriend13");
        m_friendList->addItem("MyFriend14");
        m_friendList->hide();
        m_friendList->moveToFront();
    }

    const std::string Menu::getSelectedFriend()
    {
        return m_friendList->getSelected();
    }

    const sf::FloatRect Menu::getFriendlistBounds()
    {
        return m_friendList->getBounds();
    }

    void Menu::setFont(std::shared_ptr<sf::Font> font)
    {
        m_font = font;
        m_friendList->setFont(m_font);
    }

    void Menu::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        for (auto& i : m_menuItemSprites)
            target.draw(i, states);

        if (m_friendList->isVisible())
            target.draw(*m_friendList, states);
    }

    tgui::Widget::Ptr Menu::clone()
    {
        return std::make_shared<Menu>(* this);
    }

    bool Menu::mouseOnWidget(float x, float y)
    {
        //m_friendList->mouseOnWidget(x, y);

        if (m_bounds.contains(x,y) || m_friendList->mouseOnWidget(x, y))
            return true;
        else
        {
            mouseLeftWidget();
            return false;
        }
    }

    void Menu::mouseMoved(float x, float y)
    {
        if (mouseOnWidget(x, y))
        {
            for (unsigned int i = 0; i < m_menuItemSprites.size(); i++)
            {
                if (m_menuItemSprites[i].getGlobalBounds().contains(x, y))
                {
                    m_menuItemSprites[i].setTextureRect(sf::IntRect{60 * (int)i, 60, 60, 60});
                }
                else
                    m_menuItemSprites[i].setTextureRect(sf::IntRect{60 * (int)i, 0, 60, 60});
            }

            m_friendList->mouseMoved(x, y);
        }
    }

    void Menu::mouseWheelMoved(int delta, int x, int y)
    {
        m_friendList->mouseWheelMoved(delta, x, y);
    }

    void Menu::mouseLeftWidget()
    {
        for (unsigned int i = 0; i < m_menuItemSprites.size(); i++)
        {
            m_menuItemSprites[i].setTextureRect(sf::IntRect{60 * (int)i, 0, 60, 60});
        }

        Widget::mouseLeftWidget();
    }

    void Menu::leftMousePressed(float x, float y)
    {
//        if (mouseOnWidget(x, y))
//        {
////            if (m_menuItemSprites[0].getGlobalBounds().contains(x, y))
////            {
////                m_friendlistPressed = true;
////            }
////            else
////                m_friendlistPressed = false;
//        }
////        else
////            m_friendlistPressed = true;
////
////        std::cout << " xyz" << std::endl;

        if (mouseOnWidget(x, y))
        {
            m_friendList->leftMousePressed(x, y);
            //std::cout << m_friendList->getSelected() << std::endl;

            if (m_friendList->isVisible())
                m_friendList->hide();

            else
                m_friendList->show();
        }
    }

//    bool Menu::isFriendListButtonPressed()
//    {
//        return m_friendlistPressed;
//    }

    void Menu::setPosition(const tgui::Layout& position)
    {
        sf::Vector2f p = position.getValue();

        m_bounds.left = p.x;
        m_bounds.top = p.y;

        m_menuItemSprites[0].setPosition(sf::Vector2f{p.x, p.y});
        m_menuItemSprites[1].setPosition(sf::Vector2f{m_menuItemSprites[0].getPosition().x + m_menuItemSprites[0].getGlobalBounds().width + m_spacing, m_menuItemSprites[0].getPosition().y});
        m_menuItemSprites[2].setPosition(sf::Vector2f{m_menuItemSprites[1].getPosition().x + m_menuItemSprites[1].getGlobalBounds().width + m_spacing, m_menuItemSprites[1].getPosition().y});
        m_menuItemSprites[3].setPosition(sf::Vector2f{m_menuItemSprites[2].getPosition().x + m_menuItemSprites[2].getGlobalBounds().width + m_spacing, m_menuItemSprites[2].getPosition().y});
    }

    const sf::Vector2f Menu::getPosition()
    {
        return sf::Vector2f{m_bounds.left, m_bounds.top};
    }

    const sf::Vector2f Menu::getSize()
    {
        return sf::Vector2f{m_bounds.width, m_bounds.height};
    }
}
