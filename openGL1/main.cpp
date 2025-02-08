#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "timer.h"
#include "texture.h"
#include "textureDictionary.h"
#include "camera.h"
#include "chunk.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, Camera &camera, float delta){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    const float cameraSpeed = 20.f * delta; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.pos += cameraSpeed * camera.front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.pos -= cameraSpeed * camera.front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.pos -= glm::normalize(glm::cross(camera.front, camera.up)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.pos += glm::normalize(glm::cross(camera.front, camera.up)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.pos += cameraSpeed * camera.up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.pos -= cameraSpeed * camera.up;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}


float lastX = 400, lastY = 300;
bool firstMouse = 1;
Camera camera;


void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if (firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.5f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera.horizontal += xoffset;
    camera.vertical += yoffset;

    if (camera.vertical > 89.0f)
        camera.vertical = 89.0f;
    if (camera.vertical < -89.0f)
        camera.vertical = -89.0f;
}

glm::vec3 lerp(glm::vec3 l, glm::vec3 r, float k) {
    glm::vec3 diff = r - l;
    return l + diff * k;
}

void updateSkyColor(float k, float fov, float *v, glm::vec3 bot, glm::vec3 top) {
    glm::vec3 B, T;
    float bk, tk;
    float angle = -asin(k);
    bk = (sin(angle - fov / 2) + 1) / 2;
    tk = (sin(angle + fov / 2) + 1) / 2;
    B = lerp(bot, top, bk);
    T = lerp(bot, top, tk);

    v[3] = B.x;
    v[4] = B.y;
    v[5] = B.z;

    v[9] = T.x;
    v[10] = T.y;
    v[11] = T.z;

    v[15] = T.x;
    v[16] = T.y;
    v[17] = T.z;

    v[21] = B.x;
    v[22] = B.y;
    v[23] = B.z;
}

int main() {
    srand(time(NULL));
    stbi_set_flip_vertically_on_load(true);

    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Minecraft", NULL, NULL);
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    float screenVertices[] = {
        -1.f, -1.f, 0.f, 0.f, 0.f, 0.f,
        -1.f, 1.f, 0.f, 0.f, 0.f, 0.f,
        1.f, 1.f, 0.f, 0.f, 0.f, 0.f,
        1.f, -1.f, 0.f, 0.f, 0.f, 0.f
    };

    unsigned int screenIndices[] = {
        2, 1, 0,
        3, 2, 0
    };


    float faceVertices[] = {
        1.f, 0.f, 0.f, 1.f, 0.f,
        1.f, 0.f, 1.f, 1.f, 1.f,
        0.f, 0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f, 1.f
    };

    TextureDictionary texDict;
    texDict.loadnames();
    texDict.loadfaces();

    //TexDict.loadnames();
    //TexDict.loadfaces();

    TextureArray textures("assets/block/", ".png", texDict.names);
   

    Shader shader("vertex.glsl", "fragment.glsl");

    Shader skybox("screen.glsl", "skybox.glsl");

    World world;
    //world.testgen(texDict);

    //unsigned int cVBO, fVBO;
    //glGenBuffers(1, &cVBO);
    //glGenBuffers(1, &fVBO);
    //unsigned int cVAO;
    //glGenVertexArrays(1, &cVAO);
    //glBindVertexArray(cVAO);
    //glBindBuffer(GL_ARRAY_BUFFER, fVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(faceVertices), faceVertices, GL_STATIC_DRAW);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);
    //
    //vector<unsigned int>cData(world.VOLUME * 24576);
    //cout << cData.size() << "(\n";


    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, chunk.indices.size() * sizeof(unsigned int), chunk.indices.data(), GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, chunk.vertices.size() * sizeof(float), chunk.vertices.data(), GL_STATIC_DRAW);
    //
    //
    //float stride = 5 * sizeof(float);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    //glEnableVertexAttribArray(0);
    //
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);


    unsigned int sVBO, sEBO, sVAO;
    glGenBuffers(1, &sVBO);
    glGenBuffers(1, &sEBO);
    glGenVertexArrays(1, &sVAO);

    glBindVertexArray(sVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sEBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(screenIndices), screenIndices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);


    float stride = 6 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    Timer timer;
    Timer tickTimer;
    int fps = 0;

    glEnable(GL_CULL_FACE);       //TURN ON LATAER
    //glCullFace(GL_FRONT);
    glEnable(GL_MULTISAMPLE);
    float dayTime = 3.14 / 2;
    glm::vec3 sun = 1600.f * glm::normalize(glm::vec3(cos(dayTime), sin(dayTime), 0.f));

    shader.use();
    shader.setVec3("sunPos", sun);

    while (!glfwWindowShouldClose(window)){

        //world.update(camera.pos, texDict);
        timer.use();

        dayTime += timer.deltaS() / 10.f;
        glm::vec3 sun = glm::normalize(glm::vec3(cos(dayTime), sin(dayTime), 0.f));

        fps++;

        glm::mat4 model = glm::mat4(1.f);
        model = glm::rotate(model, glm::radians(0.f), glm::vec3(1.f, 1.f, 0.f));

        glm::mat4 view = camera.getViewMatrix();
        //glm::mat4 view = glm::mat4(1.f);
        //view = glm::translate(view, glm::vec3(0.f, 0.f, -3.f));

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(80.f), 800.f / 600.f, 0.1f, 10000.f);


        camera.move();
        float timeValue = glfwGetTime();
        //float greenValue = (sin(timeValue) / 2.f) + 0.5f;
        //int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        float offset = (sin(timeValue) + 1.f) / 2.f;
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        skybox.use();
        skybox.setVec3("sun", sun);
        skybox.setVec3("front", camera.front);
        skybox.setMat4("rot", model);
        updateSkyColor(camera.front.y, 80.f, screenVertices, glm::vec3(12 / 255.f, 6 / 255.f, 69 / 255.f), glm::vec3(49 / 255.f, 184 / 255.f, 214 / 255.f));
        glBindVertexArray(sVAO);
        glBindBuffer(GL_ARRAY_BUFFER, sVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);
        //skybox.setFloat("ik", camera.front.y);
        //skybox.setVec3("topColor", glm::vec3(0.02f, 0.2f, 0.8f));
        //skybox.setVec3("bottomColor", glm::vec3(0.02f, 0.1f, 0.4f));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glClear(GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setVec3("sunPos", sun);
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textures.ID);
        shader.setInt("textures", 0);


        world.manageThreads(texDict);


        //glDrawArrays(GL_TRIANGLES, 0, chunk.indices.size() / 3);
        int triangles = 0;
        world.prevPos = glm::ivec3(camera.pos) / 16;
        for (int x = -world.radius + 1; x < world.radius; x++) {
            for (int y = MINY + 1; y < MAXY; y++) {
                for (int z = -world.radius + 1; z < world.radius; z++) {
                    glm::ivec3 cpos(x, y, z);
                    cpos += glm::ivec3(camera.pos) / 16;
                    //cpos.y = y;
                    glm::vec3 chunkPos = cpos * 16;
                    glm::vec4 transformed = projection * view * model * glm::vec4(chunkPos, 1.0f);
                    if (transformed.z < -16.f) continue;        //optimize to dynamic fov culling

                    const RenderChunk& CURRENT = world.getRenderAt(cpos, texDict);
                    if (!CURRENT.ready) continue;

                    //for (int j = 0; j < CURRENT.faceData.size(); j++) {
                    //    bitset <32> X;
                    //    X = CURRENT.faceData[j];
                    //    std::cout << X << "\n";
                    //}

                    shader.setVec3("chunkOffset", CURRENT.offset);
                    shader.setFloat("slod", CURRENT.slod);
                    //CURRENT.faceData.clear();
                    //cout << CURRENT.faceData.size() << "\n";
                    glBindVertexArray(CURRENT.VAO);
                    //glBindBuffer(GL_ARRAY_BUFFER, CURRENT.iVBO);
                    //glBindBuffer(GL_ARRAY_BUFFER, CURRENT.VBO);
                    //glBufferData(GL_ARRAY_BUFFER, sizeof(int) * CURRENT.faceData.size(), CURRENT.faceData.data(), GL_STATIC_DRAW);

                    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, CURRENT.faceData.size());
                    triangles += CURRENT.faceData.size();

                }
            }
        }

        //std::cout << "tris: " << triangles << "\n";
        processInput(window, camera, timer.deltaS());
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (tickTimer.getElapsed() > 1) {
            //debug console output
            tickTimer.use();
            std::cout << "fps: " << fps << "\n";
            fps = 0;

            std::cout << camera.pos.x << " " << camera.pos.y << " " << camera.pos.z << "\n";

            std::cout << "triangles drawn: " << triangles * 2 << "\n";
        }
    }

    glfwTerminate();
	return 0;
}
