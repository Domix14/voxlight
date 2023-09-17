#include <VoxelSystem.hpp>
#include <utils.hpp>
#include <Engine.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#include <set>
#include <string>
// cube vertices with size 1 and origin in 0, 0, 0

static const GLfloat cubeBufferData[] = {
    0.0f, 0.0f, 0.0f, // Vertex 0
    0.0f, 0.0f, 1.0f, // Vertex 1
    0.0f, 1.0f, 0.0f, // Vertex 2

    0.0f, 0.0f, 1.0f, // Vertex 1
    0.0f, 1.0f, 0.0f, // Vertex 2
    0.0f, 1.0f, 1.0f, // Vertex 3

    1.0f, 0.0f, 0.0f, // Vertex 4
    1.0f, 0.0f, 1.0f, // Vertex 5
    1.0f, 1.0f, 0.0f, // Vertex 6

    1.0f, 0.0f, 1.0f, // Vertex 5
    1.0f, 1.0f, 0.0f, // Vertex 6
    1.0f, 1.0f, 1.0f, // Vertex 7

    0.0f, 0.0f, 0.0f, // Vertex 0
    0.0f, 0.0f, 1.0f, // Vertex 1
    1.0f, 0.0f, 0.0f, // Vertex 4

    0.0f, 0.0f, 1.0f, // Vertex 1
    1.0f, 0.0f, 0.0f, // Vertex 4
    1.0f, 0.0f, 1.0f, // Vertex 5

    0.0f, 1.0f, 0.0f, // Vertex 2
    0.0f, 1.0f, 1.0f, // Vertex 3
    1.0f, 1.0f, 0.0f, // Vertex 6

    0.0f, 1.0f, 1.0f, // Vertex 3
    1.0f, 1.0f, 0.0f, // Vertex 6
    1.0f, 1.0f, 1.0f, // Vertex 7

    0.0f, 0.0f, 0.0f, // Vertex 0
    0.0f, 1.0f, 0.0f, // Vertex 2
    1.0f, 0.0f, 0.0f, // Vertex 4

    0.0f, 1.0f, 0.0f, // Vertex 2
    1.0f, 0.0f, 0.0f, // Vertex 4
    1.0f, 1.0f, 0.0f, // Vertex 6

    0.0f, 0.0f, 1.0f, // Vertex 1
    0.0f, 1.0f, 1.0f, // Vertex 3
    1.0f, 0.0f, 1.0f, // Vertex 5

    0.0f, 1.0f, 1.0f, // Vertex 3
    1.0f, 0.0f, 1.0f, // Vertex 5
    1.0f, 1.0f, 1.0f  // Vertex 7
};



VoxelSystem::VoxelSystem(Engine* engine) : engine(engine), sunRotation(0)
{
    mapData = std::vector<std::uint8_t>(MAP_SIZE * MAP_SIZE * MAP_SIZE, 0);
}

void VoxelSystem::setVoxel(glm::ivec3 pos) {
    auto pos0 = pos >> 1;
    auto bitPos = pos & 0x01;
    //std::cout << pos.x << " " << pos.y << " " << pos.z << std::endl;
    //std::cout << pos0.x << " " << pos0.y << " " << pos0.z << std::endl;
    //std::cout <<  static_cast<uint32_t>(mapData[pos0.x + pos0.y * MAP_SIZE + pos0.z * MAP_SIZE * MAP_SIZE]) << std::endl;
    mapData[pos0.x + pos0.y * MAP_SIZE + pos0.z * MAP_SIZE * MAP_SIZE] |= static_cast<std::uint8_t>(1 << (bitPos.x + bitPos.z * 2 + bitPos.y * 4));
    //std::cout <<  static_cast<uint32_t>(mapData[pos0.x + pos0.y * MAP_SIZE + pos0.z * MAP_SIZE * MAP_SIZE]) << "\n" << std::endl;
}

void VoxelSystem::createWorldVoxelTexture() {
    worldTexture = createVoxelTexture(mapData, glm::vec3(MAP_SIZE));
}

void VoxelSystem::initialise()
{
    // Create OpenGL program
    voxelProgram = glCreateProgram();
    auto [vertexShader, vertexShaderStatus] = createShader(GL_VERTEX_SHADER, "../shaders/vertexShader.glsl");
    auto [fragmentShader, fragmentShaderStatus] = createShader(GL_FRAGMENT_SHADER, "../shaders/fragmentShader.glsl");
    glAttachShader(voxelProgram, vertexShader);
    glAttachShader(voxelProgram, fragmentShader);
    glLinkProgram(voxelProgram);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeBufferData), cubeBufferData, GL_STATIC_DRAW);

    glGenTextures(1, &paletteTexture);
    glBindTexture(GL_TEXTURE_2D, paletteTexture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load("palette.png", &width, &height, &nrChannels, 0);
    std::cout << width << " " << height << " " << nrChannels << std::endl;
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 1280, 720, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 720, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &normalTexture);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB_SNORM, 1280, 720, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    glGenFramebuffers(1, &depthFb);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFb);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);

    GLint no_of_extensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &no_of_extensions);

    std::set<std::string> ogl_extensions;
    for (int i = 0; i < no_of_extensions; ++i)
        ogl_extensions.insert((const char*)glGetStringi(GL_EXTENSIONS, i));
    bool texture_storage = 
    ogl_extensions.find("GL_ARB_texture_barrier") != ogl_extensions.end();
    if(texture_storage) {
        std::cout << "WE HAVE IT!!!\n";
    }
}

void VoxelSystem::update(float deltaTime, Camera &camera)
{
    sunRotation += deltaTime * 0.1f;
    auto sunPosition = glm::vec3(glm::sin(sunRotation)*10000000, 10000000, glm::cos(sunRotation)*10000000);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS); 
    glBindFramebuffer(GL_FRAMEBUFFER, depthFb);
    GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(3, attachments);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(voxelProgram);

    auto pos = camera.getPosition();
    auto dir = camera.getDirection();

    glm::mat4 projection = glm::perspective(
        glm::radians(90.f), // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
        16.0f / 9.0f,        // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
        0.1f,               // Near clipping plane. Keep as big as possible, or you'll get precision issues.
        1000.0f             // Far clipping plane. Keep as little as possible.
    );

    glm::mat4 view = glm::lookAt(
        pos,                  // the position of your camera, in world space
        pos + (dir * 1000.f), // where you want to look at, in world space
        glm::vec3(0, 1, 0)    // probably glm::vec3(0,1,0), but (0,-1,0) would make you looking upside-down, which can be great too
    );

    glUniformMatrix4fv(glGetUniformLocation(voxelProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(voxelProgram, "view"), 1, GL_FALSE, &view[0][0]);

    float materials[] = {
        0,
        1,
        0,
        0.5,
        0.5,
        0.5,
    };
    GLuint modelMatrixID = glGetUniformLocation(voxelProgram, "uModelMatrix");
    GLuint viewProjectionMatrixID = glGetUniformLocation(voxelProgram, "uViewProjectionMatrix");
    GLuint viewProjectionInvMatrixID = glGetUniformLocation(voxelProgram, "uViewProjectionInvMatrix");
    GLuint camPosID = glGetUniformLocation(voxelProgram, "uCameraPos");
    GLuint camDirID = glGetUniformLocation(voxelProgram, "uCameraDir");
    GLuint materialsID = glGetUniformLocation(voxelProgram, "materials");
    glUniform3f(camPosID, pos.x, pos.y, pos.z);
    glUniform3f(camDirID, dir.x, dir.y, dir.z);
    glUniform3fv(materialsID, 6, materials);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(
        0,        // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );

    auto& voxelComponents = engine->voxelComponents;
    std::sort(entities.begin(), entities.end(), [&voxelComponents, &camera](std::uint32_t a, std::uint32_t b)
              {
                auto closestPointA = glm::clamp(camera.getPosition(), voxelComponents[a].position, voxelComponents[a].position + voxelComponents[a].size);
                auto closestPointB = glm::clamp(camera.getPosition(), voxelComponents[b].position, voxelComponents[b].position + voxelComponents[b].size);
                auto aDist = glm::distance(closestPointA, camera.getPosition());
                auto bDist = glm::distance(closestPointB, camera.getPosition());
                return aDist < bDist;
             });
    glUniform1i(glGetUniformLocation(voxelProgram, "uWorldTexture"), 0);
    glUniform1i(glGetUniformLocation(voxelProgram, "uChunkTexture"), 1);
    glUniform1i(glGetUniformLocation(voxelProgram, "uPaletteTexture"), 2);
    glUniform1i(glGetUniformLocation(voxelProgram, "uDepthTexture"), 3);
    auto viewProjection = projection * view;
    auto viewProjectionInv = glm::inverse(viewProjection);
    glUniformMatrix4fv(viewProjectionMatrixID, 1, GL_FALSE, &viewProjection[0][0]);
    glUniformMatrix4fv(viewProjectionInvMatrixID, 1, GL_FALSE, &viewProjectionInv[0][0]);

    float counter = 1;
    for (auto entity : entities)
    {
        float voxSize = 1/counter;
        auto &voxelComponent = voxelComponents[entity];
        auto size = voxelComponent.size;
        auto rot = glm::toMat4(voxelComponent.rotation);
        auto trans = glm::translate(glm::mat4(1.f), voxelComponent.position);
        auto scale = glm::scale(glm::mat4(1.f), voxelComponent.size*voxSize);
        auto model = trans * rot * scale;
        glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &model[0][0]);
        auto minBox = voxelComponent.position;
        auto maxBox = minBox + voxelComponent.size;
        glUniform3f(glGetUniformLocation(voxelProgram, "uMinBox"), minBox.x, minBox.y, minBox.z);
        glUniform3f(glGetUniformLocation(voxelProgram, "uMaxBox"), maxBox.x, maxBox.y, maxBox.z);
        glUniform3f(glGetUniformLocation(voxelProgram, "uChunkSize"), size.x, size.y, size.z);

        auto voxScale = glm::scale(glm::mat4(1.f), size);
        auto mvp = projection * view * model;
        auto worldMatrix = trans*voxScale*glm::inverse(mvp);
        glUniformMatrix4fv(glGetUniformLocation(voxelProgram, "uWorldMatrix"), 1, GL_FALSE, &worldMatrix[0][0]);

        counter *= 2;
        
        glUniform1f(glGetUniformLocation(voxelProgram, "uVoxSize"), voxSize);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, worldTexture);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, voxelComponent.voxelTexture);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, paletteTexture);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthTexture);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, normalTexture);

        glDrawArrays(GL_TRIANGLES, 0, 36); // 3 indices starting at 0 -> 1 triangle
        //glDrawArrays(GL_LINES, 0, 36); // 3 indices starting at 0 -> 1 triangle

        glTextureBarrier();
    }
    glBindFramebuffer(GL_READ_FRAMEBUFFER, depthFb);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, 1280, 720,
                      0, 0, 1280, 720,
                      GL_COLOR_BUFFER_BIT,
                      GL_LINEAR);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void VoxelSystem::addEntity(std::uint32_t entity)
{
    entities.push_back(entity);
    auto& entityCompontent = engine->voxelComponents[entity];
    for(std::size_t x = 0;x < entityCompontent.size.x;++x) {
        for(std::size_t y = 0;y < entityCompontent.size.y;++y) {
            for(std::size_t z = 0;z < entityCompontent.size.z;++z) {
                auto size = entityCompontent.size;
                auto voxel = entityCompontent.voxelData[x + y * size.x + z * size.x * size.y];
                if(voxel != 0) {
                    auto voxelPosition = entityCompontent.position + glm::vec3(x, y, z);
                    //setVoxel(voxelPosition);
                }
            }
        }
    }
}