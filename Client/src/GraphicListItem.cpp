#include "../include/GraphicListItem.hpp"

namespace prattle
{
    // The default constructor just does some very minimal creation and
    // doesn't create a graphic sprite at all. The graphic sprite has
    // to be created using a call to the loadTexture() method.
    GraphicListItem::GraphicListItem()
    {
        m_status = 0;
        m_spacing = 5;

        m_itemText = tgui::Label::create();
        m_itemText->setAutoSize(true);
        m_itemText->setText("GraphicListItem");
        m_itemText->setTextSize(10);
        m_itemText->setTextColor(sf::Color::White);

        m_bounds = sf::FloatRect{ m_itemSprite.getGlobalBounds().left,
                                  m_itemSprite.getGlobalBounds().top,
                                  m_itemSprite.getGlobalBounds().width +
                                   m_spacing + m_itemText->getSize().x,
                                  (float)m_itemText->getTextSize()
                                };
    }

    GraphicListItem::GraphicListItem(const std::string& label,
                    const unsigned int& labelSize,
                    const sf::Color& labelColor,
                    std::shared_ptr<sf::Texture> itemTexture ,
                    const unsigned short& status,
                    const float& spacing
                   )
    {
        m_status = status;
        m_spacing = spacing;

        m_itemText = tgui::Label::create();
        m_itemText->setAutoSize(true);
        m_itemText->setText(label);
        m_itemText->setTextSize(labelSize);
        m_itemText->setTextColor(labelColor);

        if (itemTexture == nullptr)
            throw std::runtime_error("ERROR :: Passed texture is a NULL pointer!");

        m_itemTexture = itemTexture;
        m_itemSprite.setTexture(*m_itemTexture);
        m_itemSprite.setTextureRect(sf::IntRect{10 * m_status, 0, 10, 10});
    }

    // This function is declared in tgui::Widget and will fetch the
    // global font currently used by tgui::Gui object.

    void GraphicListItem::initialize(tgui::Container *const parent)
    {
        Widget::initialize(parent);

        if (parent->getGlobalFont())
        {
            m_itemText->setTextFont(parent->getGlobalFont());
        }
    }

    // The draw(), mouseOnWidget() and clone() methods are declared
    // in tgui::Widget class, and these have to be overridden in
    // GraphicListItem.

    void GraphicListItem::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(m_itemSprite, states);
        target.draw(*m_itemText, states);
    }

    bool GraphicListItem::mouseOnWidget(float x, float y)
    {
        return m_bounds.contains(x,y);
    }

    tgui::Widget::Ptr GraphicListItem::clone()
    {
        return std::make_shared<GraphicListItem>(* this);
    }

    void GraphicListItem::setPosition(const tgui::Layout& position)
    {
        // The position is a Layout object is passed as the
        // parameter to be enable to use TGUI's excellent
        // relative position setting capabilities. See the
        // TGUI documentation entry for Layout for more details.

        auto p = position.getValue();

        // The new boundary is the x-coord of new position,
        // the y-coord of new position, the width of the
        // GraphicListItem object (width of sprite + width
        // of label + spacing between them), and the max.
        // height of the label or graphic, whichever's greater.

        m_bounds = sf::FloatRect{ p.x,
                                  p.y,
                                  m_itemSprite.getGlobalBounds().width +
                                   m_itemText->getSize().x + m_spacing,
                                  m_itemText->getSize().y > m_itemSprite.getGlobalBounds().height ? m_itemText->getSize().y : m_itemSprite.getGlobalBounds().height
                                };

        m_itemSprite.setPosition(p);
        m_itemText->setPosition(p.x + m_itemSprite.getGlobalBounds().width + m_spacing, p.y);
    }

    const sf::Vector2f GraphicListItem::getPosition()
    {
        return sf::Vector2f{m_bounds.left, m_bounds.top};
    }

    void GraphicListItem::setStatus(unsigned short status)
    {
        m_status = status;
        m_itemSprite.setTextureRect(sf::IntRect{10 * m_status, 0, 10, 10});
    }

    tgui::Label::Ptr GraphicListItem::getTextWidget()
    {
        return m_itemText;
    }

    const sf::FloatRect GraphicListItem::getBounds()
    {
        return m_bounds;
    }

    void GraphicListItem::loadTexture(const std::string& file)
    {
        if (!m_itemTexture->loadFromFile(file))
            throw std::runtime_error("ERROR :: Unable to open texture file \'" + file + "\' for GraphicListItem object.");

        m_itemSprite.setTexture(*m_itemTexture);
        m_itemSprite.setTextureRect(sf::IntRect{10 * m_status, 0, 10, 10});
    }
}