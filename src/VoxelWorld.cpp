#include <Engine.hpp>
#include <VoxelWorld.hpp>
#include <generated/Shaders.hpp>
#include <iostream>
#include <set>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static const GLfloat cubeBufferData[] = {
    0.0f, 0.0f, 0.0f,  // Vertex 0
    0.0f, 0.0f, 1.0f,  // Vertex 1
    0.0f, 1.0f, 0.0f,  // Vertex 2

    0.0f, 0.0f, 1.0f,  // Vertex 1
    0.0f, 1.0f, 0.0f,  // Vertex 2
    0.0f, 1.0f, 1.0f,  // Vertex 3

    1.0f, 0.0f, 0.0f,  // Vertex 4
    1.0f, 0.0f, 1.0f,  // Vertex 5
    1.0f, 1.0f, 0.0f,  // Vertex 6

    1.0f, 0.0f, 1.0f,  // Vertex 5
    1.0f, 1.0f, 0.0f,  // Vertex 6
    1.0f, 1.0f, 1.0f,  // Vertex 7

    0.0f, 0.0f, 0.0f,  // Vertex 0
    0.0f, 0.0f, 1.0f,  // Vertex 1
    1.0f, 0.0f, 0.0f,  // Vertex 4

    0.0f, 0.0f, 1.0f,  // Vertex 1
    1.0f, 0.0f, 0.0f,  // Vertex 4
    1.0f, 0.0f, 1.0f,  // Vertex 5

    0.0f, 1.0f, 0.0f,  // Vertex 2
    0.0f, 1.0f, 1.0f,  // Vertex 3
    1.0f, 1.0f, 0.0f,  // Vertex 6

    0.0f, 1.0f, 1.0f,  // Vertex 3
    1.0f, 1.0f, 0.0f,  // Vertex 6
    1.0f, 1.0f, 1.0f,  // Vertex 7

    0.0f, 0.0f, 0.0f,  // Vertex 0
    0.0f, 1.0f, 0.0f,  // Vertex 2
    1.0f, 0.0f, 0.0f,  // Vertex 4

    0.0f, 1.0f, 0.0f,  // Vertex 2
    1.0f, 0.0f, 0.0f,  // Vertex 4
    1.0f, 1.0f, 0.0f,  // Vertex 6

    0.0f, 0.0f, 1.0f,  // Vertex 1
    0.0f, 1.0f, 1.0f,  // Vertex 3
    1.0f, 0.0f, 1.0f,  // Vertex 5

    0.0f, 1.0f, 1.0f,  // Vertex 3
    1.0f, 0.0f, 1.0f,  // Vertex 5
    1.0f, 1.0f, 1.0f   // Vertex 7
};

static const GLfloat quadBufferData[] = {-1.f, -1.f, 0.f, 1.f, 1.f, 0.f, 1.f,  -1.f, 0.f,
                                         -1.f, 1.f,  0.f, 1.f, 1.f, 0.f, -1.f, -1.f, 0.f};

VoxelWorld::VoxelWorld(Engine *engine) : engine(engine), worldTexture(NO_TEXTURE) {}

void VoxelWorld::init() {
    // Create OpenGL program
    voxelProgram = glCreateProgram();
    auto [vertexShader, vertexShaderStatus] = createShader(GL_VERTEX_SHADER, VOXEL_VERTEX_SHADER_SRC);
    auto [fragmentShader, fragmentShaderStatus] = createShader(GL_FRAGMENT_SHADER, VOXEL_FRAGMENT_SHADER_SRC);
    glAttachShader(voxelProgram, vertexShader);
    glAttachShader(voxelProgram, fragmentShader);
    glLinkProgram(voxelProgram);

    sunlightProgram = glCreateProgram();
    auto [sunlightVertexShader, sunlightVertexShaderStatus] =
        createShader(GL_VERTEX_SHADER, SUNLIGHT_VERTEX_SHADER_SRC);
    auto [sunlightFragmentShader, sunlightFragmentShaderStatus] =
        createShader(GL_FRAGMENT_SHADER, SUNLIGHT_FRAGMENT_SHADER_SRC);
    glAttachShader(sunlightProgram, sunlightVertexShader);
    glAttachShader(sunlightProgram, sunlightFragmentShader);
    glLinkProgram(sunlightProgram);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeBufferData), cubeBufferData, GL_STATIC_DRAW);

    glGenBuffers(1, &quadVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadBufferData), quadBufferData, GL_STATIC_DRAW);

    glGenTextures(1, &paletteTexture);
    glBindTexture(GL_TEXTURE_2D, paletteTexture);
    // set the texture wrapping/filtering options (on the currently bound
    // texture object) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
    // GL_REPEAT); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load("./palette.png", &width, &height, &nrChannels, 0);
    std::cout << width << " " << height << " " << nrChannels << std::endl;
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenFramebuffers(1, &depthFb);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFb);

    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    auto [windowWidth, windowHeight] = getEngine()->getWindowSize();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &normalTexture);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB_SNORM, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // GLuint depthTexture2;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
    // GL_TEXTURE_2D, depthTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, depthTexture, 0);

    // finally check if framebuffer is complete
    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) std::cout << "Framebuffer not complete! " << status << std::endl;

    GLint no_of_extensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &no_of_extensions);

    std::set<std::string> ogl_extensions;
    for (int i = 0; i < no_of_extensions; ++i) ogl_extensions.insert((const char *)glGetStringi(GL_EXTENSIONS, i));
    bool texture_storage = ogl_extensions.find("GL_ARB_texture_barrier") != ogl_extensions.end();
    if (texture_storage) {
        std::cout << "WE HAVE IT!!!\n";
    }

    camera.setPosition(glm::vec3(0, 0, 0));
    camera.setDirection(glm::vec3(1, 1, 1));
    engine->setCamera(&camera);
}

void VoxelWorld::update(float deltaTime) {}

void VoxelWorld::render(glm::mat4 const &viewProjectionMatrix) {
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFb);
    GLenum attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);

    glClear(GL_COLOR_BUFFER_BIT);
    float depth = 1.0f;
    glClearTexImage(depthTexture, 0, GL_RGB, GL_FLOAT, &depth);

    glUseProgram(voxelProgram);

    GLuint viewProjectionMatrixID = glGetUniformLocation(voxelProgram, "uViewProjectionMatrix");
    GLuint viewProjectionInvMatrixID = glGetUniformLocation(voxelProgram, "uViewProjectionInvMatrix");

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0,  // attribute 0. No particular reason for 0, but
                              // must match the layout in the shader.
                          3,         // size
                          GL_FLOAT,  // type
                          GL_FALSE,  // normalized?
                          0,         // stride
                          (void *)0  // array buffer offset
    );

    glUniform1i(glGetUniformLocation(voxelProgram, "uChunkTexture"), 0);
    glUniform1i(glGetUniformLocation(voxelProgram, "uPaletteTexture"), 1);
    glUniform1i(glGetUniformLocation(voxelProgram, "uDepthTexture"), 2);
    glUniform1i(glGetUniformLocation(voxelProgram, "uWorldTexture"), 3);

    auto viewProjection = viewProjectionMatrix;
    auto viewProjectionInv = glm::inverse(viewProjection);
    glUniformMatrix4fv(viewProjectionMatrixID, 1, GL_FALSE, &viewProjection[0][0]);
    glUniformMatrix4fv(viewProjectionInvMatrixID, 1, GL_FALSE, &viewProjectionInv[0][0]);

    auto [windowWidth, windowHeight] = getEngine()->getWindowSize();
    glUniform2f(glGetUniformLocation(voxelProgram, "invResolution"), 1.f / windowWidth, 1.f / windowHeight);
    // glUniform3f(glGetUniformLocation(voxelProgram, "uSunPos"), sunPosition.x, sunPosition.y, sunPosition.z);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, paletteTexture);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_3D, worldTexture);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_3D, normalTexture);

    for (auto const &voxelObject : voxelObjects) {
        voxelObject.draw(voxelProgram, viewProjection);
        glTextureBarrier();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(sunlightProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, worldTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, colorTexture);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, normalTexture);

    glUniform1i(glGetUniformLocation(sunlightProgram, "uWorldTexture"), 0);
    glUniform1i(glGetUniformLocation(sunlightProgram, "uAlbedoTexture"), 1);
    glUniform1i(glGetUniformLocation(sunlightProgram, "uDepthTexture"), 2);
    glUniform1i(glGetUniformLocation(sunlightProgram, "uNormalTexture"), 3);

    auto mm = glm::scale(glm::mat4(1.f), glm::vec3(8)) * viewProjectionInv;
    glUniform2f(glGetUniformLocation(sunlightProgram, "invResolution"), 1.f / windowWidth, 1.f / windowHeight);
    glUniformMatrix4fv(glGetUniformLocation(sunlightProgram, "uMagicMatrix"), 1, GL_FALSE, &mm[0][0]);
    // glUniform3f(glGetUniformLocation(sunlightProgram, "uSunPos"), sunPosition.x, sunPosition.y, sunPosition.z);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
    glVertexAttribPointer(0,  // attribute 0. No particular reason for 0, but
                              // must match the layout in the shader.
                          3,         // size
                          GL_FLOAT,  // type
                          GL_FALSE,  // normalized?
                          0,         // stride
                          (void *)0  // array buffer offset
    );
    glDrawArrays(GL_TRIANGLES, 0, 6);  // 3 indices starting at 0 -> 1 triangle
}

VoxelObject *VoxelWorld::spawnVoxelObject() {
    voxelObjects.emplace_back();
    return &voxelObjects.back();
}

Engine *VoxelWorld::getEngine() const { return engine; }