#include <cstdlib>
#include <glcore/camera.h>
#include <glcore/core.h>
#include <glcore/texture.h>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>


class MyCam : public Camera {
public:
  MyCam(glm::vec3 position = glm::vec3(0.f), float distance = 1.f, float yaw = 0.0f, float pitch = 0.0f, glm::vec3 up = glm::vec3(0.f, 1.f, 0.f)) : Camera(position, distance, yaw, pitch, up) {


  }

  void move_towards(glm::vec3 v) {
    direction_ += v;
  }

};

class MyApp : public App {
public:
  MyApp() : App(3, 3, 800, 600, "Textures") {

    shader = std::make_unique<Shader>(SHADER_PATH "vertex.glsl",
                                      SHADER_PATH "fragment.glsl");

    std::vector<float> data = generatePlaneVx(settings.mesh_res, settings.mesh_res);
    std::vector<unsigned int> inds = generatePlaneInds(settings.mesh_res, settings.mesh_res);
    nbuf = std::make_unique<BasicBuffer>(data, inds);

    settings.container = textures.size();
    textures.emplace_back(RESOURCES_PATH "container.jpg");
    settings.face = textures.size();
    textures.emplace_back(RESOURCES_PATH "awesomeface.png");
    nbuf->build();

    cam = std::make_unique<MyCam>();
    cam->position = glm::vec3(0.f, 10.0f, 0.f);
    cam->speed = 2.f;
    cam->distance = 2.f;

    positions = std::vector<glm::vec3>(
        {glm::vec3(0.0f, 0.0f, 0.0f)});

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }

  std::vector<float> generatePlaneVx(int width, int height) { // ancho y alto en vertices
    std::vector<float> plane = {};

    for (int row = 0; row<height; row++) {
      for (int col = 0; col<width; col++) {
        plane.emplace_back(row);
        plane.emplace_back(0.0);
        plane.emplace_back(col);

      }
    }

    return plane;

  }

  std::vector<unsigned int> generatePlaneInds(int width, int height) { // ancho y alto en vertices
    std::vector<unsigned int> inds = {};

    for (int row = 0; row<height; row++) {
      for (int col = 0; col<width; col ++) {

        int curInd = row*width + col;
        int diagDown = curInd + width + 1;
        int next = curInd + 1;
        int down = curInd + width;

        if (row < height-1 && col < width - 1) {
          inds.emplace_back(curInd);
          inds.emplace_back(next);
          inds.emplace_back(down);

          inds.emplace_back(next);
          inds.emplace_back(diagDown);
          inds.emplace_back(down);
        }
      }
    }

    return inds;

  }

  void key_callback(int key, int scancode, int action, int mods) override {
    if (action == GLFW_PRESS) {
      if (key == GLFW_KEY_W) {
        settings.ycam += settings.campseed;
      }
      if (key == GLFW_KEY_S) {
        settings.ycam -= settings.campseed;
      }
      if (key == GLFW_KEY_D) {
        settings.xcam += settings.campseed;
      }
      if (key == GLFW_KEY_A) {
        settings.xcam -= settings.campseed;
      }
      if (key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, 1);

      if (key == GLFW_KEY_SPACE) {
        cam->move_towards(glm::vec3(.0f, 1.0f, 0.0f));
      }

      if (key == GLFW_KEY_K) {
        glfwSetInputMode(window, GLFW_CURSOR, glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
      }

    }
    if (action == GLFW_RELEASE) {
      if (key == GLFW_KEY_W) {
        settings.ycam -= settings.campseed;
      }
      if (key == GLFW_KEY_S) {
        settings.ycam += settings.campseed;
      }
      if (key == GLFW_KEY_D) {
        settings.xcam -= settings.campseed;
      }
      if (key == GLFW_KEY_A) {
        settings.xcam += settings.campseed;
      }
      if (key == GLFW_KEY_SPACE) {
        cam->move_towards(glm::vec3(.0f, 1.0f, 0.0f));
      }
    }
    cam->key_callback(settings.xcam, settings.ycam);
  }

  void cursor_position_callback(double xpos, double ypos) override {
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
      cam->cursor_position_callback(xpos, -ypos);
    }

  }

  void scroll_callback(double xoffset, double yoffset) override {
    cam->scroll_callback(yoffset);
  }

private:
  struct {
    float xcam = 0.f, ycam = 0.f, campseed = 3.0f, clineh = 9.0;
    int container, face;
    int mesh_res = 60;
  } settings;

  float u_time = 0.0;

  std::unique_ptr<Shader> shader;
  std::unique_ptr<MyCam> cam;
  std::unique_ptr<TextureBuffer> buf;
  std::unique_ptr<BasicBuffer> nbuf;
  std::vector<Texture> textures;
  std::vector<glm::vec3> positions;

  void render() override {
    u_time += 0.01;
    App::render();

    ImGui::Begin("Render settings");
    ImGui::SliderFloat("Isocline height", &settings.clineh, 5.0, 15.0);
    ImGui::End();

    shader->use();
    shader->set("view", cam->view());
    shader->set("projection",
                glm::perspective(glm::radians(cam->zoom),
                                 ((float)width_) / ((float)height_), 0.1f,
                                 100.0f));

    int i = 0;
    for (auto &pos : positions) {
      glm::mat4 model(1.0f);
      model = glm::translate(model, pos);
      float angle = 20.0f * i;
      i++;
      shader->set("model", model);
      shader->set("vtx", glm::vec3(1.0f, 1.0f, 1.0f));
      shader->set("u_resolution", glm::vec2(width_, height_));
      shader->set("u_time", u_time);
      shader->set("clineh", settings.clineh);
      shader->set("viewPos", cam->position);
      //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      nbuf->draw();

    }
  }

  void update(float deltaTime) override { cam->update(deltaTime); }
};

int main() {
  MyApp app;
  app.run();
  return 0;
}
