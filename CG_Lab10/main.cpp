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

uniform int uMode; 
uniform vec3 uUniformColor;

void main() {
    if (uMode == 2) {
        FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    } 
    else if (uMode == 3) {
        FragColor = vec4(uUniformColor, 1.0f);
    } 
    else {
        FragColor = vec4(vColor, 1.0f);
    }
}
)";

void checkGLError(const char* label) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL Error at " << label << ": " << err << std::endl;
    }
}

GLuint CreateShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

void hsvToRgb(float h, float s, float v, float& r, float& g, float& b) {
    h = fmod(h * 360.0f, 360.0f);
    float c = v * s;
    float x = c * (1.0f - fabs(fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;

    if (h >= 0 && h < 60) { r = c; g = x; b = 0; }
    else if (h >= 60 && h < 120) { r = x; g = c; b = 0; }
    else if (h >= 120 && h < 180) { r = 0; g = c; b = x; }
    else if (h >= 180 && h < 240) { r = 0; g = x; b = c; }
    else if (h >= 240 && h < 300) { r = x; g = 0; b = c; }
    else { r = c; g = 0; b = x; }
    r += m; g += m; b += m;
}

int main() {
    sf::ContextSettings settings;
    settings.depthBits = 24;

    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "OpenGL Tasks", sf::Style::Default, sf::State::Windowed, settings);
    window.setVerticalSyncEnabled(true);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        return -1;
    }
    glGetError();

    GLuint vShader = CreateShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fShader = CreateShader(GL_FRAGMENT_SHADER, fragmentSource);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, fShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    GLint uModeLoc = glGetUniformLocation(shaderProgram, "uMode");
    GLint uColorLoc = glGetUniformLocation(shaderProgram, "uUniformColor");

    float quadVertices[] = {
        -0.9f, 0.8f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.8f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.4f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.9f, 0.4f, 0.0f, 1.0f, 1.0f, 0.0f
    };

    GLuint vaoQuad, vboQuad;
    glGenVertexArrays(1, &vaoQuad);
    glGenBuffers(1, &vboQuad);
    glBindVertexArray(vaoQuad);
    glBindBuffer(GL_ARRAY_BUFFER, vboQuad);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float fanVertices[] = {
        0.0f,  0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        0.2f,  0.2f, 0.0f,   1.0f, 0.0f, 0.0f,
        0.1f,  0.3f, 0.0f,   1.0f, 0.5f, 0.0f,
        0.0f,  0.35f,0.0f,   1.0f, 1.0f, 0.0f,
        -0.1f, 0.3f, 0.0f,   0.0f, 1.0f, 0.0f,
        -0.2f, 0.2f, 0.0f,   0.0f, 0.0f, 1.0f
    };

    GLuint vaoFan, vboFan;
    glGenVertexArrays(1, &vaoFan);
    glGenBuffers(1, &vboFan);
    glBindVertexArray(vaoFan);
    glBindBuffer(GL_ARRAY_BUFFER, vboFan);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fanVertices), fanVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    std::vector<float> pentagonVertices;
    float centerX = 0.5f, centerY = -0.5f, radius = 0.3f;
    pentagonVertices.insert(pentagonVertices.end(), { centerX, centerY, 0.0f, 1.0f, 1.0f, 1.0f });

    for (int i = 0; i <= 5; ++i) {
        float angle = i * 2.0f * PI / 5.0f + (PI / 2);
        float h = (float)i / 5.0f;
        float r, g, b;
        hsvToRgb(h, 1.0f, 1.0f, r, g, b);

        pentagonVertices.push_back(centerX + radius * cos(angle));
        pentagonVertices.push_back(centerY + radius * sin(angle));
        pentagonVertices.push_back(0.0f);
        pentagonVertices.push_back(r); pentagonVertices.push_back(g); pentagonVertices.push_back(b);
    }

    GLuint vaoPent, vboPent;
    glGenVertexArrays(1, &vaoPent);
    glGenBuffers(1, &vboPent);
    glBindVertexArray(vaoPent);
    glBindBuffer(GL_ARRAY_BUFFER, vboPent);
    glBufferData(GL_ARRAY_BUFFER, pentagonVertices.size() * sizeof(float), pentagonVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    int currentTask = 4;

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>() ||
                (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)) {
                window.close();
            }
            if (event->is<sf::Event::KeyPressed>()) {
                sf::Keyboard::Key key = event->getIf<sf::Event::KeyPressed>()->code;
                if (key == sf::Keyboard::Key::Num1) currentTask = 1;
                if (key == sf::Keyboard::Key::Num2) currentTask = 2;
                if (key == sf::Keyboard::Key::Num3) currentTask = 3;
                if (key == sf::Keyboard::Key::Num4) currentTask = 4;
            }
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        int modeToSend = 0;
        if (currentTask == 2) modeToSend = 2;
        else if (currentTask == 3) modeToSend = 3;
        else modeToSend = 0;

        glUniform1i(uModeLoc, modeToSend);

        if (currentTask == 3) glUniform3f(uColorLoc, 1.0f, 0.0f, 1.0f);

        glBindVertexArray(vaoQuad);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glBindVertexArray(vaoFan);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

        glBindVertexArray(vaoPent);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 7);

        window.display();
    }

    return 0;
}