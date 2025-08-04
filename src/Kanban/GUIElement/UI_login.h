// #ifndef UI_LOGIN_H
// #define UI_LOGIN_H

// class UI_login
// {
// public:
//     UI_login();
// };

// #endif // UI_LOGIN_H

// #ifndef UI_LOGIN_H
// #define UI_LOGIN_H

// class UI_login
// {
// public:
//     UI_login();
// };

// #endif // UI_LOGIN_H


//====================================================================================

#ifndef UI_LOGIN_H
#define UI_LOGIN_H

#include <SFML/Graphics.hpp>

inline sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML window");
inline sf::Font font;// (sf::VideoMode(1280, 720), "SFML window");

enum UI_state {
    user_login_STATE,
    user_sign_up_STATE,
    user_profile_STATE
};

//=============================================================================================================

inline sf::RectangleShape LOGIN_Rect;
inline sf::RectangleShape userName_rect;
inline sf::RectangleShape userPasswrd_rect;


inline sf::Text userName_text;// ("Username:", font);
inline sf::Text userPasswrd_text;

inline std::string Username_string; //= "";
inline std::string Userpasswrd_string; // = "";
inline std::string UserEmail_string; // = "";

inline sf::Text userPasswrd; //(Userpasswrd_string, font);
inline sf::Text userName; //(Username_string, font);

inline sf::Texture login_button_unpressed_texture;
inline sf::Sprite login_button_unpressed_sprite;

inline sf::Texture login_button_pressed_texture;
inline sf::Sprite login_button_pressed_sprite;

inline sf::Text wrongLogin_text;
inline sf::Text validLogin_text;

inline sf::Texture signup_unpressed_texture;
inline sf::Sprite signup_unpressed_sprite;

inline sf::Texture signup_pressed_texture;
inline sf::Sprite signup_pressed_sprite;

inline sf::Text wrong_SignUp_text;
inline sf::Text valid_SignUp_text; // ("Valid sign up. Loading Table.", font);

void SET_LOGIN();

//=============================================================================================================
//  USER PROFILE
//extern sf::RectangleShape UserProfile_rect;
//extern sf::Text UserProfileText(" User Profile", font);
inline sf::RectangleShape _underLine;
//extern sf::Text PROFILE_usernameText("Username: ", font); // maybe reuse userName_text since they are the same
//extern sf::Text PROFILE_username(Username_string, font); // maybe reuse userName since they are the same
//extern sf::Text PROFILE_userPasswrd_text("Password:", font); // maybe reuse userPasswrd_text since they are the same
//extern sf::Text PROFILE_userPasswrd_covered("", font);
//extern sf::Text PROFILE_userEmail(UserEmail_string, font);

//=============================================================================================================
//  USER SIGN UP

inline sf::RectangleShape UserSignUp_rect;
inline sf::Text UserSignUp_text;
inline sf::RectangleShape SIGN_UP_underLine;
// reuse underLine;

inline std::string new_userPasswrd_string; // = "";
inline std::string new_username_string; // = "";
inline std::string new_userEmail_string; // = "";

inline sf::Text SIGN_UP_userName_text;
inline sf::RectangleShape SIGN_UP_userName_rect;
inline sf::Text new_userName;

inline sf::Text SIGN_UP_userPasswrd_text;
inline sf::RectangleShape SIGN_UP_userPasswrd_rect;
inline sf::Text new_userPasswrd;

inline sf::Text SIGN_UP_userEmail_text;
inline sf::RectangleShape SIGN_UP_userEmail_rect;
inline sf::Text new_userEmail;

inline sf::Texture SIGN_UP_confirmButton_unpressed_texture;
inline sf::Sprite SIGN_UP_confirmButton_unpressed_sprite;

inline sf::Texture SIGN_UP_confirmButton_pressed_texture;
inline sf::Sprite SIGN_UP_confirmButton_pressed_sprite;

inline sf::Texture SIGN_UP_cancelButton_unpressed_texture;
inline sf::Sprite SIGN_UP_cancelButton_unpressed_sprite;

inline sf::Texture SIGN_UP_cancelButton_pressed_texture;
inline sf::Sprite SIGN_UP_cancelButton_pressed_sprite;

void SET_SIGNUP();

//=============================================================================================================
//=============================================================================================================
//=============================================================================================================

// class random {
// public:
//     sf::RectangleShape random_rect;
//     void rectangle_fill_color(sf::RectangleShape& r_);
// };



#endif // UI_LOGIN_H
