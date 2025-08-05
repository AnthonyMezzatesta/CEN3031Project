#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Utilities.h"
#include "Subject.h"
using std::cout;
using std::endl;

struct  WindowResizeHandler final : public EventSystem::WindowResizeSubject
{
    enum WindowType { Window, Fullscreen };
    void ResizeWindowOnSelection(sf::RenderWindow& window, sf::VideoMode& mode, WindowType type = WindowType::Window)
    {
        // on Windows OS, can't have a SFML window that's larger than screen target
        if (!mode.isValid())
        {
            std::cerr << "view mode is not valid: (" << mode.width << " x " << mode.height << ")" << endl;
            return;
        }

        // update the view to the new size of the window
        int style = type == WindowType::Window ? Utilities::windowStyle : sf::Style::Fullscreen;
        window.create(mode, Utilities::windowTitle, style);

        // update other views
        Notify(EventSystem::Observer::Update, window.getView());
    }

    /**
     * currently not in-use as, even with the following,
     * the user can still resize the window such that the height is 0 pixels
     * which is a problem as we can't create render textures elsewhere that are 0 pixels in any dimension
     */
    // void ResizeWindowOnEvent(sf::RenderWindow& window, sf::Event::SizeEvent& size)
    // {
    //     // 'modes' is sorted from best to worst resolution, so worst resolution is the last one
    //     const auto& modes = sf::VideoMode::getFullscreenModes();
    //     const auto& minMode = modes[modes.size() - 1];
    //
    //     // clamping size to prevent window from becoming too small
    //     // e.g. this is done since trying to create a render texture with a width/height of 0 throws an error
    //     unsigned int width = std::max(size.width, minMode.width);
    //     unsigned int height = std::max(size.height, minMode.height);
    //
    //     // update the view to the new size of the window
    //     window.setView(sf::View(sf::FloatRect(0.f, 0.f, width, height)));
    //
    //     // update other views
    //     Notify(EventSystem::Observer::Update, window.getView());
    // }
};