#ifndef AUTHENTICATION_MANAGER_H
#define AUTHENTICATION_MANAGER_H

#include <SFML/Graphics.hpp>
#include <string>
#include "UserManager.h"

enum class AuthState {
    LOGIN,
    REGISTER,
    AUTHENTICATED
};

enum class InputField {
    NONE,
    USERNAME,
    PASSWORD,
    EMAIL
};

class AuthenticationManager {
private:
    sf::RenderWindow& window;
    UserManager& userManager;
    sf::Font font;
    
    AuthState currentState;
    InputField activeField;
    
    // Input strings
    std::string username;
    std::string password;
    std::string email;
    
    // UI Elements
    sf::RectangleShape backgroundRect;
    sf::Text titleText;
    sf::Text errorText;
    sf::Text successText;
    
    // Input fields
    sf::RectangleShape usernameRect;
    sf::Text usernameLabel;
    sf::Text usernameDisplay;
    
    sf::RectangleShape passwordRect;
    sf::Text passwordLabel;
    sf::Text passwordDisplay;
    
    sf::RectangleShape emailRect;
    sf::Text emailLabel;
    sf::Text emailDisplay;
    
    // Buttons
    sf::RectangleShape loginButton;
    sf::Text loginButtonText;
    
    sf::RectangleShape registerButton;
    sf::Text registerButtonText;
    
    sf::RectangleShape switchButton;
    sf::Text switchButtonText;
    
    // Colors
    sf::Color backgroundColor;
    sf::Color inputColor;
    sf::Color activeInputColor;
    sf::Color buttonColor;
    sf::Color buttonHoverColor;
    sf::Color textColor;
    sf::Color errorColor;
    sf::Color successColor;
    
    // State
    bool isMouseOverButton(const sf::RectangleShape& button, const sf::Vector2i& mousePos);
    void updateInputFieldColors();
    void setupUI();
    void handleLogin();
    void handleRegister();
    void switchState();
    std::string maskPassword(const std::string& password);
    
public:
    AuthenticationManager(sf::RenderWindow& window, UserManager& userManager);
    
    bool initialize();
    void handleEvent(const sf::Event& event);
    void update();
    void draw();
    
    bool isAuthenticated() const { return currentState == AuthState::AUTHENTICATED; }
    AuthState getState() const { return currentState; }
    void reset();
};

#endif // AUTHENTICATION_MANAGER_H