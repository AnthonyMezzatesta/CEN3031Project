#include "test_framework.h"
#include "../include/UserManager.h"
#include <filesystem>

void test_user_manager_creation() {
    UserManager um("test_users.db");
    
    ASSERT_TRUE(um.initialize());
    ASSERT_TRUE(um.isConnected());
    ASSERT_FALSE(um.isLoggedIn());
    
    um.close();
}

void test_user_registration() {
    UserManager um("test_users.db");
    um.initialize();
    
    // Clear any existing users
    um.clearAllUsers();
    
    ASSERT_TRUE(um.registerUser("test@example.com", "password123"));
    ASSERT_EQ(1, um.getUserCount());
    
    // Should not be able to register same email twice
    ASSERT_FALSE(um.registerUser("test@example.com", "password456"));
    
    um.close();
}

void test_user_login() {
    UserManager um("test_users.db");
    um.initialize();
    
    um.clearAllUsers();
    um.registerUser("test@example.com", "password123");
    
    // Test successful login
    ASSERT_TRUE(um.loginUser("test@example.com", "password123"));
    ASSERT_TRUE(um.isLoggedIn());
    
    auto current_user = um.getCurrentUser();
    ASSERT_TRUE(current_user.has_value());
    ASSERT_STR_EQ("test@example.com", current_user.value().getEmail());
    
    // Test failed login
    um.logoutUser();
    ASSERT_FALSE(um.loginUser("test@example.com", "wrongpassword"));
    ASSERT_FALSE(um.isLoggedIn());
    
    um.close();
}

void test_user_logout() {
    UserManager um("test_users.db");
    um.initialize();
    
    um.clearAllUsers();
    um.registerUser("test@example.com", "password123");
    um.loginUser("test@example.com", "password123");
    
    ASSERT_TRUE(um.isLoggedIn());
    
    um.logoutUser();
    ASSERT_FALSE(um.isLoggedIn());
    ASSERT_FALSE(um.getCurrentUser().has_value());
    
    um.close();
}

void test_get_user_by_email() {
    UserManager um("test_users.db");
    um.initialize();
    
    um.clearAllUsers();
    um.registerUser("test@example.com", "password123");
    
    auto user = um.getUserByEmail("test@example.com");
    ASSERT_TRUE(user.has_value());
    ASSERT_STR_EQ("test@example.com", user.value().getEmail());
    
    auto missing_user = um.getUserByEmail("nonexistent@example.com");
    ASSERT_FALSE(missing_user.has_value());
    
    um.close();
}

void test_email_exists() {
    UserManager um("test_users.db");
    um.initialize();
    
    um.clearAllUsers();
    um.registerUser("test@example.com", "password123");
    
    ASSERT_TRUE(um.emailExists("test@example.com"));
    ASSERT_FALSE(um.emailExists("nonexistent@example.com"));
    
    um.close();
}

void test_user_count() {
    UserManager um("test_users.db");
    um.initialize();
    
    um.clearAllUsers();
    ASSERT_EQ(0, um.getUserCount());
    
    um.registerUser("test1@example.com", "password123");
    ASSERT_EQ(1, um.getUserCount());
    
    um.registerUser("test2@example.com", "password123");
    ASSERT_EQ(2, um.getUserCount());
    
    um.close();
}

void cleanup_user_test_files() {
    // Clean up test database files
    std::filesystem::remove("test_users.db");
}

void run_user_manager_tests() {
    std::cout << "\n=== Running UserManager Tests ===" << std::endl;
    
    RUN_TEST(test_user_manager_creation);
    RUN_TEST(test_user_registration);
    RUN_TEST(test_user_login);
    RUN_TEST(test_user_logout);
    RUN_TEST(test_get_user_by_email);
    RUN_TEST(test_email_exists);
    RUN_TEST(test_user_count);
    
    cleanup_user_test_files();
} 