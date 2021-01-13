#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <learnopengl/camera.h>
#include <learnopengl/filesystem.h>
#include <learnopengl/model.h>
#include <learnopengl/shader_m.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(std::vector<std::string> faces);
void sphereSubdivision(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& line);
void computeFaceNormal(glm::vec3* v0, glm::vec3* v1, glm::vec3* v2, glm::vec3& normal);
void computeHalfVertex(glm::vec3 v1, glm::vec3 v2, glm::vec3& v);
void addVertices(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, std::vector<glm::vec3>& v);
void addNormals(glm::vec3 n1, glm::vec3 n2, glm::vec3 n3, std::vector<glm::vec3>& normals);
void addLines(glm::vec3 v1, glm::vec3 v2, std::vector<glm::vec3>& lines);
void initSphere();

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool init = true;
unsigned int sphereLineVAO, sphereLineVBO;
unsigned int sphereVAO, sphereVBO;
float sphereRadius = 1.0f;
int sphereSubdivisionLevel = 5;

std::vector<glm::vec3> sphereVertices;
std::vector<glm::vec3> sphereLines;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "P5 Amusement Park", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Can not create glfw window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Can not initilize opengl" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader floorShader("floor.vs", "floor.fs");
    Shader sphereShader("sphere.vs", "sphere.fs");
    Shader manShader("man.vs", "man.fs", "man.gs");
    Shader skyboxShader("skybox.vs", "skybox.fs");

    float planeVertices[] = {
        // positions          // texture Coords
        5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, 5.0f, 0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,

        5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
        5.0f, -0.5f, -5.0f, 2.0f, 2.0f};

    float skyboxVertices[] = {
        // positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f};

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    std::vector<std::string> faces{
        FileSystem::getPath("resources/textures/skybox/right.jpg"),
        FileSystem::getPath("resources/textures/skybox/left.jpg"),
        FileSystem::getPath("resources/textures/skybox/top.jpg"),
        FileSystem::getPath("resources/textures/skybox/bottom.jpg"),
        FileSystem::getPath("resources/textures/skybox/front.jpg"),
        FileSystem::getPath("resources/textures/skybox/back.jpg")};
    unsigned int cubemapTexture = loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    unsigned int floorTexture = loadTexture(FileSystem::getPath("resources/textures/wood.png").c_str());

    Model man(FileSystem::getPath("resources/objects/nanosuit/nanosuit.obj"));

    initSphere();

    // todo
    std::vector<glm::vec3> path;
    path.push_back(glm::vec3(5.0, 2.0, 5.0));
    path.push_back(glm::vec3(3.0, 2.0, 3.0));
    path.push_back(glm::vec3(1.0, 2.0, 1.0));
    path.push_back(glm::vec3(0.0, 2.0, 0.0));
    path.push_back(glm::vec3(-1.0, 2.0, -1.0));
    path.push_back(glm::vec3(-3.0, 2.0, -3.0));
    path.push_back(glm::vec3(-5.0, 2.0, -5.0));

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        floorShader.use();
        floorShader.setMat4("view", view);
        floorShader.setMat4("projection", projection);
        floorShader.setMat4("model", glm::mat4(1.0f));
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        manShader.use();
        manShader.setMat4("view", view);
        manShader.setMat4("projection", projection);
        model = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.3f, 0.3f)), glm::vec3(0.0, 1.0, -10.0));
        std::cout << "yaw: " << camera.Yaw << std::endl;
        std::cout << "pitch: " << camera.Pitch << std::endl;
        model = glm::rotate(model, glm::radians(-camera.Yaw - 90.0f), glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, glm::radians(camera.Pitch), glm::vec3(1.0, 0.0, 0.0));
        manShader.setMat4("model", model);
        manShader.setVec3("color", glm::vec3(204 / 255.0f, 153 / 255.0f, 0.0f));
        manShader.setFloat("time", glfwGetTime());
        man.Draw(manShader);

        model = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0, 1.0, -5.0));
        sphereShader.use();
        sphereShader.setMat4("view", view);
        sphereShader.setMat4("projection", projection);
        sphereShader.setMat4("model", model);
        sphereShader.setVec3("color", glm::vec3(1.0, 0.0, 0.0));
        glBindVertexArray(sphereVAO);
        glDrawArrays(GL_TRIANGLES, 0, sphereVertices.size());

        sphereShader.setVec3("color", glm::vec3(0.0, 0.0, 0.0));
        glBindVertexArray(sphereLineVAO);
        glDrawArrays(GL_LINES, 0, sphereLines.size());

        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// not used
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    // std::cout << xoffset << "," << yoffset << std::endl;

    // camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    float xoffset = 0.0f, yoffset = 0.0f;
    const float cameraDirectionSpeed = 210.0f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        yoffset += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        yoffset -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        xoffset += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        xoffset -= 1.0f;

    xoffset *= cameraDirectionSpeed * deltaTime;
    yoffset *= cameraDirectionSpeed * deltaTime;

    camera.ProcessMouseMovement(xoffset, yoffset);

    bool subdivisionLevelChanged = false;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && sphereSubdivisionLevel != 1) {
        sphereSubdivisionLevel = 1;
        subdivisionLevelChanged = true;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && sphereSubdivisionLevel != 2) {
        sphereSubdivisionLevel = 2;
        subdivisionLevelChanged = true;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && sphereSubdivisionLevel != 3) {
        sphereSubdivisionLevel = 3;
        subdivisionLevelChanged = true;
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && sphereSubdivisionLevel != 4) {
        sphereSubdivisionLevel = 4;
        subdivisionLevelChanged = true;
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && sphereSubdivisionLevel != 5) {
        sphereSubdivisionLevel = 5;
        subdivisionLevelChanged = true;
    }

    if (subdivisionLevelChanged) {
        // std::cout << "subdivision level changed" << std::endl;
        initSphere();
    }
}

unsigned int loadTexture(char const* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void sphereSubdivision(std::vector<glm::vec3>& v, std::vector<glm::vec3>& l) {
    const float PI = M_PI;
    const float H_ANGLE = PI / 180 * 72;
    const float V_ANGLE = atanf(1.0f / 2);

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;

    std::vector<glm::vec3> tmpVertices;
    std::vector<glm::vec3> tmpLines;

    tmpVertices.clear();
    tmpVertices.resize(12);
    int i1, i2;
    float z, xy;
    float hAngle1 = -PI / 2 - H_ANGLE / 2;
    float hAngle2 = -PI / 2;

    tmpVertices[0] = glm::vec3(0.0f, 0.0f, sphereRadius);

    for (int i = 1; i <= 5; ++i) {
        i1 = i;
        i2 = (i + 5);

        z = sphereRadius * sinf(V_ANGLE);
        xy = sphereRadius * cosf(V_ANGLE);

        tmpVertices[i1] = glm::vec3(xy * cosf(hAngle1), xy * sinf(hAngle1), z);
        tmpVertices[i2] = glm::vec3(xy * cosf(hAngle2), xy * sinf(hAngle2), -z);

        hAngle1 += H_ANGLE;
        hAngle2 += H_ANGLE;
    }

    i1 = 11;
    tmpVertices[i1] = glm::vec3(0.0f, 0.0f, -sphereRadius);

    glm::vec3 *v0, *v1, *v2, *v3, *v4, *v11;
    glm::vec3 normal;

    v0 = &tmpVertices[0];    // north pole
    v11 = &tmpVertices[11];  // south pole
    for (int i = 1; i <= 5; ++i) {
        v1 = &tmpVertices[i];
        v3 = &tmpVertices[i + 5];
        if (i < 5) {
            v2 = &tmpVertices[i + 1];
            v4 = &tmpVertices[i + 5 + 1];
        } else {
            v2 = &tmpVertices[1];
            v4 = &tmpVertices[6];
        }

        // v0 -> v1 -> v2 -> v0
        computeFaceNormal(v0, v1, v2, normal);
        vertices.push_back(*v0);
        vertices.push_back(*v1);
        vertices.push_back(*v2);
        normals.push_back(normal);
        normals.push_back(normal);
        normals.push_back(normal);

        // v1 -> v3 -> v2 -> v1
        computeFaceNormal(v1, v3, v2, normal);
        vertices.push_back(*v1);
        vertices.push_back(*v3);
        vertices.push_back(*v2);
        normals.push_back(normal);
        normals.push_back(normal);
        normals.push_back(normal);

        // v2 -> v3 -> v4 -> v2
        computeFaceNormal(v2, v3, v4, normal);
        vertices.push_back(*v2);
        vertices.push_back(*v3);
        vertices.push_back(*v4);
        normals.push_back(normal);
        normals.push_back(normal);
        normals.push_back(normal);

        // v3 -> v11 -> v4 -> v3
        computeFaceNormal(v3, v11, v4, normal);
        vertices.push_back(*v3);
        vertices.push_back(*v11);
        vertices.push_back(*v4);
        normals.push_back(normal);
        normals.push_back(normal);
        normals.push_back(normal);

        // lines
        tmpLines.push_back(*v0);
        tmpLines.push_back(*v1);
        tmpLines.push_back(*v1);
        tmpLines.push_back(*v3);
        tmpLines.push_back(*v1);
        tmpLines.push_back(*v2);
        tmpLines.push_back(*v3);
        tmpLines.push_back(*v2);
        tmpLines.push_back(*v3);
        tmpLines.push_back(*v4);
        tmpLines.push_back(*v3);
        tmpLines.push_back(*v11);
    }

    for (int i = 0; i < sphereSubdivisionLevel; ++i) {
        tmpVertices = vertices;
        vertices.clear();
        tmpLines.clear();

        std::size_t count = tmpVertices.size();
        glm::vec3 v1, v2, v3, new1, new2, new3;
        for (std::size_t j = 0; j < count; j += 3) {
            v1 = tmpVertices[j];
            v2 = tmpVertices[j + 1];
            v3 = tmpVertices[j + 2];

            computeHalfVertex(v1, v2, new1);
            computeHalfVertex(v2, v3, new2);
            computeHalfVertex(v1, v3, new3);

            computeFaceNormal(&v1, &new1, &new3, normal);
            addVertices(v1, new1, new3, vertices);
            addNormals(normal, normal, normal, normals);

            computeFaceNormal(&new1, &v2, &new2, normal);
            addVertices(new1, v2, new2, vertices);
            addNormals(normal, normal, normal, normals);

            computeFaceNormal(&new1, &new2, &new3, normal);
            addVertices(new1, new2, new3, vertices);
            addNormals(normal, normal, normal, normals);

            computeFaceNormal(&new3, &new2, &v3, normal);
            addVertices(new3, new2, v3, vertices);
            addNormals(normal, normal, normal, normals);

            addLines(v1, new1, tmpLines);
            addLines(new1, new3, tmpLines);
            addLines(new1, new2, tmpLines);
            addLines(new1, v2, tmpLines);
            addLines(v2, new2, tmpLines);
            addLines(new2, v3, tmpLines);
            addLines(new2, new3, tmpLines);
        }
    }

    // build interleaved vertices
    v.clear();
    std::size_t count = vertices.size();
    for (std::size_t i = 0; i < count; ++i) {
        v.push_back(vertices[i]);
        v.push_back(normals[i]);
    }

    l = tmpLines;
}

void computeFaceNormal(glm::vec3* v0, glm::vec3* v1, glm::vec3* v2, glm::vec3& normal) {
    normal = glm::vec3(0.0, 0.0, 0.0);

    glm::vec3 e1(*v1 - *v0);
    glm::vec3 e2(*v2 - *v1);

    normal = glm::cross(e1, e2);
    normal = glm::normalize(normal);
}

void computeHalfVertex(glm::vec3 v1, glm::vec3 v2, glm::vec3& v) {
    v = glm::vec3(v1 + v2);
    v = glm::normalize(v) * sphereRadius;
}

void addVertices(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, std::vector<glm::vec3>& v) {
    v.push_back(v1);
    v.push_back(v2);
    v.push_back(v3);
}

void addNormals(glm::vec3 n1, glm::vec3 n2, glm::vec3 n3, std::vector<glm::vec3>& normals) {
    normals.push_back(n1);
    normals.push_back(n2);
    normals.push_back(n3);
}

void addLines(glm::vec3 v1, glm::vec3 v2, std::vector<glm::vec3>& lines) {
    lines.push_back(v1);
    lines.push_back(v2);
}

void initSphere() {
    sphereSubdivision(sphereVertices, sphereLines);
    // std::cout << "size of glm::vec3: " << sizeof(glm::vec3) << std::endl;
    // std::cout << "3 * size of float: " << 3 * sizeof(float) << std::endl;
    // std::cout << "sizeof sphere vertices: " << sphereVertices.size() << std::endl;
    // for (int i = 0; i < sphereVertices.size(); ++i) {
    //     std::cout << sphereVertices[i].x << "," << sphereVertices[i].y << "," << sphereVertices[i].z << std::endl;
    // }

    if (init) {
        init = false;
        glGenVertexArrays(1, &sphereVAO);
        glGenBuffers(1, &sphereVBO);
        glGenVertexArrays(1, &sphereLineVAO);
        glGenBuffers(1, &sphereLineVBO);
    }

    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVAO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * (3 * sizeof(float)), &sphereVertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * (3 * sizeof(float)), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * (3 * sizeof(float)), (void*)(3 * sizeof(float)));

    glBindVertexArray(sphereLineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereLineVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereLines.size() * (3 * sizeof(float)), &sphereLines[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3 * sizeof(float)), (void*)0);
}