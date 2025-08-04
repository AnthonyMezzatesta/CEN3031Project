#include <iostream>
// #include <SFML/Graphics.hpp>
#include "UI_login.h"

//using namespace std;


// int main()
// {
//     cout << "Hello World!" << endl;
//     return 0;
// }

int main ()
{
    // Create a new render-window
    std::cout << "Hello game!" << "\n";

    //UI_state current_state = user_login_STATE;

    //sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML window");
    //window_1.setActive(false);
    //sf::RenderWindow window_2(sf::VideoMode(1280, 720), "SFML window");

    window.setFramerateLimit(60);

    if(false) {
        //sf::RectangleShape rect_;
        // rect_.setFillColor(sf::Color(18, 18, 20)); // (66, 84, 114)
        // rect_.setSize(sf::Vector2f(500, 320));
        // rect_.setOrigin(rect_.getGlobalBounds().width/2, rect_.getGlobalBounds().height/2);

        //sf::RectangleShape userName_rect;
        // userName_rect.setFillColor(sf::Color::White);
        // userName_rect.setOutlineColor(sf::Color::Black);
        // userName_rect.setSize(sf::Vector2f(310, 40));
        // userName_rect.setOutlineThickness(3);
        // userName_rect.setOrigin(userName_rect.getGlobalBounds().width/2,
        //                         userName_rect.getGlobalBounds().height/2);

        // sf::RectangleShape userPasswrd_rect;
        // userPasswrd_rect.setFillColor(sf::Color::White);
        // userPasswrd_rect.setOutlineColor(sf::Color::Black);
        // userPasswrd_rect.setSize(sf::Vector2f(310, 40));
        // userPasswrd_rect.setOutlineThickness(3);
        // userPasswrd_rect.setOrigin(userPasswrd_rect.getGlobalBounds().width/2,
        //                         userPasswrd_rect.getGlobalBounds().height/2);


        // sf::Font font;
        // font.loadFromFile("../resources/aovel-sans-rounded-font/AovelSansRounded-rdDL.ttf");


        // Create a text
        // sf::Text userName_text("Username:", font);
        // userName_text.setCharacterSize(30);
        // //userName_text.setStyle(sf::Text::Bold);
        // userName_text.setFillColor(sf::Color::White);
        // userName_text.setOrigin(userName_text.getGlobalBounds().width/2,
        //                         userName_text.getGlobalBounds().height/2);

        // sf::Text userPasswrd_text("Password:", font);
        // userPasswrd_text.setCharacterSize(30);
        // userPasswrd_text.setFillColor(sf::Color::White);
        // userPasswrd_text.setOrigin(userName_text.getGlobalBounds().width/2,
        //                            userName_text.getGlobalBounds().height/2);
        //==================================================================================================================

        // std::string Username_string = "";
        // std::string Userpasswrd_string = "";

        // sf::Text userPasswrd(Userpasswrd_string, font);
        // userPasswrd.setCharacterSize(30);
        // userPasswrd.setFillColor(sf::Color::Black);
        // userPasswrd.setOrigin(userPasswrd.getGlobalBounds().width/2,
        //                       userPasswrd.getGlobalBounds().height/2);

        // sf::Text userName(Username_string, font);
        // userName.setCharacterSize(30);
        // userName.setFillColor(sf::Color::Black);
        // userName.setOrigin(userName.getGlobalBounds().width/2,
        //                    userName.getGlobalBounds().height/2);

        //==================================================================================================================

        // LOGIN button pressed and unpressed

        // sf::Texture button_unpressed_texture;
        // if(!button_unpressed_texture.loadFromFile("../resources/textures/b_unpressed.png")) {
        //     std::cout << "Button Unpressed texture not loaded!" << "\n";
        // }

        // sf::Sprite button_unpressed_sprite;
        // button_unpressed_sprite.setTexture(button_unpressed_texture);
        // button_unpressed_sprite.setOrigin(button_unpressed_sprite.getGlobalBounds().width/2,
        //                                   button_unpressed_sprite.getGlobalBounds().height/2);


        // sf::Texture button_pressed_texture;
        // if(!button_pressed_texture.loadFromFile("../resources/textures/b_pressed.png")) {
        //     std::cout << "Button Unpressed texture not loaded!" << "\n";
        // }

        // sf::Sprite button_pressed_sprite;
        // button_pressed_sprite.setTexture(button_pressed_texture);
        // button_pressed_sprite.setOrigin(button_pressed_sprite.getGlobalBounds().width/2,
        //                                   button_pressed_sprite.getGlobalBounds().height/2);

        // sf::Text wrongLogin_text("The username or password is not valid! Try again.", font);
        // wrongLogin_text.setCharacterSize(25);
        // wrongLogin_text.setFillColor(sf::Color(255, 221, 112));
        // wrongLogin_text.setOrigin(wrongLogin_text.getGlobalBounds().width/2,
        //                           wrongLogin_text.getGlobalBounds().height/2);

        // sf::Text validLogin_text("Valid login. Loading Table.", font);
        // validLogin_text.setCharacterSize(25);
        // validLogin_text.setFillColor(sf::Color(255, 221, 112));
        // validLogin_text.setOrigin(validLogin_text.getGlobalBounds().width/2,
        //                           validLogin_text.getGlobalBounds().height/2);

        //==================================================================================================================

        // SIGN UP button pressed and unpressed

        // sf::Texture signup_unpressed_texture;
        // if(!signup_unpressed_texture.loadFromFile("../resources/textures/sign_up_unpressed.png")) {
        //     std::cout << "signup Button Unpressed texture not loaded!" << "\n";
        // }

        // sf::Sprite signup_unpressed_sprite;
        // signup_unpressed_sprite.setTexture(signup_unpressed_texture);
        // signup_unpressed_sprite.setOrigin(signup_unpressed_sprite.getGlobalBounds().width/2,
        //                                   signup_unpressed_sprite.getGlobalBounds().height/2);


        // sf::Texture signup_pressed_texture;
        // if(!signup_pressed_texture.loadFromFile("../resources/textures/sign_up_pressed.png")) {
        //     std::cout << "Button Unpressed texture not loaded!" << "\n";
        // }

        // sf::Sprite signup_pressed_sprite;
        // signup_pressed_sprite.setTexture(signup_pressed_texture);
        // signup_pressed_sprite.setOrigin(signup_pressed_sprite.getGlobalBounds().width/2,
        //                                 signup_pressed_sprite.getGlobalBounds().height/2);

        // sf::Text wrong_SignUp_text("Invalid input or username already exists! Try again.", font);
        // wrong_SignUp_text.setCharacterSize(25);
        // wrong_SignUp_text.setFillColor(sf::Color(255, 221, 112));
        // wrong_SignUp_text.setOrigin(wrong_SignUp_text.getGlobalBounds().width/2,
        //                             wrong_SignUp_text.getGlobalBounds().height/2);

        // sf::Text valid_SignUp_text("Valid sign up. Loading Table.", font);
        // valid_SignUp_text.setCharacterSize(25);
        // valid_SignUp_text.setFillColor(sf::Color(255, 221, 112));
        // valid_SignUp_text.setOrigin(valid_SignUp_text.getGlobalBounds().width/2,
        //                             valid_SignUp_text.getGlobalBounds().height/2);
    }
    //==================================================================
    UI_state current_state = user_login_STATE;

    SET_LOGIN();
    SET_SIGNUP();
    //==================================================================
    float mouse_x = 0;
    float mouse_y = 0;

    bool login_pressed = false;

    bool userName_pressed = false;
    bool userPasswrd_pressed = false;
    bool userEmail_pressed = false;
    int counter = 0;
    int login_counter = 0;

    bool invalid_login = false;
    bool valid_login = false;

    bool signup_pressed = false;

    bool invalid_signup = false;
    bool valid_signup = false;

    //==================================================================

    bool confirmButton_pressed = false;
    bool cancelButton_pressed = false;
    //auto mouse_pos  = sf::Vector2f(0.f, 0.f);

    // The main loop
    while (window.isOpen()) {
        mouse_x = sf::Mouse::getPosition(window).x;
        mouse_y = sf::Mouse::getPosition(window).y;

        sf::Event event{};
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (current_state == user_login_STATE) {

                if (event.type == sf::Event::MouseButtonPressed){
                    if(event.mouseButton.button == sf::Mouse::Left) {
                        std::cout << "clicked!" << counter <<"\n";
                        counter += 1;
                        if (login_button_unpressed_sprite.getGlobalBounds().contains(mouse_x, mouse_y)) {
                            login_pressed = true;
                            userPasswrd_pressed = false;
                            userName_pressed = false;
                            invalid_signup = false; // ----
                            signup_pressed = false; // ----

                            // TODO: ACTUAL LOGIN VERIFICATION CAN HAPPEN HERE. update valid_login and/or invalid_login boolean variables
                        }

                        else if(signup_unpressed_sprite.getGlobalBounds().contains(mouse_x, mouse_y)) {
                            signup_pressed = true;
                            userPasswrd_pressed = false;
                            userName_pressed = false;
                            invalid_login = false; //---
                            login_pressed = false; //---

                            //current_state = user_sign_up_STATE; // IMPORTANT
                        }

                        else if(userName_rect.getGlobalBounds().contains(mouse_x, mouse_y)) {
                            userName_pressed = !userName_pressed;
                            userPasswrd_pressed = false;
                            invalid_login = false;
                        }
                        else if(userPasswrd_rect.getGlobalBounds().contains(mouse_x, mouse_y)) {
                            userPasswrd_pressed = !userPasswrd_pressed;
                            userName_pressed = false;
                            invalid_login = false;
                        }
                        else {
                            userPasswrd_pressed = false;
                            userName_pressed = false;
                        }
                    }
                }

                if (userName_pressed) {
                    // if (event.key.code == sf::Keyboard::BackSpace) {
                    //     Username_string = Username_string.substr(0, Username_string.size() - 1);
                    //     userName.setString(Username_string);
                    // }
                    if (event.type == sf::Event::TextEntered) {
                        if (event.text.unicode == '\b') {
                            Username_string = Username_string.substr(0, Username_string.size() - 1);
                            userName.setString(Username_string);
                        }
                        //else if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Tab) {
                        else if (event.text.unicode == '\r' || event.text.unicode == '\t') {
                            //userName.setString(Username_string);
                            userName_pressed = false;
                            userPasswrd_pressed = true;
                        }

                        else if(event.text.unicode < 128) {
                            Username_string += static_cast<char>(event.text.unicode);
                            userName.setString(Username_string);
                        }
                    }

                }

                else if(userPasswrd_pressed) {
                    if (event.type == sf::Event::TextEntered) {

                        //if (event.key.code == sf::Keyboard::Escape) {

                        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Backspace)) {
                        //     Userpasswrd_string = Userpasswrd_string.substr(0, Userpasswrd_string.size() - 1);

                        // }

                        if (event.text.unicode == '\b') {
                            Userpasswrd_string = Userpasswrd_string.substr(0, Userpasswrd_string.size() - 1);
                            userPasswrd.setString(Userpasswrd_string);
                        }

                        // else if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Tab) {
                        //     //userPasswrd.setString(Userpasswrd_string);
                        //     //userName_pressed = false;
                        //     userPasswrd_pressed = false;
                        // }

                        else if (event.text.unicode == '\r') {
                            //userPasswrd.setString(Userpasswrd_string);
                            userName_pressed = false;
                            userPasswrd_pressed = false;
                            // MAYBE try to login as if the actual button was clicked
                        }

                        else if( event.text.unicode != '\t' && event.text.unicode < 128) {
                            Userpasswrd_string += static_cast<char>(event.text.unicode);
                            userPasswrd.setString(Userpasswrd_string);
                        }
                    }
                }

                else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                     login_pressed = false;
                     signup_pressed = false;
                }

                if (event.type == sf::Event::Resized) {
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));

                    LOGIN_Rect.setPosition(event.size.width/2, event.size.height/2);

                    userName_text.setPosition(event.size.width/2 - 160, event.size.height/2 - 110);
                    userName_rect.setPosition(event.size.width/2 + 70, event.size.height/2 - 100);
                    userName.setPosition(event.size.width/2 - 85, event.size.height/2 - 120);

                    userPasswrd_text.setPosition(event.size.width/2 - 160, event.size.height/2 - 30);
                    userPasswrd_rect.setPosition(event.size.width/2 + 70, event.size.height/2 - 20);
                    userPasswrd.setPosition(event.size.width/2 - 85, event.size.height/2 - 40);

                    login_button_unpressed_sprite.setPosition(event.size.width/2 - 100, event.size.height/2 + 55);
                    login_button_pressed_sprite.setPosition(event.size.width/2 - 100, event.size.height/2 + 55);

                    signup_pressed_sprite.setPosition(event.size.width/2 + 100, event.size.height/2 + 55);
                    signup_unpressed_sprite.setPosition(event.size.width/2 + 100, event.size.height/2 + 55);

                    wrongLogin_text.setPosition(event.size.width/2, event.size.height/2 + 125);
                    validLogin_text.setPosition(event.size.width/2, event.size.height/2 + 125);

                    //wrong_SignUp_text.setPosition(event.size.width/2, event.size.height/2 + 125);
                    //valid_SignUp_text.setPosition(event.size.width/2, event.size.height/2 + 125);
                    //rect_.setSize(sf::Vector2f(event.size.width, event.size.height));
                }

                //if (event.mouseButton.button == sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {


                if(userPasswrd_pressed) {
                    userPasswrd_rect.setOutlineColor(sf::Color(0, 153, 255));
                }
                else {
                    userPasswrd_rect.setOutlineColor(sf::Color::Black);
                }
                if(userName_pressed) {
                    userName_rect.setOutlineColor(sf::Color(0, 153, 255));
                }
                else {
                    userName_rect.setOutlineColor(sf::Color::Black);
                }
            }


            else if (current_state == user_sign_up_STATE) {              
                if (event.type == sf::Event::MouseButtonPressed){
                    if(event.mouseButton.button == sf::Mouse::Left) {
                        std::cout << "clicked!" << counter <<"\n";
                        counter += 1;
                        if (SIGN_UP_confirmButton_unpressed_sprite.getGlobalBounds().contains(mouse_x, mouse_y)) {
                             confirmButton_pressed = true;

                            // TODO: CONFIRMATION OF VALID USER SIGN UP CAN HAPPEN HERE also. update valid_signup and/or invalid_signup boolean variables
                        //     userPasswrd_pressed = false;
                        //     userName_pressed = false;
                        //     invalid_signup = false; // ----
                        //     signup_pressed = false; // ----
                        }

                         else if(SIGN_UP_cancelButton_unpressed_sprite.getGlobalBounds().contains(mouse_x, mouse_y)) {
                            cancelButton_pressed = true;
                        //     userPasswrd_pressed = false;
                        //     userName_pressed = false;
                        //     invalid_login = false; //---
                        //     login_pressed = false; //---
                            login_pressed = false;

                            userName_pressed = false;
                            userPasswrd_pressed = false;
                            userEmail_pressed = false;

                            invalid_login = false;
                            valid_login = false;

                            signup_pressed = false;

                            invalid_signup = false;
                            valid_signup = false;

                            current_state = user_login_STATE;
                        }

                        if(SIGN_UP_userName_rect.getGlobalBounds().contains(mouse_x, mouse_y)) {
                            userName_pressed = !userName_pressed;
                            userPasswrd_pressed = false;
                            userEmail_pressed = false;
                        }
                        else if(SIGN_UP_userPasswrd_rect.getGlobalBounds().contains(mouse_x, mouse_y)) {
                            std::cout << "SIGN_UP_userPasswrd_rect was clicked!" << "\n";
                            userPasswrd_pressed = !userPasswrd_pressed;
                            userName_pressed = false;
                            userEmail_pressed = false;
                        }
                        else if(SIGN_UP_userEmail_rect.getGlobalBounds().contains(mouse_x, mouse_y)) {
                            userEmail_pressed = !userEmail_pressed;
                            userPasswrd_pressed = false;
                            userName_pressed = false;
                        }
                        else {
                            userPasswrd_pressed = false;
                            userName_pressed = false;
                            userEmail_pressed = false;
                        }
                    }

                    if(userPasswrd_pressed) {
                        userPasswrd_rect.setOutlineColor(sf::Color(0, 153, 255));
                    }
                    else {
                        userPasswrd_rect.setOutlineColor(sf::Color::Black);
                    }
                    if(userName_pressed) {
                        userName_rect.setOutlineColor(sf::Color(0, 153, 255));
                    }
                    else {
                        userName_rect.setOutlineColor(sf::Color::Black);
                    }
                    if (userEmail_pressed) {
                        SIGN_UP_userEmail_rect.setOutlineColor(sf::Color(0, 153, 255));
                    }
                    else {
                        SIGN_UP_userEmail_rect.setOutlineColor(sf::Color::Black);
                    }
                }

                if (userName_pressed) {
                    if (event.type == sf::Event::TextEntered) {
                        if (event.text.unicode == '\b') {
                            new_username_string = new_username_string.substr(0, new_username_string.size() - 1);
                            new_userName.setString(new_username_string);
                        }
                        //else if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Tab) {
                        else if (event.text.unicode == '\r' || event.text.unicode == '\t') {
                            //userName.setString(Username_string);
                            userName_pressed = false;
                            userPasswrd_pressed = true;
                            userEmail_pressed = false;
                        }

                        else if(event.text.unicode < 128) {
                            new_username_string += static_cast<char>(event.text.unicode);
                            new_userName.setString(new_username_string);
                        }
                    }

                }

                else if(userPasswrd_pressed) {
                    if (event.type == sf::Event::TextEntered) {

                        //if (event.key.code == sf::Keyboard::Escape) {

                        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Backspace)) {
                        //     Userpasswrd_string = Userpasswrd_string.substr(0, Userpasswrd_string.size() - 1);

                        // }

                        if (event.text.unicode == '\b') {
                            new_userPasswrd_string = new_userPasswrd_string.substr(0, new_userPasswrd_string.size() - 1);
                            new_userPasswrd.setString(new_userPasswrd_string);
                        }

                        // else if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Tab) {
                        //     //userPasswrd.setString(Userpasswrd_string);
                        //     //userName_pressed = false;
                        //     userPasswrd_pressed = false;
                        // }

                        else if (event.text.unicode == '\r') {
                            //userPasswrd.setString(Userpasswrd_string);
                            userName_pressed = false;
                            userPasswrd_pressed = false;
                            userEmail_pressed = true;
                            // MAYBE try to login as if the actual button was clicked
                        }

                        else if( event.text.unicode != '\t' && event.text.unicode < 128) {
                            new_userPasswrd_string += static_cast<char>(event.text.unicode);
                            new_userPasswrd.setString(new_userPasswrd_string);
                        }
                    }
                }

                else if (userEmail_pressed) {
                    if (event.type == sf::Event::TextEntered) {
                        if (event.text.unicode == '\b') {
                            new_userEmail_string = new_userEmail_string.substr(0, new_userEmail_string.size() - 1);
                            new_userEmail.setString(new_userEmail_string);
                        }

                        else if (event.text.unicode == '\r') {
                            userName_pressed = false;
                            userPasswrd_pressed = false;
                            userEmail_pressed = true;
                            // MAYBE try to login as if the actual login_button was clicked
                        }

                        else if( event.text.unicode != '\t' && event.text.unicode < 128) {
                            new_userEmail_string += static_cast<char>(event.text.unicode);
                            new_userEmail.setString(new_userEmail_string);
                        }
                    }
                }


                else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                    login_pressed = false;
                    signup_pressed = false;
                    userEmail_pressed = false;
                    cancelButton_pressed = false;
                    confirmButton_pressed = false;
                }

                if (event.type == sf::Event::Resized) {
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));

                    UserSignUp_rect.setPosition(event.size.width/2, event.size.height/2);
                    UserSignUp_text.setPosition(event.size.width/2, event.size.height/2 - 180);
                    SIGN_UP_underLine.setPosition(event.size.width/2, event.size.height/2 - 150);

                    SIGN_UP_userName_text.setPosition(event.size.width/2 - 160, event.size.height/2 - 100);
                    SIGN_UP_userName_rect.setPosition(event.size.width/2 + 70, event.size.height/2 - 90);
                    new_userName.setPosition(event.size.width/2 - 85, event.size.height/2 - 120); // --------------

                    SIGN_UP_userPasswrd_text.setPosition(event.size.width/ 2 - 160, event.size.width/ 2 - 25);
                    SIGN_UP_userPasswrd_rect.setPosition(event.size.width/ 2 + 70, event.size.width/ 2 - 15);
                    new_userPasswrd.setPosition(event.size.width/2 - 85, event.size.height/2 - 40);

                    SIGN_UP_userEmail_text.setPosition(event.size.width/ 2 - 160, event.size.width/ 2 + 55);
                    SIGN_UP_userEmail_rect.setPosition(event.size.width/ 2 + 70, event.size.width/ 2 + 65);
                    new_userEmail.setPosition(event.size.width/2 - 85, event.size.height/2 + 45);

                    SIGN_UP_cancelButton_unpressed_sprite.setPosition(event.size.width/2 - 100, event.size.height/2 + 140);
                    SIGN_UP_cancelButton_pressed_sprite.setPosition(event.size.width/2 - 100, event.size.height/2 + 140);

                    SIGN_UP_confirmButton_unpressed_sprite.setPosition(event.size.width/2 + 100, event.size.height/2 + 140);
                    SIGN_UP_confirmButton_pressed_sprite.setPosition(event.size.width/2 + 100, event.size.height/2 + 140);

                    wrong_SignUp_text.setPosition(event.size.width/2, event.size.height/2 + 125);
                    valid_SignUp_text.setPosition(event.size.width/2, event.size.height/2 + 125);
                }

                if(userPasswrd_pressed) {
                    SIGN_UP_userPasswrd_rect.setOutlineColor(sf::Color(0, 153, 255));
                }
                else {
                    SIGN_UP_userPasswrd_rect.setOutlineColor(sf::Color::Black);
                }
                if(userName_pressed) {
                    SIGN_UP_userName_rect.setOutlineColor(sf::Color(0, 153, 255));
                }
                else {
                    SIGN_UP_userName_rect.setOutlineColor(sf::Color::Black);
                }
            }
        }

        // Clear the whole texture with red color
        //rect_.clear(sf::Color::Red);
        // mouse_pos = sf::Vector2f(sf::Mouse::getPosition(window));
        //texture.display();

        window.clear(sf::Color::Green);

        if(current_state == user_login_STATE) {
            window.draw(LOGIN_Rect);

            window.draw(userName_text);
            window.draw(userName_rect);
            window.draw(userName);

            window.draw(userPasswrd_text);
            window.draw(userPasswrd_rect);
            window.draw(userPasswrd);

            if(signup_pressed == false) {
                window.draw(signup_unpressed_sprite);
            }
            else {
                invalid_login = false;
                valid_login = false;

                window.draw(signup_pressed_sprite);
                current_state = user_sign_up_STATE;
                // if(Username_string == "NEW USER" || Userpasswrd_string == "") {
                //     invalid_signup = true;
                // }
                // else {
                //     invalid_signup = false;
                //     valid_signup = true;
                // }

            }

            if(login_pressed == false) {
                window.draw(login_button_unpressed_sprite);
            }
            else {
                invalid_signup = false;
                valid_signup = false;
                window.draw(login_button_pressed_sprite);

                // TODO: ACTUAL LOGIN VERIFICATION SHOULD HAPPEN HERE after a small coded delay and start after login_button is actually unpresed.

                std::cout << "Username is: " << Username_string << "\n";
                std::cout << "User password is: " << Userpasswrd_string << "\n";
                if(Username_string != "Diego" || Userpasswrd_string != "123") {
                    invalid_login = true;
                    std::cout << "INVALID LOGIN TRY AGAIN" << "\n";
                }
                else {
                    invalid_login = false;
                    valid_login = true;

                    login_counter += 1;

                    std::cout << "Valid login... Loading table... " << login_counter << "\n";
                }
            }
            if(invalid_login) {
                window.draw(wrongLogin_text);
            }
            if(valid_login) {
                window.draw(validLogin_text);
                // IMPORTANT CHECK_login();...
            }
        }

        else if(current_state == user_sign_up_STATE) {
            window.draw(UserSignUp_rect);
            window.draw(UserSignUp_text);
            window.draw(SIGN_UP_underLine);

            window.draw(SIGN_UP_userName_text);
            window.draw(SIGN_UP_userName_rect);
            window.draw(new_userName);

            window.draw(SIGN_UP_userPasswrd_text);
            window.draw(SIGN_UP_userPasswrd_rect);
            window.draw(new_userPasswrd);

            window.draw(SIGN_UP_userEmail_text);
            window.draw(SIGN_UP_userEmail_rect);
            window.draw(new_userEmail);


            if(confirmButton_pressed == false) {
                window.draw(SIGN_UP_confirmButton_unpressed_sprite);
            }
            else {
                //invalid_login = false;
                //valid_login = false;

                window.draw(SIGN_UP_confirmButton_pressed_sprite);

                // TODO: CONFIRMATION OF VALID USER SIGN UP SHOULD HAPPEN HERE after a coded delay
                // and after confirm button is actually unpressed.

                // USE invalid_signup to check for edge cases (maybe?) like the one below.

                if(Username_string == "NEW USER" || Userpasswrd_string == "") {
                    invalid_signup = true;
                }
                else {
                    invalid_signup = false;
                    valid_signup = true;
                }

            }

            if(cancelButton_pressed == false) {
                //SIGN_UP_cancelButton_pressed_texture
                window.draw(SIGN_UP_cancelButton_unpressed_sprite);
            }
            else {
                invalid_signup = false;
                valid_signup = false;
                window.draw(SIGN_UP_cancelButton_pressed_sprite);
                //current_state = user_login_STATE;

                std::cout << "Username is: " << Username_string << "\n";
                std::cout << "User password is: " << Userpasswrd_string << "\n";
                if(Username_string != "Diego" || Userpasswrd_string != "123") {
                    invalid_login = true;
                    std::cout << "INVALID LOGIN TRY AGAIN" << "\n";
                }
                else {
                    invalid_login = false;
                    valid_login = true;

                    login_counter += 1;

                    std::cout << "Valid login... Loading table... " << login_counter << "\n";
                }
            }


            if (invalid_signup) {
                valid_signup = false;
                window.draw(wrong_SignUp_text);
            }
            if (valid_signup) {
                window.draw(valid_SignUp_text);
                invalid_signup = false;
                // SHOULD WE CARE ABOUT EMPTY PASSWORD BEING VALID? what about weird passwords?
                // IMPORTANT SIGNUP_login();...
            }

            //window.draw(SIGN_UP_cancelButton);
            //window.draw(SIGN_UP_confirmButton);
        }


        window.display();
    }

}
