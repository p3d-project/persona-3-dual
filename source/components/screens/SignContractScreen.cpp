#include "SignContractScreen.hpp"
#include "core/globals.hpp"

SignContractScreen* SignContractScreen::instance = nullptr;

void SignContractScreen::create()
{
    if (!instance)
    {
        instance = new SignContractScreen();
    }
}

void SignContractScreen::destroy()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
}

SignContractScreen* SignContractScreen::getInstance()
{
    if (!instance)
    {
        instance = new SignContractScreen();
    }
    return instance;
}

void SignContractScreen::loadSaveDataName()
{
    for (int i = 0; i < 10; ++i)
    {
        lastName[i] = saveData.lastName[i];
        firstName[i] = saveData.firstName[i];
    }
    renderName();
}

void SignContractScreen::loadBackgrounds()
{
    // Load backgrounds into ram
    bgUI[0] = graphics->loadGraphic(bgPath + "base/base");
    bgUI[1] = graphics->loadGraphic(bgPath + "shift/shift");
}

void SignContractScreen::renderSprites()
{
    renderBackground();
    updateStatus("Enter your last name");
}

void SignContractScreen::renderBackground(int bgIndex)
{
    // load palettes
    dmaCopy(bgUI[bgIndex].pal, BG_PALETTE_SUB, bgUI[bgIndex].palLen);

    // draw background (copy into vram)
    dmaFillHalfWords(0, bgGetMapPtr(bgId), 2048);
    dmaCopy(bgUI[bgIndex].tiles, bgGetGfxPtr(bgId), bgUI[bgIndex].tilesLen);
    dmaCopy(bgUI[bgIndex].map, bgGetMapPtr(bgId), bgUI[bgIndex].mapLen);
}

void SignContractScreen::switchBackground()
{
    int index = 0;
    if (isShift || isCapsLock)
    {
        index = 1;
    }
    else
    {
        index = 0;
    }
    renderBackground(index);
}

void SignContractScreen::unloadBackgrounds()
{
    // unload backgrounds from ram
    graphics->unloadGraphic(bgUI[0]);
    graphics->unloadGraphic(bgUI[1]);
    bgUI[0] = {};
    bgUI[1] = {};
}

int SignContractScreen::onTouch(touchPosition* touch)
{
    char c = keyboard.evaluateInput(touch);

    switch (c)
    {
    case KEYCODES::CAPSLOCK:
    {
        isCapsLock = !isCapsLock;
        switchBackground();
        break;
    }
    case KEYCODES::SHIFT:
    {
        isShift = !isShift;
        switchBackground();
        break;
    }
    case KEYCODES::BACKSPACE:
    {
        if (index == 0)
        {
            lastName[0] = ' ';
        }
        else if (isLastName)
        {
            lastName[--index] = ' ';
        }
        else
        {
            if (index != 10)
            {
                firstName[--index - 10] = ' ';
            }
            else
            {
                index--;
                isLastName = true;
                updateStatus("Enter your last name");
                lastName[9] = ' ';
            }
        }
        renderName();
        break;
    }
    case KEYCODES::LEFT:
    {
        if (index > 0)
        {
            index--;
            if (index == 9)
            {
                isLastName = true;
                updateStatus("Enter your last name");
            }
        }
        break;
    }
    case KEYCODES::RIGHT:
    {
        if (index < 19)
        {
            index++;
            if (index == 10)
            {
                isLastName = false;
                updateStatus("Enter your first name");
            }
        }
        break;
    }
    case KEYCODES::CONFIRM:
    {
        return 1; // TODO: implement confirm action
    }
    case 0:
    {
        break; // didnt hit any key
    }
    default:
    {
        if (index < 20)
        {
            writeCharacter(c);
        }
        break;
    }
    }
    return 0;
}

void SignContractScreen::writeCharacter(char c)
{
    if (isShift)
    {
        isShift = false;
        switchBackground();
    }
    if (index < 10)
    {
        lastName[index] = c;
        index++;
        if (index == 10)
        {
            isLastName = false;
            updateStatus("Enter your first name");
        }
    }
    else
    {
        firstName[index - 10] = c;
        if (index != 19)
        {
            index++;
        }
    }
    renderName();
}

void SignContractScreen::updateStatus(std::string status)
{
    text->clearArea(10, 5, 105, FONT_SIZE + lineSpacing);
    text->drawText(status, 10, 5);
}

void SignContractScreen::renderName()
{
    text->clearArea(65, 31, 125, 30);
    for (int i = 0; i < 10; i++)
    {
        if (lastName[i] == '\0' || lastName[i] == ' ')
        {
            continue;
        }
        char c = lastName[i];
        Glyph* g = text->getGlyph(c, true);
        text->drawGlyph(*g, 65 + (13 * i), 31 - lineSpacing, TextColor::White, true);
    }
    for (int i = 0; i < 10; i++)
    {
        if (firstName[i] == '\0' || firstName[i] == ' ')
        {
            continue;
        }
        char c = firstName[i];
        Glyph* g = text->getGlyph(c, true);
        text->drawGlyph(*g, 65 + (13 * i), 48 - lineSpacing, TextColor::White, true);
    }
}

void SignContractScreen::load()
{
    if (signContractUI == nullptr)
    {
        signContractUI = engine.CreateEntity();
        graphics = engine.CreateComponent<GraphicsComponent>();
        signContractUI->AddComponent(graphics);
    }

    text = engine.CreateComponent<TextComponent>();
    signContractUI->AddComponent(text);
    int bgTextSub = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 4, 0);
    uint16_t* textVideoBufferSub = bgGetGfxPtr(bgTextSub);
    bgSetPriority(bgTextSub, 0);
    bgSetPriority(bgId, 1);
    text->configureText(TextConfig{textVideoBufferSub, &FONT_NAME, FONT_SIZE});
    lineSpacing = text->getLineSpacing(); // get it now so we don't waste time at runtime

    loadBackgrounds();
    loadSaveDataName();
}

void SignContractScreen::unload()
{
    // update save data (names)
    for (int i = 0; i < 10; ++i)
    {
        saveData.lastName[i] = lastName[i];
        saveData.firstName[i] = firstName[i];
    }
    ae::BroadcastEvent(Event::WriteSave{});

    unloadBackgrounds();

    if (signContractUI != nullptr)
    {
        engine.DestroyEntity(signContractUI);
        signContractUI = nullptr;
        graphics = nullptr;
    }
}
