#include "../include/Menu.hpp"

namespace prattle
{
    void Menu::initialize(tgui::Container *const parent)
    {
        Widget::initialize(parent);

        if (parent->getGlobalFont())
        {
            m_font = parent->getGlobalFont();
            add(m_friendList);
            add(m_searchPanel);
        }
    }

    Menu::Menu() : m_friendList{std::make_shared<GraphicList>()},
                   m_searchField{std::make_shared<tgui::EditBox>()},
                   m_resultMessage{std::make_shared<tgui::Label>()},
                   m_results{std::make_shared<tgui::ListBox>()}
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

        m_friendList->setPosition(tgui::Layout{m_bounds.left + 5, m_menuItemSprites[0].getPosition().y + m_menuItemSprites[0].getGlobalBounds().height + 5});
        m_friendList->hide();

        m_searchField->setDefaultText("Search");
        m_searchField->setSize(200, 20);
        m_searchField->setPosition(20, 20);

        m_searchButton = tgui::Button::create("resources/widgets/Black.conf");//, "search_button");
        m_searchButton->setText("Search");
        m_searchButton->setSize(200, 20);
        m_searchButton->setPosition(m_searchField->getPosition().x, m_searchField->getPosition().y + m_searchField->getSize().y + 7);

        m_resultMessage->setText("Results found:");
        m_resultMessage->setSize(100, 12);
        m_resultMessage->setTextSize(12);
        m_resultMessage->setPosition(m_searchButton->getPosition().x, m_searchButton->getPosition().y + m_searchButton->getSize().y + 30);
        m_resultMessage->hide();

        m_results->setSize(200, 200);
        m_results->setPosition(m_resultMessage->getPosition().x, m_resultMessage->getPosition().y + m_resultMessage->getSize().y + 10);
        m_results->hide();

        m_addFriendButton = tgui::Button::create("resources/widgets/Black.conf");//, "search_button");
        m_addFriendButton->setText("Add");
        m_addFriendButton->setSize(200, 20);
        m_addFriendButton->setPosition(m_results->getPosition().x, m_results->getPosition().y + m_results->getSize().y + 10);
        m_addFriendButton->hide();

        m_searchPanel = tgui::Panel::create(sf::Vector2f{m_searchField->getSize().x + 2 * 20, m_searchField->getSize().y + m_searchButton->getSize().y + 7 + 2 * 20});
        m_searchPanel->setBackgroundColor(sf::Color{255, 224, 194});
        m_searchPanel->setPosition(m_menuItemSprites[1].getPosition().x, m_menuItemSprites[1].getPosition().y + m_menuItemSprites[1].getGlobalBounds().height + 5);
        m_searchPanel->hide();
        m_searchPanel->add(m_searchField);
        m_searchPanel->add(m_searchButton, "search_button");
        m_searchPanel->add(m_resultMessage);
        m_searchPanel->add(m_results);
        m_searchPanel->add(m_addFriendButton, "add_friend_button");
    }

    void Menu::initList(const std::vector<std::string>& friends)
    {
        //m_friendList->setPosition(tgui::Layout{m_bounds.left + 5, m_bounds.height + 50 + 5});

        for (auto& i : friends)
            m_friendList->addItem(i);

        m_friendList->hide();
    }

    void Menu::resetList()
    {
        m_friendList->clear();
    }

    std::string Menu::getSelectedFriend()
    {
        return m_friendList->getSelected();
    }

    unsigned int Menu::getStatus(const std::string& friendName)
    {
        return m_friendList->getStatusOfItem(friendName);
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

        if (m_searchPanel->isVisible())
            target.draw(*m_searchPanel, states);
    }

    tgui::Widget::Ptr Menu::clone()
    {
        return std::make_shared<Menu>(* this);
    }

    bool Menu::mouseOnWidget(float x, float y)
    {
        if (m_bounds.contains(x,y) || (m_friendList->mouseOnWidget(x, y) && m_friendList->isVisible()))
            return true;
        else if (m_bounds.contains(x,y) || (m_searchPanel->mouseOnWidget(x, y) && m_searchPanel->isVisible()))
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
            if (m_searchPanel->isVisible())
            {
                if (m_searchField->mouseOnWidget(x, y))
                {
                    m_searchField->mouseMoved(x, y);
                }

                if (m_searchButton->mouseOnWidget(x, y))
                {
                    m_searchButton->mouseMoved(x, y);
                }

                if (m_results->mouseOnWidget(x, y))
                {
                    m_results->mouseMoved(x, y);
                }

                if (m_addFriendButton->mouseOnWidget(x, y))
                {
                    m_addFriendButton->mouseMoved(x, y);
                }

                m_searchPanel->mouseMoved(x, y);
            }

            for (unsigned int i = 0; i < m_menuItemSprites.size(); i++)
            {
                if (m_menuItemSprites[i].getGlobalBounds().contains(x, y))
                {
                    m_menuItemSprites[i].setTextureRect(sf::IntRect{60 * (int)i, 60, 60, 60});
                }
                else
                    m_menuItemSprites[i].setTextureRect(sf::IntRect{60 * (int)i, 0, 60, 60});
            }

            if (m_friendList->isVisible() && m_friendList->mouseOnWidget(x, y))
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
        if (mouseOnWidget(x, y))
        {
            if (m_friendList->isVisible())
            {
                m_friendList->leftMousePressed(x, y);
                if (m_menuItemSprites[0].getGlobalBounds().contains(x, y))
                    m_friendList->hide();
            }
            else
                if (m_menuItemSprites[0].getGlobalBounds().contains(x, y))
                    m_friendList->show();

            if (m_searchPanel->isVisible())
            {
                m_searchPanel->leftMousePressed(x, y);

                if (m_menuItemSprites[1].getGlobalBounds().contains(x, y))
                    m_searchPanel->hide();
            }
            else
                if (m_menuItemSprites[1].getGlobalBounds().contains(x, y))
                {
                    m_searchPanel->show();
                }
        }
    }

    void Menu::textEntered(sf::Uint32 key)
    {
        m_searchField->textEntered(key);
    }

    void Menu::keyPressed(const sf::Event::KeyEvent& event)
    {
        m_searchField->keyPressed(event);
    }

    void Menu::setPosition(const tgui::Layout& position)
    {
        sf::Vector2f p = position.getValue();

        m_bounds.left = p.x;
        m_bounds.top = p.y;

        m_menuItemSprites[0].setPosition(sf::Vector2f{p.x, p.y});
        m_menuItemSprites[1].setPosition(sf::Vector2f{m_menuItemSprites[0].getPosition().x + m_menuItemSprites[0].getGlobalBounds().width + m_spacing, m_menuItemSprites[0].getPosition().y});
        m_menuItemSprites[2].setPosition(sf::Vector2f{m_menuItemSprites[1].getPosition().x + m_menuItemSprites[1].getGlobalBounds().width + m_spacing, m_menuItemSprites[1].getPosition().y});
        m_menuItemSprites[3].setPosition(sf::Vector2f{m_menuItemSprites[2].getPosition().x + m_menuItemSprites[2].getGlobalBounds().width + m_spacing, m_menuItemSprites[2].getPosition().y});

        //std::cout << "Pos : " << m_menuItemSprites[0].getPosition().y + m_menuItemSprites[0].getGlobalBounds().height + 5 << std::endl;
        m_friendList->setPosition(tgui::Layout{m_menuItemSprites[0].getPosition().x, m_menuItemSprites[0].getPosition().y + m_menuItemSprites[0].getGlobalBounds().height + 5});
        m_searchPanel->setPosition(m_menuItemSprites[1].getPosition().x, m_menuItemSprites[1].getPosition().y + m_menuItemSprites[1].getGlobalBounds().height + 5);
        m_searchButton->setPosition(m_searchField->getPosition().x, m_searchField->getPosition().y + m_searchField->getSize().y + 7);
        m_resultMessage->setPosition(m_searchButton->getPosition().x, m_searchButton->getPosition().y + m_searchButton->getSize().y + 30);
        m_results->setPosition(m_resultMessage->getPosition().x, m_resultMessage->getPosition().y + m_resultMessage->getSize().y + 10);

//        std::cout << "pos1 : " << m_menuItemSprites[0].getPosition().y + m_menuItemSprites[0].getGlobalBounds().height + 5 << std::endl;
//        std::cout << "pos2 : " << m_friendList->getPosition().y << std::endl;
//
//        std::cout << "pos3 : " << m_searchPanel->getPosition().y << std::endl;
    }

    sf::Vector2f Menu::getPosition()
    {
        return sf::Vector2f{m_bounds.left, m_bounds.top};
    }

    sf::Vector2f Menu::getSize()
    {
        return sf::Vector2f{m_bounds.width, m_bounds.height};
    }

    GraphicList::Ptr Menu::getFriendlist()
    {
        return m_friendList;
    }

    tgui::Widget::Ptr Menu::getMenuItem(Item itemType)
    {
        switch (itemType)
        {
            case Item::FriendPanel:
                return m_friendList;

            case Item::SearchPanel:
                return m_searchPanel;

            case Item::SettingsPanel:;
            case Item::AboutPanel:;
        }
    }

    sf::FloatRect Menu::getItemBounds(const unsigned int& itemIndex)
    {
        return m_menuItemSprites[itemIndex].getGlobalBounds();
    }

    std::string Menu::getSearchFieldText()
    {
        return m_searchField->getText().toAnsiString();
    }

    void Menu::showSearchResults(const std::vector<std::string>& results)
    {
        if (results.size() > 0)
        {
            for (auto& i : results)
                m_results->addItem(i);

            m_searchPanel->setSize(m_searchPanel->getSize().x, m_searchPanel->getSize().y +
                                                                m_resultMessage->getSize().y +
                                                                 m_results->getSize().y +
                                                                  m_addFriendButton->getSize().y +
                                                                  + 10 + 10 + 10 + 20);
            m_resultMessage->show();
            m_results->show();
            m_addFriendButton->show();
        }
    }

    void Menu::closeSearchPanel()
    {
    }

    std::string Menu::getSelectedResult()
    {
        return m_results->getSelectedItem().toAnsiString();
    }

    void Menu::setNotif(const std::string& friendLabel, const std::string& notif)
    {
        m_friendList->setNotifOfItem(friendLabel, notif);
    }

    std::string Menu::getNotif(const std::string& friendLabel)
    {
        m_friendList->getNotifOfItem(friendLabel);
    }
}
