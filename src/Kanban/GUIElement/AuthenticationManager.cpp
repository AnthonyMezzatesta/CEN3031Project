#include "AuthenticationManager.h"
#include <iostream>

AuthenticationManager::AuthenticationManager(sf::RenderWindow& window, UserManager& userManager)
    : window(window), userManager(userManager), currentState(AuthState::LOGIN), activeField(InputField::NONE) {
    
    // Initialize colors
    backgroundColor = sf::Color(18, 18, 20);
    inputColor = sf::Color::White;
    activeInputColor = sf::Color(200, 200, 255);
    buttonColor = sf::Color(70, 130, 180);
    buttonHoverColor = sf::Color(100, 150, 200);
    textColor = sf::Color::White;
    errorColor = sf::Color(255, 100, 100);
    successColor = sf::Color(100, 255, 100);
}

bool AuthenticationManager::initialize() {
    if (!font.loadFromFile("resources/aovel-sans-rounded-font/AovelSansRounded-rdDL.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return false;
    }
    
    setupUI();
    return true;
}

void AuthenticationManager::setupUI() {
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f center(windowSize.x / 2.0f, windowSize.y / 2.0f);
    
    // Background
    backgroundRect.setSize(sf::Vector2f(500, 450));
    backgroundRect.setFillColor(backgroundColor);
    backgroundRect.setOutlineColor(sf::Color::White);
    backgroundRect.setOutlineThickness(2);
    backgroundRect.setOrigin(backgroundRect.getSize() / 2.0f);
    backgroundRect.setPosition(center);
    
    // Title
    titleText.setFont(font);
    titleText.setCharacterSize(36);
    titleText.setFillColor(textColor);
    titleText.setString(currentState == AuthState::LOGIN ? "Login" : "Register");
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
    titleText.setPosition(center.x, center.y - 170);
    
    // Username field
    usernameRect.setSize(sf::Vector2f(350, 40));
    usernameRect.setFillColor(inputColor);
    usernameRect.setOutlineColor(sf::Color::Black);
    usernameRect.setOutlineThickness(2);
    usernameRect.setOrigin(usernameRect.getSize() / 2.0f);
    usernameRect.setPosition(center.x, center.y - 100);
    
    usernameLabel.setFont(font);
    usernameLabel.setCharacterSize(24);
    usernameLabel.setFillColor(textColor);
    usernameLabel.setString("Username:");
    usernameLabel.setPosition(center.x - 175, center.y - 140);
    
    usernameDisplay.setFont(font);
    usernameDisplay.setCharacterSize(24);
    usernameDisplay.setFillColor(sf::Color::Black);
    usernameDisplay.setPosition(center.x - 170, center.y - 110);
    
    // Password field
    passwordRect.setSize(sf::Vector2f(350, 40));
    passwordRect.setFillColor(inputColor);
    passwordRect.setOutlineColor(sf::Color::Black);
    passwordRect.setOutlineThickness(2);
    passwordRect.setOrigin(passwordRect.getSize() / 2.0f);
    passwordRect.setPosition(center.x, center.y - 30);
    
    passwordLabel.setFont(font);
    passwordLabel.setCharacterSize(24);
    passwordLabel.setFillColor(textColor);
    passwordLabel.setString("Password:");
    passwordLabel.setPosition(center.x - 175, center.y - 70);
    
    passwordDisplay.setFont(font);
    passwordDisplay.setCharacterSize(24);
    passwordDisplay.setFillColor(sf::Color::Black);
    passwordDisplay.setPosition(center.x - 170, center.y - 40);
    
    // Email field (for register)
    emailRect.setSize(sf::Vector2f(350, 40));
    emailRect.setFillColor(inputColor);
    emailRect.setOutlineColor(sf::Color::Black);
    emailRect.setOutlineThickness(2);
    emailRect.setOrigin(emailRect.getSize() / 2.0f);
    emailRect.setPosition(center.x, center.y + 40);
    
    emailLabel.setFont(font);
    emailLabel.setCharacterSize(24);
    emailLabel.setFillColor(textColor);
    emailLabel.setString("Email:");
    emailLabel.setPosition(center.x - 175, center.y);
    
    emailDisplay.setFont(font);
    emailDisplay.setCharacterSize(24);
    emailDisplay.setFillColor(sf::Color::Black);
    emailDisplay.setPosition(center.x - 170, center.y + 30);
    
    // Login/Register button
    loginButton.setSize(sf::Vector2f(120, 40));
    loginButton.setFillColor(buttonColor);
    loginButton.setOrigin(loginButton.getSize() / 2.0f);
    loginButton.setPosition(center.x - 80, center.y + 120);
    
    loginButtonText.setFont(font);
    loginButtonText.setCharacterSize(20);
    loginButtonText.setFillColor(textColor);
    loginButtonText.setString(currentState == AuthState::LOGIN ? "Login" : "Register");
    sf::FloatRect loginBounds = loginButtonText.getLocalBounds();
    loginButtonText.setOrigin(loginBounds.width / 2.0f, loginBounds.height / 2.0f);
    loginButtonText.setPosition(center.x - 80, center.y + 120);
    
    // Switch button
    switchButton.setSize(sf::Vector2f(120, 40));
    switchButton.setFillColor(buttonColor);
    switchButton.setOrigin(switchButton.getSize() / 2.0f);
    switchButton.setPosition(center.x + 80, center.y + 120);
    
    switchButtonText.setFont(font);
    switchButtonText.setCharacterSize(20);
    switchButtonText.setFillColor(textColor);
    switchButtonText.setString(currentState == AuthState::LOGIN ? "Sign Up" : "Back to Login");
    sf::FloatRect switchBounds = switchButtonText.getLocalBounds();
    switchButtonText.setOrigin(switchBounds.width / 2.0f, switchBounds.height / 2.0f);
    switchButtonText.setPosition(center.x + 80, center.y + 120);
    
    // Error/Success text
    errorText.setFont(font);
    errorText.setCharacterSize(18);
    errorText.setFillColor(errorColor);
    errorText.setPosition(center.x, center.y + 180);
    
    successText.setFont(font);
    successText.setCharacterSize(18);
    successText.setFillColor(successColor);
    successText.setPosition(center.x, center.y + 180);
    
    updateInputFieldColors();
}

bool AuthenticationManager::isMouseOverButton(const sf::RectangleShape& button, const sf::Vector2i& mousePos) {
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
    return button.getGlobalBounds().contains(worldPos);
}

void AuthenticationManager::updateInputFieldColors() {
    usernameRect.setFillColor(activeField == InputField::USERNAME ? activeInputColor : inputColor);
    passwordRect.setFillColor(activeField == InputField::PASSWORD ? activeInputColor : inputColor);
    emailRect.setFillColor(activeField == InputField::EMAIL ? activeInputColor : inputColor);
}

void AuthenticationManager::handleEvent(const sf::Event& event) {
    if (currentState == AuthState::AUTHENTICATED) return;
    
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
        
        // Check input field clicks
        activeField = InputField::NONE;
        if (usernameRect.getGlobalBounds().contains(worldPos)) {
            activeField = InputField::USERNAME;
        } else if (passwordRect.getGlobalBounds().contains(worldPos)) {
            activeField = InputField::PASSWORD;
        } else if (currentState == AuthState::REGISTER && emailRect.getGlobalBounds().contains(worldPos)) {
            activeField = InputField::EMAIL;
        }
        
        // Check button clicks
        if (isMouseOverButton(loginButton, mousePos)) {
            if (currentState == AuthState::LOGIN) {
                handleLogin();
            } else {
                handleRegister();
            }
        } else if (isMouseOverButton(switchButton, mousePos)) {
            switchState();
        }
        
        updateInputFieldColors();
    }
    
    if (event.type == sf::Event::TextEntered && activeField != InputField::NONE) {
        if (event.text.unicode < 128) {
            char c = static_cast<char>(event.text.unicode);
            
            if (c == '\b') { // Backspace
                switch (activeField) {
                    case InputField::USERNAME:
                        if (!username.empty()) username.pop_back();
                        break;
                    case InputField::PASSWORD:
                        if (!password.empty()) password.pop_back();
                        break;
                    case InputField::EMAIL:
                        if (!email.empty()) email.pop_back();
                        break;
                    case InputField::NONE:
                        // Do nothing
                        break;
                }
            } else if (c >= 32 && c < 127) { // Printable characters
                switch (activeField) {
                    case InputField::USERNAME:
                        username += c;
                        break;
                    case InputField::PASSWORD:
                        password += c;
                        break;
                    case InputField::EMAIL:
                        email += c;
                        break;
                    case InputField::NONE:
                        // Do nothing
                        break;
                }
            }
        }
    }
    
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Enter) {
            if (currentState == AuthState::LOGIN) {
                handleLogin();
            } else {
                handleRegister();
            }
        } else if (event.key.code == sf::Keyboard::Tab) {
            // Cycle through input fields
            if (currentState == AuthState::LOGIN) {
                activeField = (activeField == InputField::USERNAME) ? InputField::PASSWORD : InputField::USERNAME;
            } else {
                switch (activeField) {
                    case InputField::USERNAME:
                        activeField = InputField::PASSWORD;
                        break;
                    case InputField::PASSWORD:
                        activeField = InputField::EMAIL;
                        break;
                    default:
                        activeField = InputField::USERNAME;
                        break;
                }
            }
            updateInputFieldColors();
        }
    }
}

void AuthenticationManager::handleLogin() {
    errorText.setString("");
    successText.setString("");
    
    if (username.empty() || password.empty()) {
        errorText.setString("Please fill in all fields");
        sf::FloatRect errorBounds = errorText.getLocalBounds();
        errorText.setOrigin(errorBounds.width / 2.0f, errorBounds.height / 2.0f);
        return;
    }
    
    if (userManager.loginUser(username, password)) {
        successText.setString("Login successful!");
        sf::FloatRect successBounds = successText.getLocalBounds();
        successText.setOrigin(successBounds.width / 2.0f, successBounds.height / 2.0f);
        currentState = AuthState::AUTHENTICATED;
    } else {
        errorText.setString("Invalid username or password");
        sf::FloatRect errorBounds = errorText.getLocalBounds();
        errorText.setOrigin(errorBounds.width / 2.0f, errorBounds.height / 2.0f);
    }
}

void AuthenticationManager::handleRegister() {
    errorText.setString("");
    successText.setString("");
    
    if (username.empty() || password.empty() || email.empty()) {
        errorText.setString("Please fill in all fields");
        sf::FloatRect errorBounds = errorText.getLocalBounds();
        errorText.setOrigin(errorBounds.width / 2.0f, errorBounds.height / 2.0f);
        return;
    }
    
    if (userManager.registerUser(email, password)) {
        successText.setString("Registration successful! Please login.");
        sf::FloatRect successBounds = successText.getLocalBounds();
        successText.setOrigin(successBounds.width / 2.0f, successBounds.height / 2.0f);
        
        // Clear fields and switch to login
        username.clear();
        password.clear();
        email.clear();
        currentState = AuthState::LOGIN;
        setupUI();
    } else {
        errorText.setString("Registration failed. Email may already exist.");
        sf::FloatRect errorBounds = errorText.getLocalBounds();
        errorText.setOrigin(errorBounds.width / 2.0f, errorBounds.height / 2.0f);
    }
}

void AuthenticationManager::switchState() {
    currentState = (currentState == AuthState::LOGIN) ? AuthState::REGISTER : AuthState::LOGIN;
    username.clear();
    password.clear();
    email.clear();
    activeField = InputField::NONE;
    errorText.setString("");
    successText.setString("");
    setupUI();
}

std::string AuthenticationManager::maskPassword(const std::string& password) {
    return std::string(password.length(), '*');
}

void AuthenticationManager::update() {
    // Update display text
    usernameDisplay.setString(username);
    passwordDisplay.setString(maskPassword(password));
    emailDisplay.setString(email);
    
    // Update button hover effects
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    
    if (isMouseOverButton(loginButton, mousePos)) {
        loginButton.setFillColor(buttonHoverColor);
    } else {
        loginButton.setFillColor(buttonColor);
    }
    
    if (isMouseOverButton(switchButton, mousePos)) {
        switchButton.setFillColor(buttonHoverColor);
    } else {
        switchButton.setFillColor(buttonColor);
    }
}

void AuthenticationManager::draw() {
    if (currentState == AuthState::AUTHENTICATED) return;
    
    // Draw background
    window.draw(backgroundRect);
    window.draw(titleText);
    
    // Draw username field
    window.draw(usernameRect);
    window.draw(usernameLabel);
    window.draw(usernameDisplay);
    
    // Draw password field
    window.draw(passwordRect);
    window.draw(passwordLabel);
    window.draw(passwordDisplay);
    
    // Draw email field (only for register)
    if (currentState == AuthState::REGISTER) {
        window.draw(emailRect);
        window.draw(emailLabel);
        window.draw(emailDisplay);
    }
    
    // Draw buttons
    window.draw(loginButton);
    window.draw(loginButtonText);
    window.draw(switchButton);
    window.draw(switchButtonText);
    
    // Draw error/success messages
    if (!errorText.getString().isEmpty()) {
        window.draw(errorText);
    }
    if (!successText.getString().isEmpty()) {
        window.draw(successText);
    }
}

void AuthenticationManager::reset() {
    currentState = AuthState::LOGIN;
    activeField = InputField::NONE;
    username.clear();
    password.clear();
    email.clear();
    errorText.setString("");
    successText.setString("");
    setupUI();
}