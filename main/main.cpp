#include <cstdlib>
#include <glcore/camera.h>
#include <glcore/core.h>
#include <glcore/texture.h>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

class MyApp : public App {
public:
  MyApp() : App(3, 3, 800, 600, "Textures") {

    shader = std::make_unique<Shader>(SHADER_PATH "vertex.glsl",
                                      SHADER_PATH "fragment.glsl");

    /*std::vector<float> data(
        {-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 0.5f,   -0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f,  -0.5f,   1.0f, 1.0f, 0.5f,   0.5f,  -0.5f, 1.0f, 1.0f,
         -0.5f, 0.5f,  -0.5f,   0.0f, 1.0f, -0.5f,  -0.5f, -0.5f, 0.0f, 0.0f,

         -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

         -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
         -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
         -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
         0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
         0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
         0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
         -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

         -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f});*/

    std::vector<float> data = generatePlaneVx(120, 120);
    std::vector<unsigned int> inds = generatePlaneInds(120, 120);
    nbuf = std::make_unique<BasicBuffer>(data, inds);

    settings.container = textures.size();
    textures.emplace_back(RESOURCES_PATH "container.jpg");
    settings.face = textures.size();
    textures.emplace_back(RESOURCES_PATH "awesomeface.png");
    
    // Texture *container = new Texture(RESOURCES_PATH "container.jpg");
    // Texture *face = new Texture(RESOURCES_PATH "awesomeface.png");

    //buf->addTexture(&textures[settings.container]);
    //buf->addTexture(&textures[settings.face]);
     
    // buf->addTexture(container);
    // buf->addTexture(face);
    
    //buf->build();
    nbuf->build();

    cam = std::make_unique<Camera>();
    cam->position = glm::vec3(0.f, 2.f, 0.f);
    cam->speed = 2.f;
    cam->distance = 2.f;

    positions = std::vector<glm::vec3>(
        {glm::vec3(0.0f, 0.0f, 0.0f)});

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }

  std::vector<float> generatePlaneVx(int width, int height) { // ancho y alto en vertices
    std::vector<float> plane = {};

    int vertCount = width*height*3;
    int indCount = (width*height) + (width-1)*(height-2);

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

    int vertCount = width*height*3;
    int indCount = (width*height) + (width-1)*(height-2);

    for (int row = 0; row<height; row++) {
      for (int col = 0; col<width; col ++) {

        int curInd = row*width + col;
        int diagUp = curInd - width + 1;
        int diagDown = curInd + width + 1;
        int next = curInd + 1;
        int down = curInd + width;
        int up = curInd - width;

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
        settings.ycam += 1.f;
      }
      if (key == GLFW_KEY_S) {
        settings.ycam -= 1.f;
      }
      if (key == GLFW_KEY_D) {
        settings.xcam += 1.f;
      }
      if (key == GLFW_KEY_A) {
        settings.xcam -= 1.f;
      }
      if (key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, 1);
    }
    if (action == GLFW_RELEASE) {
      if (key == GLFW_KEY_W) {
        settings.ycam -= 1.f;
      }
      if (key == GLFW_KEY_S) {
        settings.ycam += 1.f;
      }
      if (key == GLFW_KEY_D) {
        settings.xcam -= 1.f;
      }
      if (key == GLFW_KEY_A) {
        settings.xcam += 1.f;
      }
    }
    cam->key_callback(settings.xcam, settings.ycam);
  }

  void cursor_position_callback(double xpos, double ypos) override {
    cam->cursor_position_callback(xpos, -ypos);
  }

  void scroll_callback(double xoffset, double yoffset) override {
    cam->scroll_callback(yoffset);
  }

private:
  struct {
    float xcam = 0.f, ycam = 0.f;
    int container, face;
  } settings;

  float u_time = 0.0;

  std::unique_ptr<Shader> shader;
  std::unique_ptr<Camera> cam;
  std::unique_ptr<TextureBuffer> buf;
  std::unique_ptr<BasicBuffer> nbuf;
  std::vector<Texture> textures;
  std::vector<glm::vec3> positions;

  void render() override {
    u_time += 0.01;
    App::render();
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
      /*model =
          glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, .5f));*/
      shader->set("model", model);
      shader->set("vtx", glm::vec3(1.0f, 1.0f, 1.0f));
      shader->set("u_resolution", glm::vec2(width_, height_));
      shader->set("u_time", u_time);
      //buf->draw();
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
