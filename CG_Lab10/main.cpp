#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <optional>

const float PI = 3.14159265359f;

const char* vertexSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

const char* fragmentSource = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f); // Зеленый цвет
}
)";

void CheckErrors(const std::string& type) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL Error (" << type << "): 0x" << std::hex << err << std::dec << std::endl;
    }
}

GLuint CreateShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    return shader;
}

int main() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.attributeFlags = sf::ContextSettings::Core;

    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Quad, Fan, Pentagon", sf::Style::Default, sf::State::Windowed, settings);
    window.setVerticalSyncEnabled(true);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return -1;
    while (glGetError() != GL_NO_ERROR);

    GLuint vShader = CreateShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fShader = CreateShader(GL_FRAGMENT_SHADER, fragmentSource);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, fShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    GLint attribLocation = glGetAttribLocation(shaderProgram, "aPos");

    float quadVertices[] = {
        -0.8f,  0.8f, 0.0f,
         -0.4f,  0.8f, 0.0f,
         -0.4f,  0.4f, 0.0f,
        -0.8f,  0.4f, 0.0f 
    };

    GLuint vaoQuad, vboQuad;
    glGenVertexArrays(1, &vaoQuad);
    glGenBuffers(1, &vboQuad);

    glBindVertexArray(vaoQuad);
    glBindBuffer(GL_ARRAY_BUFFER, vboQuad);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(attribLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(attribLocation);

    float fanVertices[] = {
         0.0f,  0.0f, 0.0f, 
         0.2f,  0.2f, 0.0f, 
         0.15f, 0.3f, 0.0f, 
         0.0f,  0.35f, 0.0f, 
        -0.15f, 0.3f, 0.0f, 
        -0.2f,  0.2f, 0.0f  
    };

    GLuint vaoFan, vboFan;
    glGenVertexArrays(1, &vaoFan);
    glGenBuffers(1, &vboFan);

    glBindVertexArray(vaoFan);
    glBindBuffer(GL_ARRAY_BUFFER, vboFan);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fanVertices), fanVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(attribLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(attribLocation);

    std::vector<float> pentagonVertices;
    float centerX = 0.5f;
    float centerY = -0.5f;
    float radius = 0.3f;


    for (int i = 0; i < 5; ++i) {
        float angle = i * 2.0f * PI / 5.0f + (PI / 2);
        pentagonVertices.push_back(centerX + radius * cos(angle)); 
        pentagonVertices.push_back(centerY + radius * sin(angle)); 
        pentagonVertices.push_back(0.0f);                          
    }

    GLuint vaoPent, vboPent;
    glGenVertexArrays(1, &vaoPent);
    glGenBuffers(1, &vboPent);

    glBindVertexArray(vaoPent);
    glBindBuffer(GL_ARRAY_BUFFER, vboPent);
    glBufferData(GL_ARRAY_BUFFER, pentagonVertices.size() * sizeof(float), pentagonVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(attribLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(attribLocation);


    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>() ||
                (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)) {
                window.close();
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glBindVertexArray(vaoQuad);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glBindVertexArray(vaoFan);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

        glBindVertexArray(vaoPent);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 5);

        window.display();
    }

    glDeleteVertexArrays(1, &vaoQuad);
    glDeleteVertexArrays(1, &vaoFan);
    glDeleteVertexArrays(1, &vaoPent);
    glDeleteBuffers(1, &vboQuad);
    glDeleteBuffers(1, &vboFan);
    glDeleteBuffers(1, &vboPent);
    glDeleteProgram(shaderProgram);

    return 0;
}