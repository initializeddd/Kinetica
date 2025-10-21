#include <kinetica/rendering.hpp>
#include <kinetica/window.hpp>
#include <kinetica/log.hpp>

#include <kinetica/ecs/registry.hpp>

#include <kinetica/ecs/components/transform.hpp>
#include <kinetica/ecs/components/material.hpp>
#include <kinetica/ecs/components/mesh.hpp>

#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

static std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        KLOG_ERROR(("Failed to open shader file: " + path).c_str());
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static GLuint compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        KLOG_ERROR((std::string("Shader compilation failed: ") + infoLog).c_str());
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static GLuint createProgram(const char* vertSrc, const char* fragSrc) {
    GLuint vert = compileShader(vertSrc, GL_VERTEX_SHADER);
    GLuint frag = compileShader(fragSrc, GL_FRAGMENT_SHADER);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);

    GLint success;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(prog, 512, nullptr, infoLog);
        KLOG_ERROR((std::string("Shader linking failed: ") + infoLog).c_str());
        glDeleteProgram(prog);
        prog = 0;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
    return prog;
}

namespace Kinetica {

    CRenderer::CRenderer(const Kinetica::CWindow& window) {

        glfwMakeContextCurrent(window.m_pWindow.get());

        if (!glGetString(GL_VERSION)) {
            KLOG_ERROR("No OpenGL context active!");
            m_bValid = false;
            return;
        }
        std::cout << "OpenGL version: " << std::string((const char*)glGetString(GL_VERSION)) << "\n";

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        std::string vertSource = readFile("../shader/basic.vert");
        std::string fragSource = readFile("../shader/basic.frag");

        if (vertSource.empty() || fragSource.empty()) {
            KLOG_ERROR("Shader source loading failed!");
            m_bValid = false;
            return;
        }

        m_shaderProgram = createProgram(vertSource.c_str(), fragSource.c_str());
        if (!m_shaderProgram) {
            KLOG_ERROR("Failed to create shader program!");
            m_bValid = false;
            return;
        }

        m_uModelLoc = glGetUniformLocation(m_shaderProgram, "uModel");
        m_uBaseColorLoc = glGetUniformLocation(m_shaderProgram, "uBaseColor");
        m_uMetallicLoc = glGetUniformLocation(m_shaderProgram, "uMetallic");
        m_uRoughnessLoc = glGetUniformLocation(m_shaderProgram, "uRoughness");
        m_uUseVertexColorLoc = glGetUniformLocation(m_shaderProgram, "uUseVertexColor");

        m_bValid = true;
    }

    CRenderer::~CRenderer() {
        // Nothing to destroy — OpenGL state is tied to context
    }

    void CRenderer::clear() {
        if (!m_bValid) return;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void CRenderer::setViewProjection(const glm::mat4& view, const glm::mat4& proj) {
        glUseProgram(m_shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uView"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uProjection"), 1, GL_FALSE, &proj[0][0]);
    }

    void CRenderer::present() {
        // This method is a placeholder for future renderer-side post-processing.
    }

    void CRenderer::uploadMesh(Kinetica::Components::SMesh& mesh) {
        if (mesh.vao == 0) {
            glGenVertexArrays(1, &mesh.vao);
            glGenBuffers(1, &mesh.vbo);
            glGenBuffers(1, &mesh.ebo);
        }

        glBindVertexArray(mesh.vao);

        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     mesh.vertices.size() * sizeof(Kinetica::Components::SVertex),
                     mesh.vertices.data(),
                     GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     mesh.indices.size() * sizeof(Kinetica::Components::SIndex),
                     mesh.indices.data(),
                     GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Kinetica::Components::SVertex), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Kinetica::Components::SVertex), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Kinetica::Components::SVertex), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
        mesh.isDirty = false;
    }

    void CRenderer::renderEntity(const Components::STransform& transform,
                                 const Components::SMesh& mesh,
                                 const Components::SMaterial& material) {
        if (!m_bValid || mesh.vao == 0) return;

        glUseProgram(m_shaderProgram);

        glUniformMatrix4fv(m_uModelLoc, 1, GL_FALSE, &transform.getMatrix()[0][0]);
        glUniform3fv(m_uBaseColorLoc, 1, &material.baseColor[0]);
        glUniform1f(m_uMetallicLoc, material.metallic);
        glUniform1f(m_uRoughnessLoc, material.roughness);
        glUniform1i(m_uUseVertexColorLoc, material.useVertexColor ? 1 : 0);

        glBindVertexArray(mesh.vao);
        if (mesh.indexCount() > 0) {
            glDrawElements(GL_TRIANGLES, mesh.indexCount(), GL_UNSIGNED_INT, 0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount());
        }
        glBindVertexArray(0);
    }

} // namespace Kinetica
