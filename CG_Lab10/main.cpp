#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <iostream>
#include <vector>
#include <optional>

const char* vertexSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main() { gl_Position = vec4(aPos, 1.0); }
)";

const char* fragmentSource = R"(
#version 330 core
out vec4 FragColor;
void main() { FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f); }
)";

void CheckCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n";
    }
}

void CheckLinkErrors(GLuint program) {
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << "\n";
    }
}

int main() {
    sf::ContextSettings settings;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.attributeFlags = sf::ContextSettings::Core;

    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Task: Init Shaders", sf::Style::Default, sf::State::Windowed, settings);
    window.setVerticalSyncEnabled(true);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return -1;

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);    
    glShaderSource(vertexShader, 1, &vertexSource, NULL);        
    glCompileShader(vertexShader);                               
    CheckCompileErrors(vertexShader, "VERTEX");                  

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);  
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);    
    glCompileShader(fragmentShader);                             
    CheckCompileErrors(fragmentShader, "FRAGMENT");              

    GLuint shaderProgram = glCreateProgram();                    
    glAttachShader(shaderProgram, vertexShader);                 
    glAttachShader(shaderProgram, fragmentShader);               
    glLinkProgram(shaderProgram);                                
    CheckLinkErrors(shaderProgram);                              

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLfloat vertices[] = { -0.5f, -0.5f, 0.0f,  0.5f, -0.5f, 0.0f,  0.0f,  0.5f, 0.0f };
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>() ||
                (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)) {
                window.close();
            }
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        window.display();
    }

    return 0;
}