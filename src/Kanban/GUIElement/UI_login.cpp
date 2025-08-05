//
// Created by diego on 7/31/2025.
//
#include "UI_login.h"
#include <iostream>

//void random::rectangle_fill_color(sf::RectangleShape& r_) {
//    r_.setFillColor(sf::Color::Black);
//}

// int wajaja() {
//     sf::RectangleShape bola;
//     bola.setFillColor(sf::Color::Blue);
//     return 0;
// }


void SET_loginRect() {
    LOGIN_Rect.setFillColor(sf::Color(18, 18, 20)); // (66, 84, 114)
    LOGIN_Rect.setSize(sf::Vector2f(510, 320));
    LOGIN_Rect.setOrigin(LOGIN_Rect.getGlobalBounds().width/2,
                         LOGIN_Rect.getGlobalBounds().height/2);
    LOGIN_Rect.setPosition(window.getSize().x / 2,window.getSize().y / 2);
}

void SET_userName_text() {
    userName_text.setString("Username: ");
    userName_text.setFont(font);
    userName_text.setCharacterSize(30);
    //userName_text.setStyle(sf::Text::Bold);
    userName_text.setFillColor(sf::Color::White);
    userName_text.setOrigin(userName_text.getGlobalBounds().width/2,
                            userName_text.getGlobalBounds().height/2);
    userName_text.setPosition(window.getSize().x / 2 - 160,
                              window.getSize().y / 2 - 110);
}

void SET_userName_rect() {
    userName_rect.setFillColor(sf::Color::White);
    userName_rect.setOutlineColor(sf::Color::Black);
    userName_rect.setSize(sf::Vector2f(310, 40));
    userName_rect.setOutlineThickness(3);
    userName_rect.setOrigin(userName_rect.getGlobalBounds().width/2,
                            userName_rect.getGlobalBounds().height/2);
    userName_rect.setPosition(window.getSize().x / 2 + 70,
                              window.getSize().y / 2 - 100);
}

void SET_userName() {
    userName.setString(Username_string);
    userName.setFont(font);
    userName.setCharacterSize(30);
    userName.setFillColor(sf::Color::Black);
    userName.setOrigin(userName.getGlobalBounds().width/2,
                       userName.getGlobalBounds().height/2);
    userName.setPosition(window.getSize().x / 2 - 85,
                         window.getSize().y / 2 - 40);
}

void SET_userPasswrd_text() {
    userPasswrd_text.setString("Password: ");
    userPasswrd_text.setFont(font);
    userPasswrd_text.setCharacterSize(30);
    userPasswrd_text.setFillColor(sf::Color::White);
    userPasswrd_text.setOrigin(userName_text.getGlobalBounds().width/2,
                               userName_text.getGlobalBounds().height/2);
    userPasswrd_text.setPosition(window.getSize().x / 2 - 160,
                                 window.getSize().y / 2 - 30);
}

void SET_userPasswrd_rect() {
    userPasswrd_rect.setFillColor(sf::Color::White);
    userPasswrd_rect.setOutlineColor(sf::Color::Black);
    userPasswrd_rect.setSize(sf::Vector2f(310, 40));
    userPasswrd_rect.setOutlineThickness(3);
    userPasswrd_rect.setOrigin(userPasswrd_rect.getGlobalBounds().width/2,
                               userPasswrd_rect.getGlobalBounds().height/2);
    userPasswrd_rect.setPosition(window.getSize().x / 2 + 70,
                                 window.getSize().y / 2 - 20);
}

void SET_userPasswrd() {
    userPasswrd.setString(Userpasswrd_string);
    userPasswrd.setFont(font);
    userPasswrd.setCharacterSize(30);
    userPasswrd.setFillColor(sf::Color::Black);
    userPasswrd.setOrigin(userPasswrd.getGlobalBounds().width/2,
                          userPasswrd.getGlobalBounds().height/2);
    userPasswrd.setPosition(window.getSize().x / 2 - 85,
                            window.getSize().y / 2 - 40);
}

void SET_font() {
    font.loadFromFile("resources/aovel-sans-rounded-font/AovelSansRounded-rdDL.ttf"); //("resources/aovel-sans-rounded-font/AovelSansRounded-rdDL.ttf");
} // /home/jepeto/login_sfml/build

void SET_login_button_unpressed_texture() {
    if(!login_button_unpressed_texture.loadFromFile("resources/textures/login_unpressed.png")) {
        std::cout << "Button Unpressed texture not loaded!" << "\n";
    }
}
void SET_login_button_unpressed_sprite() {
    login_button_unpressed_sprite.setTexture(login_button_unpressed_texture);
    login_button_unpressed_sprite.setOrigin(login_button_unpressed_sprite.getGlobalBounds().width/2,
                                            login_button_unpressed_sprite.getGlobalBounds().height/2);
    login_button_unpressed_sprite.setPosition(window.getSize().x/ 2 - 100,
                                              window.getSize().y/ 2 + 55);
}

void SET_login_button_pressed_texture() {
    if(!login_button_pressed_texture.loadFromFile("resources/textures/login_pressed.png")) { //("resources/textures/b_pressed.png")) {
        std::cout << "Button Unpressed texture not loaded!" << "\n";
    }
}
void SET_login_button_pressed_sprite() {
    login_button_pressed_sprite.setTexture(login_button_pressed_texture);
    login_button_pressed_sprite.setOrigin(login_button_pressed_sprite.getGlobalBounds().width/2,
                                          login_button_pressed_sprite.getGlobalBounds().height/2);
    login_button_pressed_sprite.setPosition(window.getSize().x / 2 - 100,
                                            window.getSize().y / 2 + 55);
}

void SET_signup_pressed_texture() {
    if(!signup_pressed_texture.loadFromFile("resources/textures/signup_pressed.png")) {
        std::cout << "Button Unpressed texture not loaded!" << "\n";
    }
}
void SET_signup_pressed_sprite() {
    signup_pressed_sprite.setTexture(signup_pressed_texture);
    signup_pressed_sprite.setOrigin(signup_pressed_sprite.getGlobalBounds().width/2,
                                    signup_pressed_sprite.getGlobalBounds().height/2);
    signup_pressed_sprite.setPosition(window.getSize().x / 2 + 100,
                                      window.getSize().y / 2 + 55);
}

void SET_signup_unpressed_texture() {
    if(!signup_unpressed_texture.loadFromFile("resources/textures/signup_unpressed.png")) {
        std::cout << "signup Button Unpressed texture not loaded!" << "\n";
    }
}
void SET_signup_unpressed_sprite() {
    signup_unpressed_sprite.setTexture(signup_unpressed_texture);
    signup_unpressed_sprite.setOrigin(signup_unpressed_sprite.getGlobalBounds().width/2,
                                      signup_unpressed_sprite.getGlobalBounds().height/2);
    signup_unpressed_sprite.setPosition(window.getSize().x / 2 + 100,
                                        window.getSize().y / 2 + 55);
}

void SET_wrongLogin_text() {
    wrongLogin_text.setString("The username or password is not valid! Try again.");
    wrongLogin_text.setFont(font);
    wrongLogin_text.setCharacterSize(25);
    wrongLogin_text.setFillColor(sf::Color(255, 221, 112));
    wrongLogin_text.setOrigin(wrongLogin_text.getGlobalBounds().width/2,
                              wrongLogin_text.getGlobalBounds().height/2);
    wrongLogin_text.setPosition(window.getSize().x / 2,
                                window.getSize().y / 2 + 125);
}

void SET_validLogin_text() {
    validLogin_text.setString("Valid login. Loading Table.");
    validLogin_text.setFont(font);
    validLogin_text.setCharacterSize(25);
    validLogin_text.setFillColor(sf::Color(255, 221, 112));
    validLogin_text.setOrigin(validLogin_text.getGlobalBounds().width/2,
                              validLogin_text.getGlobalBounds().height/2);
    validLogin_text.setPosition(window.getSize().x / 2,
                                window.getSize().y / 2 + 125);
}

void SET_wrong_SignUp_text() {
    wrong_SignUp_text.setString("Invalid input or username already exists! Try again.");
    wrong_SignUp_text.setFont(font);
    wrong_SignUp_text.setCharacterSize(25);
    wrong_SignUp_text.setFillColor(sf::Color(255, 221, 112));
    wrong_SignUp_text.setOrigin(wrong_SignUp_text.getGlobalBounds().width/2,
                                wrong_SignUp_text.getGlobalBounds().height/2);
    wrong_SignUp_text.setPosition(window.getSize().x / 2,
                                  window.getSize().y / 2 + 180);
}

void SET_valid_SignUp_text() {
    valid_SignUp_text.setString("Valid sign up. Loading Table.");
    valid_SignUp_text.setFont(font);
    valid_SignUp_text.setCharacterSize(25);
    valid_SignUp_text.setFillColor(sf::Color(255, 221, 112));
    valid_SignUp_text.setOrigin(valid_SignUp_text.getGlobalBounds().width/2,
                                valid_SignUp_text.getGlobalBounds().height/2);
    valid_SignUp_text.setPosition(window.getSize().x / 2,
                                  window.getSize().y / 2 + 125);
}

void SET_LOGIN() {
    SET_loginRect();
    SET_userName_rect();
    SET_userPasswrd_rect();
    SET_font();
    SET_userName_text();
    SET_userPasswrd_text();
    SET_userPasswrd();
    SET_userName();
    SET_login_button_unpressed_texture();
    SET_login_button_unpressed_sprite();
    SET_login_button_pressed_texture();
    SET_login_button_pressed_sprite();
    SET_wrongLogin_text();
    SET_validLogin_text();
    SET_signup_unpressed_texture();
    SET_signup_unpressed_sprite();
    SET_signup_pressed_texture();
    SET_signup_pressed_sprite();
    SET_wrong_SignUp_text();
    SET_valid_SignUp_text();
}

//================================================================================



//================================================================================

void SET_UserSignUp_rect() {
    UserSignUp_rect.setFillColor(sf::Color(18, 18, 20)); // (66, 84, 114)
    UserSignUp_rect.setSize(sf::Vector2f(510, 480));
    UserSignUp_rect.setOrigin(UserSignUp_rect.getGlobalBounds().width/2,
                         UserSignUp_rect.getGlobalBounds().height/2);
    UserSignUp_rect.setPosition(window.getSize().x / 2,window.getSize().y / 2);

}

void SET_UserSignUp_text() {
    UserSignUp_text.setString("New User");
    UserSignUp_text.setFont(font);
    UserSignUp_text.setCharacterSize(30);
    UserSignUp_text.setOrigin(UserSignUp_text.getGlobalBounds().width/2,
                                UserSignUp_text.getGlobalBounds().height/2);
    UserSignUp_text.setPosition(window.getSize().x / 2,
                                window.getSize().y / 2 - 180);
}

void SET_SIGN_UP_underLine() {
    SIGN_UP_underLine.setFillColor(sf::Color::White);
    SIGN_UP_underLine.setSize(sf::Vector2f(400, 2));
    SIGN_UP_underLine.setOrigin(SIGN_UP_underLine.getGlobalBounds().width / 2,
                        SIGN_UP_underLine.getGlobalBounds().height / 2);
    SIGN_UP_underLine.setPosition(window.getSize().x / 2,
                          window.getSize().y / 2 - 150);

}


void SET_SIGN_UP_userName_text() {
    SIGN_UP_userName_text.setString("Username: ");
    SIGN_UP_userName_text.setFont(font);
    SIGN_UP_userName_text.setCharacterSize(30);
    //userName_text.setStyle(sf::Text::Bold);
    SIGN_UP_userName_text.setFillColor(sf::Color::White);
    SIGN_UP_userName_text.setOrigin(SIGN_UP_userName_text.getGlobalBounds().width/2,
                                    SIGN_UP_userName_text.getGlobalBounds().height/2);
    SIGN_UP_userName_text.setPosition(window.getSize().x / 2 - 160,
                              window.getSize().y / 2 - 100);
}

void SET_SIGN_UP_userName_rect(){
    SIGN_UP_userName_rect.setFillColor(sf::Color::White);
    SIGN_UP_userName_rect.setOutlineColor(sf::Color::Black);
    SIGN_UP_userName_rect.setSize(sf::Vector2f(310, 40));
    SIGN_UP_userName_rect.setOutlineThickness(3);
    SIGN_UP_userName_rect.setOrigin(SIGN_UP_userName_rect.getGlobalBounds().width/2,
                            SIGN_UP_userName_rect.getGlobalBounds().height/2);
    SIGN_UP_userName_rect.setPosition(window.getSize().x / 2 + 70,
                              window.getSize().y / 2 - 90);
}

void SET_new_userName() {
    new_userName.setString(new_username_string);
    new_userName.setFont(font);
    new_userName.setCharacterSize(30);
    new_userName.setFillColor(sf::Color::Black);
    new_userName.setOrigin(new_userName.getGlobalBounds().width/2,
                       new_userName.getGlobalBounds().height/2);
    new_userName.setPosition(window.getSize().x / 2 - 85,
                         window.getSize().y / 2 - 110);
}


void SET_SIGN_UP_userPasswrd_text() {
    SIGN_UP_userPasswrd_text.setString("Password: ");
    SIGN_UP_userPasswrd_text.setFont(font);
    SIGN_UP_userPasswrd_text.setCharacterSize(30);
    SIGN_UP_userPasswrd_text.setFillColor(sf::Color::White);
    SIGN_UP_userPasswrd_text.setOrigin(SIGN_UP_userPasswrd_text.getGlobalBounds().width/2,
                                    SIGN_UP_userPasswrd_text.getGlobalBounds().height/2);
    SIGN_UP_userPasswrd_text.setPosition(window.getSize().x / 2 - 160,
                                      window.getSize().y / 2 - 25);
}

void SET_SIGN_UP_userPasswrd_rect() {
    SIGN_UP_userPasswrd_rect.setFillColor(sf::Color::White);
    SIGN_UP_userPasswrd_rect.setOutlineColor(sf::Color::Black);
    SIGN_UP_userPasswrd_rect.setSize(sf::Vector2f(310, 40));
    SIGN_UP_userPasswrd_rect.setOutlineThickness(3);
    SIGN_UP_userPasswrd_rect.setOrigin(SIGN_UP_userPasswrd_rect.getGlobalBounds().width/2,
                                    SIGN_UP_userPasswrd_rect.getGlobalBounds().height/2);
    SIGN_UP_userPasswrd_rect.setPosition(window.getSize().x / 2 + 70,
                                      window.getSize().y / 2 - 15);
}

void SET_new_userPasswrd() {
    new_userPasswrd.setString(new_userPasswrd_string);
    new_userPasswrd.setFont(font);
    new_userPasswrd.setCharacterSize(30);
    new_userPasswrd.setFillColor(sf::Color::Black);
    new_userPasswrd.setOrigin(new_userPasswrd.getGlobalBounds().width/2,
                           new_userPasswrd.getGlobalBounds().height/2);
    new_userPasswrd.setPosition(window.getSize().x / 2 - 85,
                             window.getSize().y / 2 - 35);
}


void SET_SIGN_UP_userEmail_text() {
    SIGN_UP_userEmail_text.setString("   Email: ");
    SIGN_UP_userEmail_text.setFont(font);
    SIGN_UP_userEmail_text.setCharacterSize(30);
    SIGN_UP_userEmail_text.setFillColor(sf::Color::White);
    SIGN_UP_userEmail_text.setOrigin(SIGN_UP_userEmail_text.getGlobalBounds().width/2,
                                       SIGN_UP_userEmail_text.getGlobalBounds().height/2);
    SIGN_UP_userEmail_text.setPosition(window.getSize().x / 2 - 160,
                                         window.getSize().y / 2 + 55);
}

void SET_SIGN_UP_userEmail_rect() {
    SIGN_UP_userEmail_rect.setFillColor(sf::Color::White);
    SIGN_UP_userEmail_rect.setOutlineColor(sf::Color::Black);
    SIGN_UP_userEmail_rect.setSize(sf::Vector2f(310, 40));
    SIGN_UP_userEmail_rect.setOutlineThickness(3);
    SIGN_UP_userEmail_rect.setOrigin(SIGN_UP_userEmail_rect.getGlobalBounds().width/2,
                                       SIGN_UP_userEmail_rect.getGlobalBounds().height/2);
    SIGN_UP_userEmail_rect.setPosition(window.getSize().x / 2 + 70,
                                         window.getSize().y / 2 + 65);
}

void SET_new_userEmail() {
    new_userEmail.setString(new_userEmail_string);
    new_userEmail.setFont(font);
    new_userEmail.setCharacterSize(30);
    new_userEmail.setFillColor(sf::Color::Black);
    new_userEmail.setOrigin(new_userEmail.getGlobalBounds().width/2,
                              new_userEmail.getGlobalBounds().height/2);
    new_userEmail.setPosition(window.getSize().x / 2 - 85,
                                window.getSize().y / 2 + 45);
}

void SET_SIGN_UP_confirmButton_unpressed_texture() {
    if(!SIGN_UP_confirmButton_unpressed_texture.loadFromFile("resources/textures/confirm_unpressed.png")) {
        std::cout << "signup Button Unpressed texture not loaded!" << "\n";
    }
}
void SET_SIGN_UP_confirmButton_unpressed_sprite() {
    SIGN_UP_confirmButton_unpressed_sprite.setTexture(SIGN_UP_confirmButton_unpressed_texture);
    SIGN_UP_confirmButton_unpressed_sprite.setOrigin(SIGN_UP_confirmButton_unpressed_sprite.getGlobalBounds().width/2,
                                            SIGN_UP_confirmButton_unpressed_sprite.getGlobalBounds().height/2);
    SIGN_UP_confirmButton_unpressed_sprite.setPosition(window.getSize().x/ 2 + 100,
                                              window.getSize().y/ 2 + 140);
}

void SET_SIGN_UP_confirmButton_pressed_texture() {
    if(!SIGN_UP_confirmButton_pressed_texture.loadFromFile("resources/textures/confirm_pressed.png")) {
        std::cout << "signup Button Unpressed texture not loaded!" << "\n";
    }
}
void SET_SIGN_UP_confirmButton_pressed_sprite() {
    SIGN_UP_confirmButton_pressed_sprite.setTexture(SIGN_UP_confirmButton_pressed_texture);
    SIGN_UP_confirmButton_pressed_sprite.setOrigin(SIGN_UP_confirmButton_pressed_sprite.getGlobalBounds().width/2,
                                                     SIGN_UP_confirmButton_pressed_sprite.getGlobalBounds().height/2);
    SIGN_UP_confirmButton_pressed_sprite.setPosition(window.getSize().x/ 2 + 100,
                                                       window.getSize().y/ 2 + 140);
}


void SET_SIGN_UP_cancelButton_pressed_texture() {
    if(!SIGN_UP_cancelButton_pressed_texture.loadFromFile("resources/textures/cancel_pressed.png")) {
        std::cout << "signup Button Unpressed texture not loaded!" << "\n";
    }
}
void SET_SIGN_UP_cancelButton_pressed_sprite() {
    SIGN_UP_cancelButton_pressed_sprite.setTexture(SIGN_UP_cancelButton_pressed_texture);
    SIGN_UP_cancelButton_pressed_sprite.setOrigin(SIGN_UP_cancelButton_pressed_sprite.getGlobalBounds().width/2,
                                                   SIGN_UP_cancelButton_pressed_sprite.getGlobalBounds().height/2);
    SIGN_UP_cancelButton_pressed_sprite.setPosition(window.getSize().x/ 2 - 100,
                                                     window.getSize().y/ 2 + 140);
}

void SET_SIGN_UP_cancelButton_unpressed_texture() {
    if(!SIGN_UP_cancelButton_unpressed_texture.loadFromFile("resources/textures/cancel_unpressed.png")) {
        std::cout << "signup Button Unpressed texture not loaded!" << "\n";
    }
}
void SET_SIGN_UP_cancelButton_unpressed_sprite() {
    SIGN_UP_cancelButton_unpressed_sprite.setTexture(SIGN_UP_cancelButton_unpressed_texture);
    SIGN_UP_cancelButton_unpressed_sprite.setOrigin(SIGN_UP_cancelButton_unpressed_sprite.getGlobalBounds().width/2,
                                                  SIGN_UP_cancelButton_unpressed_sprite.getGlobalBounds().height/2);
    SIGN_UP_cancelButton_unpressed_sprite.setPosition(window.getSize().x/ 2 - 100,
                                                    window.getSize().y/ 2 + 140);
}

void SET_SIGNUP() {
    SET_UserSignUp_rect();
    SET_UserSignUp_text();
    SET_SIGN_UP_underLine();// maybe removable from here

    SET_SIGN_UP_userName_text();
    SET_SIGN_UP_userName_rect();
    SET_new_userName();

    SET_SIGN_UP_userPasswrd_text();
    SET_SIGN_UP_userPasswrd_rect();
    SET_new_userPasswrd();

    SET_SIGN_UP_userEmail_text();
    SET_SIGN_UP_userEmail_rect();
    SET_new_userEmail();

    SET_SIGN_UP_confirmButton_unpressed_texture();
    SET_SIGN_UP_confirmButton_unpressed_sprite();

    SET_SIGN_UP_confirmButton_pressed_texture();
    SET_SIGN_UP_confirmButton_pressed_sprite();

    SET_SIGN_UP_cancelButton_unpressed_texture();
    SET_SIGN_UP_cancelButton_unpressed_sprite();

    SET_SIGN_UP_cancelButton_pressed_texture();
    SET_SIGN_UP_cancelButton_pressed_sprite();
}

//================================================================================





