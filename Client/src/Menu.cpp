#include "../include/Menu.hpp"

namespace prattle
{
    void Menu::initialize(tgui::Container *const parent)
    {
        Widget::initialize(parent);

        if (parent->getGlobalFont())
        {
            m_font = parent->getGlobalFont();
        }
    }

    Menu::Menu()
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

        m_menuItemSprites[0].setPosition(sf::Vector2f{50, 50});
        m_menuItemSprites[1].setPosition(sf::Vector2f{m_menuItemSprites[0].getPosition().x + m_menuItemSprites[0].getGlobalBounds().width + m_spacing, m_menuItemSprites[0].getPosition().y});
        m_menuItemSprites[2].setPosition(sf::Vector2f{m_menuItemSprites[1].getPosition().x + m_menuItemSprites[1].getGlobalBounds().width + m_spacing, m_menuItemSprites[1].getPosition().y});
        m_menuItemSprites[3].setPosition(sf::Vector2f{m_menuItemSprites[2].getPosition().x + m_menuItemSprites[2].getGlobalBounds().width + m_spacing, m_menuItemSprites[2].getPosition().y});

        m_bounds = sf::FloatRect{ m_menuItemSprites[0].getPosition().x,
                                  m_menuItemSprites[0].getPosition().y,
                                  m_menuItemSprites[0].getGlobalBounds().width + m_menuItemSprites[1].getGlobalBounds().width + m_menuItemSprites[2].getGlobalBounds().width + m_menuItemSprites[3].getGlobalBounds().width + 3 * m_spacing,
                                  m_menuItemSprites[0].getGlobalBounds().height };
    }

    void Menu::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        for (auto& i : m_menuItemSprites)
            target.draw(i, states);
    }

    tgui::Widget::Ptr Menu::clone()
    {
        return std::make_shared<Menu>(* this);
    }

    bool Menu::mouseOnWidget(float x, float y)
    {
        if (m_bounds.contains(x,y))
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
        }
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
        if (mouseOnWidget(x, y))
        {
            if (m_menuItemSprites[0].getGlobalBounds().contains(x, y))
                {
                    if (!m_friendlistVisibility)
                        m_friendlistVisibility = true;
                    else
                        m_friendlistVisibility = false;
                }
        }
    }

    bool Menu::isFriendListButtonPressed()
    {
        return m_friendlistVisibility;
    }

    const sf::Vector2f& Menu::getPosition()
    {
        return sf::Vector2f{m_bounds.left, m_bounds.top};
    }

    const sf::Vector2f& Menu::getSize()
    {
        return sf::Vector2f{m_bounds.width, m_bounds.height};
    }
}
