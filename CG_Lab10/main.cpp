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
layout (location = 1) in vec3 aColor;

out vec3 vColor;

void main() {
    gl_Position = vec4(aPos, 1.0f);
    vColor = aColor;
}
)";

const char* fragmentSource = R"(
#version 330 core
in vec3 vColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(vColor, 1.0f);
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

void hsvToRgb(float h, float s, float v, float& r, float& g, float& b) {
    h = fmod(h * 360.0f, 360.0f);

    float c = v * s;
    float x = c * (1.0f - fabs(fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;

    if (h >= 0.0f && h < 60.0f) {
        r = c; g = x; b = 0.0f;
    }
    else if (h >= 60.0f && h < 120.0f) {
        r = x; g = c; b = 0.0f;
    }
    else if (h >= 120.0f && h < 180.0f) {
        r = 0.0f; g = c; b = x;
    }
    else if (h >= 180.0f && h < 240.0f) {
        r = 0.0f; g = x; b = c;
    }
    else if (h >= 240.0f && h < 300.0f) {
        r = x; g = 0.0f; b = c;
    }
    else {
        r = c; g = 0.0f; b = x;
    }

    r += m;
    g += m;
    b += m;
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
        -0.8f,  0.8f, 0.0f,   1,0,0,
        -0.4f,  0.8f, 0.0f,   0,1,0,
        -0.4f,  0.4f, 0.0f,   0,0,1,
        -0.8f,  0.4f, 0.0f,   1,1,0
    };

    GLuint vaoQuad, vboQuad;
    glGenVertexArrays(1, &vaoQuad);
    glGenBuffers(1, &vboQuad);

    glBindVertexArray(vaoQuad);
    glBindBuffer(GL_ARRAY_BUFFER, vboQuad);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(attribLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(attribLocation);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float fanVertices[] = {
        0.0f,  0.0f, 0.0f,    1,1,1,
        0.2f,  0.2f, 0.0f,    1,0,0,
        0.15f, 0.3f, 0.0f,    1,0.5,0,
        0.0f,  0.35f,0.0f,    1,1,0,
        -0.15f,0.3f, 0.0f,    0,1,0,
        -0.2f, 0.2f, 0.0f,    0,0,1
    };

    GLuint vaoFan, vboFan;
    glGenVertexArrays(1, &vaoFan);
    glGenBuffers(1, &vboFan);

    glBindVertexArray(vaoFan);
    glBindBuffer(GL_ARRAY_BUFFER, vboFan);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fanVertices), fanVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(attribLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(attribLocation);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    std::vector<float> pentagonVertices;
    float centerX = 0.5f;
    float centerY = -0.5f;
    float radius = 0.3f;


    for (int i = 0; i < 5; ++i) {
        float angle = i * 2.0f * PI / 5.0f + (PI / 2);
        pentagonVertices.push_back(centerX + radius * cos(angle)); 
        pentagonVertices.push_back(centerY + radius * sin(angle)); 
        pentagonVertices.push_back(0.0f);

        float hue = (float)i / 5.0f;  // Hue меняется от 0 до 1
        float saturation = 1.0f;
        float value = 1.0f;

        // Преобразуем HSV в RGB
        float r, g, b;
        hsvToRgb(hue, saturation, value, r, g, b);
        pentagonVertices.push_back(r);
        pentagonVertices.push_back(g);
        pentagonVertices.push_back(b);
    }



    GLuint vaoPent, vboPent;
    glGenVertexArrays(1, &vaoPent);
    glGenBuffers(1, &vboPent);

    glBindVertexArray(vaoPent);
    glBindBuffer(GL_ARRAY_BUFFER, vboPent);
    glBufferData(GL_ARRAY_BUFFER, pentagonVertices.size() * sizeof(float), pentagonVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(attribLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(attribLocation);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


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