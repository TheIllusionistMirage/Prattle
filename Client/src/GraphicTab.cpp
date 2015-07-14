#include "../include/GraphicTab.hpp"

namespace prattle
{
    void GraphicTab::initialize(tgui::Container *const parent)
    {
        tgui::Widget::initialize(parent);

        if (parent->getGlobalFont())
        {
            m_font = parent->getGlobalFont();
        }
    }

    GraphicTab::GraphicTab()
    {
        if (!m_iconTexture.loadFromFile("resources/images/custom/close_button.png"))
            throw std::runtime_error("ERROR :: Unable to open \'close_button.png\'.");

        m_absoluteBounds = sf::FloatRect{0, 0, 0, 0};
        m_selected = "";
        m_maxDefaultTabSize = sf::Vector2f{150, 25};
        m_firstTabVisible = 0;
        m_lastTabVisible = 0;

        m_tabTexture = new sf::Texture;
        if (!m_tabTexture->loadFromFile("resources/images/custom/graphic_tab.png"))
            throw std::runtime_error("ERROR :: Unable to open graphic tab texture file.");

        m_tabActiveTexture = new sf::Texture;
        if (!m_tabActiveTexture->loadFromFile("resources/images/custom/graphic_tab_active.png"))
            throw std::runtime_error("ERROR :: Unable to open graphic tab texture file.");

        m_GLItemTexPtr = std::make_shared<sf::Texture>();
        if (!m_GLItemTexPtr->loadFromFile("resources/images/custom/graphic_list_item.png"))
            throw std::runtime_error("ERROR :: Unable to open texture file.");

        if (!m_arrowTexture.loadFromFile("resources/images/custom/tab_arrow.png"))
            throw std::runtime_error("ERROR :: Unable to open arrow texture file.");

        m_leftArrow.setTexture(m_arrowTexture);
        m_leftArrow.setTextureRect(sf::IntRect{0, 0, 25 , 25});

        m_rightArrow.setTexture(m_arrowTexture);
        m_rightArrow.setTextureRect(sf::IntRect{25, 0, 25 , 25});

        m_leftArrowVisibile = false;
        m_rightArrowVisibile = false;
    }

    GraphicTab::~GraphicTab()
    {
        delete m_tabTexture;
    }

    void GraphicTab::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        for (unsigned int i = 0; i < m_tabs.size(); i++)
            if (m_tabVisibility[i] == true)
            {
                target.draw(m_tabs[i], states);
                target.draw(m_iconSprites[i], states);
                target.draw(*m_items[i], states);
            }

        if (m_leftArrowVisibile)
            target.draw(m_leftArrow, states);

        if (m_rightArrowVisibile)
            target.draw(m_rightArrow, states);
    }

    tgui::Widget::Ptr GraphicTab::clone()
    {
        return std::make_shared<GraphicTab>(* this);
    }

    bool GraphicTab::mouseOnWidget(float x, float y)
    {
        if (!m_absoluteBounds.contains(x,y))
            for (unsigned int i = 0; i < m_tabs.size(); i++)
                if (m_selected != m_items[i]->getTextWidget()->getText())
                            m_tabs[i].setTexture(m_tabActiveTexture);

        return m_absoluteBounds.contains(x,y);
    }

    void GraphicTab::mouseMoved(float x, float y)
    {
        if (m_tabs.size() && m_items.size())
        {
            if (mouseOnWidget(x, y))
            {
                for (unsigned int i = 0; i < m_tabs.size(); i++)
                {
                    if (m_tabs[i].getGlobalBounds().contains(x, y))
                    {
                        m_tabs[i].setTexture(m_tabTexture);
                    }
                    else
                    {
                        if (m_selected != m_items[i]->getTextWidget()->getText())
                            m_tabs[i].setTexture(m_tabActiveTexture);
                    }

                    if (m_iconSprites[i].getGlobalBounds().contains(x, y))
                    {
                        m_iconSprites[i].setTextureRect(sf::IntRect{0, 0, 10, 10});
                    }
                    else
                        m_iconSprites[i].setTextureRect(sf::IntRect{10, 0, 10, 10});
                }
            }
        }
    }

    void GraphicTab::leftMousePressed(float x, float y)
    {
        if (m_tabs.size() && m_items.size())
        {
            if (mouseOnWidget(x, y))
            {
                for (unsigned int i = 0; i < m_tabs.size(); i++)
                {
                    if (m_tabs[i].getGlobalBounds().contains(x, y) && m_tabVisibility[i])
                    {
                        select(m_items[i]->getTextWidget()->getText());
                        m_tabs[i].setTexture(m_tabTexture);
                    }
                    else
                    {
                        if (m_selected != m_items[i]->getTextWidget()->getText())
                            m_tabs[i].setTexture(m_tabActiveTexture);
                    }

                    if (m_iconSprites[i].getGlobalBounds().contains(x, y) && m_tabVisibility[i])
                    {
                        removeTab(i);
                    }
                }

                if (m_leftArrow.getGlobalBounds().contains(x, y) && m_leftArrowVisibile)
                {
                    m_rightArrowVisibile = true;

                    for (unsigned int i = 0; i < m_tabs.size(); i++)
                    {
                        m_tabs[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x + m_tabs[i].getSize().x + 2, m_tabs[i].getPosition().y});
                        m_iconSprites[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x + m_tabs[i].getSize().x - 15, m_tabs[i].getPosition().y + 7});
                        m_items[i]->setPosition(sf::Vector2f{m_tabs[i].getPosition().x + 7, m_tabs[i].getPosition().y + 7});
                    }

                    m_firstTabVisible--;
                    m_tabVisibility[m_firstTabVisible] = true;
                    m_tabVisibility[m_lastTabVisible] = false;
                    m_lastTabVisible--;

                    m_absoluteBounds = sf::FloatRect{m_absoluteBounds.left + m_maxDefaultTabSize.x + 2, m_absoluteBounds.top, m_absoluteBounds.width, m_tabs.back().getSize().y};

                    if (!m_firstTabVisible)
                        m_leftArrowVisibile= false;
                }

                if (m_rightArrow.getGlobalBounds().contains(x, y) && m_rightArrowVisibile)
                {
                    m_leftArrowVisibile = true;

                    for (unsigned int i = 0; i < m_tabs.size(); i++)
                    {
                        m_tabs[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x - m_tabs[i].getSize().x - 2, m_tabs[i].getPosition().y});
                        m_iconSprites[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x + m_tabs[i].getSize().x - 15, m_tabs[i].getPosition().y + 7});
                        m_items[i]->setPosition(sf::Vector2f{m_tabs[i].getPosition().x + 7, m_tabs[i].getPosition().y + 7});
                    }

                    m_tabVisibility[m_firstTabVisible] = false;
                    m_firstTabVisible++;
                    m_lastTabVisible++;
                    m_tabVisibility[m_lastTabVisible] = true;

                    m_absoluteBounds = sf::FloatRect{m_absoluteBounds.left - m_maxDefaultTabSize.x - 2, m_absoluteBounds.top, m_absoluteBounds.width, m_tabs.back().getSize().y};

                    if (m_lastTabVisible == m_tabs.size() - 1)
                        m_rightArrowVisibile= false;
                }
            }
        }
    }

    void GraphicTab::addTab(const std::string& tabLabel)
    {
        if (m_tabs.size())
        {
            m_tabs.back().setTexture(m_tabActiveTexture);
            deselect();
        }
        else
        {
            m_firstTabVisible = 0;
            m_lastTabVisible = 0;
        }

        m_tabs.push_back(sf::RectangleShape{m_maxDefaultTabSize});
        m_tabs.back().setTexture(m_tabTexture);
        m_tabs.back().setPosition(sf::Vector2f{ m_absoluteBounds.left + m_absoluteBounds.width,
                                                m_absoluteBounds.top});
        m_lastTabVisible = m_tabs.size() - 1;

        m_iconSprites.push_back(sf::Sprite{m_iconTexture});
        m_iconSprites.back().setTextureRect(sf::IntRect{10, 0, 10, 10});
        m_iconSprites.back().setPosition(sf::Vector2f{m_tabs.back().getPosition().x + m_tabs.back().getSize().x - 15,
                                                      m_absoluteBounds.top + 7});

        m_items.push_back(std::make_shared<GraphicListItem>(tabLabel, 10, sf::Color::Black, m_GLItemTexPtr, 1, 5));
        m_items.back()->getTextWidget()->setTextFont(m_font);
        m_items.back()->setPosition(sf::Vector2f{m_tabs.back().getPosition().x + 7, m_absoluteBounds.top + 7});

        m_tabVisibility.push_back(true);

        m_absoluteBounds = sf::FloatRect{m_absoluteBounds.left, m_absoluteBounds.top, m_absoluteBounds.width + m_tabs.back().getSize().x + 2, m_tabs.back().getSize().y};

        select(tabLabel);

        if (m_absoluteBounds.width >= (m_parent->getSize().x - 100))
        {
            for (unsigned int i = 0; i < m_tabs.size(); i++)
            {
                m_tabs[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x - m_tabs[i].getSize().x - 2, m_tabs[i].getPosition().y});
                m_iconSprites[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x + m_tabs[i].getSize().x - 15, m_tabs[i].getPosition().y + 7});
                m_items[i]->setPosition(sf::Vector2f{m_tabs[i].getPosition().x + 7, m_tabs[i].getPosition().y + 7});
            }

            m_tabVisibility[m_firstTabVisible] = false;
            m_firstTabVisible++;

            m_absoluteBounds = sf::FloatRect{m_absoluteBounds.left - m_maxDefaultTabSize.x - 2, m_absoluteBounds.top, m_absoluteBounds.width, m_tabs.back().getSize().y};

            m_leftArrowVisibile = true;
            m_leftArrow.setPosition(sf::Vector2f{m_tabs[m_firstTabVisible].getPosition().x - 27, m_tabs[m_firstTabVisible].getPosition().y});

            m_rightArrowVisibile = false;
            m_rightArrow.setPosition(sf::Vector2f{m_tabs[m_lastTabVisible].getPosition().x + m_tabs[m_lastTabVisible].getSize().x + 2, m_tabs[m_lastTabVisible].getPosition().y});
        }
    }

    void GraphicTab::focusTab(const unsigned int& tabIndex)
    {
        if (tabIndex == getSelectedTabIndex())
            return;

        if (tabIndex < getSelectedTabIndex())
        {
            select(m_items[tabIndex]->getTextWidget()->getText());

            while (!m_tabVisibility[tabIndex])
            {
                for (unsigned int i = 0; i < m_tabs.size(); i++)
                {
                    m_tabs[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x + m_tabs[i].getSize().x + 2, m_tabs[i].getPosition().y});
                    m_iconSprites[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x + m_tabs[i].getSize().x - 15, m_tabs[i].getPosition().y + 7});
                    m_items[i]->setPosition(sf::Vector2f{m_tabs[i].getPosition().x + 7, m_tabs[i].getPosition().y + 7});
                }

                m_firstTabVisible--;
                m_tabVisibility[m_firstTabVisible] = true;
                m_tabVisibility[m_lastTabVisible] = false;
                m_lastTabVisible--;

                m_absoluteBounds = sf::FloatRect{m_absoluteBounds.left + m_maxDefaultTabSize.x + 2, m_absoluteBounds.top, m_absoluteBounds.width, m_tabs.back().getSize().y};
            }
        }

        if (tabIndex > getSelectedTabIndex())
        {
            select(m_items[tabIndex]->getTextWidget()->getText());

            while (!m_tabVisibility[tabIndex])
            {
                for (unsigned i = 0; i < m_tabs.size(); i++)
                {
                    m_tabs[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x - m_tabs[i].getSize().x - 2, m_tabs[i].getPosition().y});
                    m_iconSprites[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x + m_tabs[i].getSize().x - 15, m_tabs[i].getPosition().y + 7});
                    m_items[i]->setPosition(sf::Vector2f{m_tabs[i].getPosition().x + 7, m_tabs[i].getPosition().y + 7});
                }

                m_tabVisibility[m_firstTabVisible] = false;
                m_firstTabVisible++;
                m_lastTabVisible++;
                m_tabVisibility[m_lastTabVisible] = true;

                m_absoluteBounds = sf::FloatRect{m_absoluteBounds.left - m_maxDefaultTabSize.x - 2, m_absoluteBounds.top, m_absoluteBounds.width, m_tabs.back().getSize().y};
            }
        }
    }

    void GraphicTab::focusTab(const std::string& tabLabel)
    {
        int tabIndex;

        for (unsigned int i = 0; i < m_tabs.size(); i++)
            if (m_items[i]->getTextWidget()->getText() == tabLabel)
                tabIndex = i;

        if (tabIndex == getSelectedTabIndex())
            return;

        if (tabIndex < getSelectedTabIndex())
        {
            select(m_items[tabIndex]->getTextWidget()->getText());

            while (!m_tabVisibility[tabIndex])
            {
                for (unsigned int i = 0; i < m_tabs.size(); i++)
                {
                    m_tabs[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x + m_tabs[i].getSize().x + 2, m_tabs[i].getPosition().y});
                    m_iconSprites[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x + m_tabs[i].getSize().x - 15, m_tabs[i].getPosition().y + 7});
                    m_items[i]->setPosition(sf::Vector2f{m_tabs[i].getPosition().x + 7, m_tabs[i].getPosition().y + 7});
                }

                m_firstTabVisible--;
                m_tabVisibility[m_firstTabVisible] = true;
                m_tabVisibility[m_lastTabVisible] = false;
                m_lastTabVisible--;

                m_absoluteBounds = sf::FloatRect{m_absoluteBounds.left + m_maxDefaultTabSize.x + 2, m_absoluteBounds.top, m_absoluteBounds.width, m_tabs.back().getSize().y};
            }
        }

        if (tabIndex > getSelectedTabIndex())
        {
            select(m_items[tabIndex]->getTextWidget()->getText());

            while (!m_tabVisibility[tabIndex])
            {
                for (unsigned i = 0; i < m_tabs.size(); i++)
                {
                    m_tabs[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x - m_tabs[i].getSize().x - 2, m_tabs[i].getPosition().y});
                    m_iconSprites[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x + m_tabs[i].getSize().x - 15, m_tabs[i].getPosition().y + 7});
                    m_items[i]->setPosition(sf::Vector2f{m_tabs[i].getPosition().x + 7, m_tabs[i].getPosition().y + 7});
                }

                m_tabVisibility[m_firstTabVisible] = false;
                m_firstTabVisible++;
                m_lastTabVisible++;
                m_tabVisibility[m_lastTabVisible] = true;

                m_absoluteBounds = sf::FloatRect{m_absoluteBounds.left - m_maxDefaultTabSize.x - 2, m_absoluteBounds.top, m_absoluteBounds.width, m_tabs.back().getSize().y};
            }
        }
    }

    void GraphicTab::removeTab(unsigned int index)
    {
        for (unsigned int i = 0; i < m_tabs.size(); i++)
        {
            if (i == index)
            {
                if (index > 0)
                {
                    select(m_items[index - 1]->getTextWidget()->getText());
                }

                m_absoluteBounds = sf::FloatRect{m_absoluteBounds.left, m_absoluteBounds.top, m_absoluteBounds.width - (m_tabs.back().getSize().x + 2), m_tabs.back().getSize().y};
                m_items.erase(m_items.begin() + i);
                m_iconSprites.erase(m_iconSprites.begin() + i);
                m_tabs.erase(m_tabs.begin() + i);
                m_tabVisibility.erase(m_tabVisibility.begin() + i);

                for (auto j = i; j < m_tabs.size(); j++)
                {
                    m_items[j]->setPosition(sf::Vector2f{m_items[j]->getPosition().x - m_maxDefaultTabSize.x - 2, m_items[j]->getPosition().y});
                    m_iconSprites[j].setPosition(sf::Vector2f{m_iconSprites[j].getPosition().x - m_maxDefaultTabSize.x - 2, m_iconSprites[j].getPosition().y});
                    m_tabs[j].setPosition(sf::Vector2f{m_tabs[j].getPosition().x - m_maxDefaultTabSize.x - 2, m_tabs[j].getPosition().y});

                    if ((m_parent->getSize().x - m_tabs[j].getPosition().x) > m_maxDefaultTabSize.x + 100)
                        m_tabVisibility[j] = true;
                }

                if ((((m_parent->getSize().x - (m_tabs.back().getPosition().x + m_tabs.back().getSize().x)) > m_maxDefaultTabSize.x + 50)) && (m_tabs[0].getPosition().x < 50) )
                {
                    for (unsigned int i = 0; i < m_tabs.size(); i++)
                    {
                        m_tabs[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x + m_tabs[i].getSize().x + 2, m_tabs[i].getPosition().y});
                        m_iconSprites[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x + m_tabs[i].getSize().x - 15, m_tabs[i].getPosition().y + 7});
                        m_items[i]->setPosition(sf::Vector2f{m_tabs[i].getPosition().x + 7, m_tabs[i].getPosition().y + 7});
                    }

                    m_absoluteBounds = sf::FloatRect{m_absoluteBounds.left + m_maxDefaultTabSize.x + 2, m_absoluteBounds.top, m_absoluteBounds.width, m_tabs.back().getSize().y};
                    m_firstTabVisible--;
                    m_tabVisibility[m_firstTabVisible] = true;
                }

                break;
            }
        }
    }

    void GraphicTab::setPosition(const tgui::Layout& position)
    {
        sf::Vector2f p = position.getValue();

        m_absoluteBounds.left = p.x;
        m_absoluteBounds.top = p.y;

        for (unsigned int i = 0; i < m_tabs.size(); i++)
        {
            m_tabs[i].setPosition(sf::Vector2f{ m_absoluteBounds.left, m_absoluteBounds.top});
            m_iconSprites[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x + m_tabs[i].getSize().x - 15, m_tabs[i].getPosition().y + 7});
            m_items[i]->setPosition(sf::Vector2f{m_tabs[i].getPosition().x + 7, m_tabs[i].getPosition().y + 7});
        }
    }

    sf::Vector2f GraphicTab::getPosition()
    {
        return sf::Vector2f{m_absoluteBounds.left, m_absoluteBounds.top};
    }

    void GraphicTab::setSize(const sf::Vector2f& size)
    {
//        m_absoluteBounds.width = size.x;
//        m_absoluteBounds.height = size.y;
//
//        for (auto& i : m_tabs)
//            i.setPosition(sf::Vector2f{ m_absoluteBounds.left + m_absoluteBounds.width,
//                                        m_absoluteBounds.top + m_absoluteBounds.height
//                                      }
//                         );
//
//        for (auto& i : m_iconSprites)
//            ;
//
//        for (auto& i : m_items)
//            ;
    }

    sf::Vector2f GraphicTab::getSize()
    {
        return sf::Vector2f{m_absoluteBounds.width, m_absoluteBounds.height};
    }

    void GraphicTab::select(const std::string& tabLabel)
    {
        for (unsigned int i = 0; i < m_tabs.size(); i++)
        {
            if (m_items[i]->getTextWidget()->getText() == m_selected)
            {
                deselect();
            }
        }

        for (unsigned int i = 0; i < m_tabs.size(); i++)
        {
            if (m_items[i]->getTextWidget()->getText() == tabLabel)
            {
                m_tabs[i].setSize(sf::Vector2f{m_tabs[i].getSize().x, m_maxDefaultTabSize.y + 5});
                m_tabs[i].setTexture(m_tabTexture);
                m_selected = tabLabel;
            }
        }
    }

    unsigned int GraphicTab::getSelectedTabIndex()
    {
        for (unsigned int i = 0; i < m_tabs.size(); i++)
        {
            if (m_items[i]->getTextWidget()->getText() == m_selected)
                return i;
        }

        return -1;
    }

    void GraphicTab::deselect()
    {
        for (unsigned int i = 0; i < m_tabs.size(); i++)
        {
            if (m_items[i]->getTextWidget()->getText() == m_selected)
            {
                m_tabs[i].setSize(sf::Vector2f{m_tabs[i].getSize().x, m_maxDefaultTabSize.y});
                m_selected = "";
            }
        }
    }

    void GraphicTab::update()
    {
        if (m_tabs.size())
        {
            while ((m_firstTabVisible && (m_lastTabVisible == m_tabs.size() - 1)) && ((m_parent->getSize().x - (m_tabs.back().getPosition().x + m_tabs.back().getSize().x)) > m_maxDefaultTabSize.x + 50))
            {
                for (unsigned int i = 0; i < m_tabs.size(); i++)
                {
                    m_tabs[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x + m_tabs[i].getSize().x + 2, m_tabs[i].getPosition().y});
                    m_iconSprites[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x + m_tabs[i].getSize().x - 15, m_tabs[i].getPosition().y + 7});
                    m_items[i]->setPosition(sf::Vector2f{m_tabs[i].getPosition().x + 7, m_tabs[i].getPosition().y + 7});
                }

                m_absoluteBounds = sf::FloatRect{m_absoluteBounds.left + m_maxDefaultTabSize.x + 2, m_absoluteBounds.top, m_absoluteBounds.width, m_tabs.back().getSize().y};
                m_firstTabVisible--;
                m_tabVisibility[m_firstTabVisible] = true;
            }

            while ((m_lastTabVisible == m_tabs.size() - 1) && (((m_tabs.back().getPosition().x + m_tabs.back().getSize().x) > m_parent->getSize().x - 50)))
            {
                for (unsigned int i = 0; i < m_tabs.size(); i++)
                {
                    m_tabs[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x - m_tabs[i].getSize().x - 2, m_tabs[i].getPosition().y});
                    m_iconSprites[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x + m_tabs[i].getSize().x - 15, m_tabs[i].getPosition().y + 7});
                    m_items[i]->setPosition(sf::Vector2f{m_tabs[i].getPosition().x + 7, m_tabs[i].getPosition().y + 7});
                }

                m_absoluteBounds = sf::FloatRect{m_absoluteBounds.left - m_maxDefaultTabSize.x - 2, m_absoluteBounds.top, m_absoluteBounds.width, m_tabs.back().getSize().y};
                m_tabVisibility[m_firstTabVisible] = false;
                m_firstTabVisible++;
            }

            while (!m_firstTabVisible && ((m_parent->getSize().x - (m_tabs[m_lastTabVisible].getPosition().x + m_tabs[m_lastTabVisible].getSize().x)) > m_maxDefaultTabSize.x + 50))
            {
                m_lastTabVisible++;
                m_tabVisibility[m_lastTabVisible] = true;
            }

            while (!m_firstTabVisible && (((m_tabs[m_lastTabVisible].getPosition().x + m_tabs[m_lastTabVisible].getSize().x) > m_parent->getSize().x - 50)))
            {
                m_tabVisibility[m_lastTabVisible] = false;
                m_lastTabVisible--;
            }

            if (m_firstTabVisible && (m_lastTabVisible != m_tabs.size() - 1))
            {
                while ((m_parent->getSize().x - (m_tabs[m_lastTabVisible].getPosition().x + m_tabs[m_lastTabVisible].getSize().x)) > m_maxDefaultTabSize.x + 50)
                {
                    m_lastTabVisible++;
                    m_tabVisibility[m_lastTabVisible] = true;

                    if (m_lastTabVisible == m_tabs.size() - 1 && ((m_parent->getSize().x - (m_tabs[m_lastTabVisible].getPosition().x + m_tabs[m_lastTabVisible].getSize().x)) > m_maxDefaultTabSize.x + 50))
                    {
                        for (unsigned int i = 0; i < m_tabs.size(); i++)
                        {
                            m_tabs[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x + m_tabs[i].getSize().x + 2, m_tabs[i].getPosition().y});
                            m_iconSprites[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x + m_tabs[i].getSize().x - 15, m_tabs[i].getPosition().y + 7});
                            m_items[i]->setPosition(sf::Vector2f{m_tabs[i].getPosition().x + 7, m_tabs[i].getPosition().y + 7});
                        }

                        m_absoluteBounds = sf::FloatRect{m_absoluteBounds.left + m_maxDefaultTabSize.x + 2, m_absoluteBounds.top, m_absoluteBounds.width, m_tabs.back().getSize().y};
                        m_firstTabVisible--;
                        m_tabVisibility[m_firstTabVisible] = true;
                    }
                }

                while ((m_parent->getSize().x - (m_tabs[m_lastTabVisible].getPosition().x + m_tabs[m_lastTabVisible].getSize().x)) > m_maxDefaultTabSize.x + 50)
                {
                    m_firstTabVisible--;
                    m_tabVisibility[m_firstTabVisible] = true;

                    if (m_firstTabVisible == 0 && ((m_parent->getSize().x - (m_tabs[m_lastTabVisible].getPosition().x + m_tabs[m_lastTabVisible].getSize().x)) > m_maxDefaultTabSize.x + 50))
                    {
                        for (unsigned int i = 0; i < m_tabs.size(); i++)
                        {
                            m_tabs[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x - m_tabs[i].getSize().x - 2, m_tabs[i].getPosition().y});
                            m_iconSprites[i].setPosition(sf::Vector2f{m_tabs[i].getPosition().x + m_tabs[i].getSize().x - 15, m_tabs[i].getPosition().y + 7});
                            m_items[i]->setPosition(sf::Vector2f{m_tabs[i].getPosition().x + 7, m_tabs[i].getPosition().y + 7});
                        }

                        m_absoluteBounds = sf::FloatRect{m_absoluteBounds.left - m_maxDefaultTabSize.x - 2, m_absoluteBounds.top, m_absoluteBounds.width, m_tabs.back().getSize().y};
                        m_lastTabVisible--;
                        m_tabVisibility[m_lastTabVisible] = true;
                    }
                }
            }

            m_leftArrow.setPosition(sf::Vector2f{m_tabs[m_firstTabVisible].getPosition().x - 27, m_tabs[m_firstTabVisible].getPosition().y});
            m_rightArrow.setPosition(sf::Vector2f{m_tabs[m_lastTabVisible].getPosition().x + m_tabs[m_lastTabVisible].getSize().x + 2, m_tabs[m_lastTabVisible].getPosition().y});
        }
    }

    unsigned int GraphicTab::getTabCount()
    {
        return m_tabs.size();
    }

    bool GraphicTab::isTabPresent(const std::string& tabLabel)
    {
        for (unsigned int i = 0; i < m_tabs.size(); i++)
            if (m_items[i]->getTextWidget()->getText() == tabLabel)
                return true;

        return false;
    }
}
