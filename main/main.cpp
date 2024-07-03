#include <cstdlib>
#include <glcore/camera.h>
#include <glcore/core.h>
#include <glcore/texture.h>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

class SquarePrimitiveBuffer : BasicBuffer {
  using BasicBuffer::BasicBuffer;

public:
  void draw() override {
    bind();

    glDrawArrays(GL_QUADS, 0, vertices_.size());
  }
};


class SquarePatchBuffer : BasicBuffer {
  using BasicBuffer::BasicBuffer;
  public:

  void draw() override {
    bind();

    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawArrays(GL_PATCHES, 0, vertices_.size()/3);
  }

  void build() override {
    bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_.size(),
                 vertices_.data(), GL_STATIC_DRAW);

    if (indices_.size() == 0) {
      printf("a");
      for (unsigned int i = 0; i < vertices_.size(); i++) {
        indices_.push_back(i);
      }
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices_.size(),
                 indices_.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
  }
};




class MyCam : public Camera {
public:
  MyCam(glm::vec3 position = glm::vec3(0.f), float distance = 1.f, float yaw = 0.0f, float pitch = 0.0f, glm::vec3 up = glm::vec3(0.f, 1.f, 0.f)) : Camera(position, distance, yaw, pitch, up) {


  }

  void update_h(float h) {
    position += h*up_;
  }

};

class MyApp : public App {
public:
  MyApp() : App(3, 3, 960, 640, "Textures") {

    shader = std::make_unique<Shader>(SHADER_PATH "terrain_vertex.glsl",
                                      SHADER_PATH "terrain_fragment.glsl");

    tess_shader = std::make_unique<Shader>(SHADER_PATH "reference_vs.glsl",
                                        SHADER_PATH "terrain_tess_control.tesc",
                                      SHADER_PATH "terrain_tess_eval.tese",
                                      SHADER_PATH "terrain_fragment.glsl");

    sun_shader = std::make_unique<Shader>(SHADER_PATH "sun_vertex.glsl",
                                          SHADER_PATH "sun_fragment.glsl");

    tess_sun_shader = std::make_unique<Shader>(SHADER_PATH "reference_vs.glsl",
                                        SHADER_PATH "sun_tess_control.tesc",
                                        SHADER_PATH "sun_tess_eval.tese",
                                        SHADER_PATH "sun_fragment.glsl");


    std::vector<float> data = generatePlaneVx(settings.mesh_res, settings.mesh_res);
    std::vector<unsigned int> inds = generatePlaneInds(settings.mesh_res, settings.mesh_res);
    nbuf = std::make_unique<BasicBuffer>(data, inds);
    pbuf = std::make_unique<SquarePatchBuffer>(generatePatchPlaneVx(settings.patch_num, settings.patch_num), generatePatchPlaneInds(settings.patch_num, settings.patch_num));
    sunbuf = std::make_unique<BasicBuffer>(generateCubeVx(2.0), generateCubeInds());
    sqbuf = std::make_unique<SquarePrimitiveBuffer>(generatePlaneVx(settings.patch_num, settings.patch_num), generatePatchPlaneInds(settings.patch_num, settings.patch_num));
    sqsun = std::make_unique<SquarePatchBuffer>(generateCubeVx(2), generateCubePatchInds());

    nbuf->build();
    pbuf->build();
    sunbuf->build();
    sqsun->build();

    cam = std::make_unique<MyCam>();
    cam->position = glm::vec3(0.f, 10.0f, 0.f);
    cam->speed = 2.f;
    cam->distance = 2.f;

    positions = std::vector<glm::vec3>(
        {glm::vec3(0.0f, 0.0f, 0.0f)});

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


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
        settings.camh += settings.campseed*0.166;
        cam->update_h(settings.camh);
      }

      if (key == GLFW_KEY_P) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
        settings.camh += settings.campseed;
        cam->update_h(settings.camh);
      }

      if (key == GLFW_KEY_P) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
    float xcam = 0.f, ycam = 0.f,
          campseed = 9.0f,  camh = 0.0,
          clineh = 9.0, dayspeed = 1.0,
          sunr = 4.0, world_scale = 0.1,
          fog_f = 0.5;
    int container, face;
    int mesh_res = 400;
    int terrain_gen = 0;
    int gen_num = 2;
    int patch_num = 10;

    bool better_sun = true;

    glm::vec3 sunPos = glm::vec3(0.0, 15.0, 0.0);
  } settings;

  float u_time = 0.0;

  //shaders
  std::unique_ptr<Shader> shader;
  std::unique_ptr<Shader> tess_shader;
  std::unique_ptr<Shader> sun_shader;
  std::unique_ptr<Shader> tess_sun_shader;
  std::unique_ptr<MyCam> cam;

  std::unique_ptr<BasicBuffer> nbuf;
  std::unique_ptr<SquarePatchBuffer> pbuf;
  std::unique_ptr<SquarePatchBuffer> sqsun;
  std::unique_ptr<SquarePrimitiveBuffer> sqbuf;
  std::unique_ptr<BasicBuffer> sunbuf;

  std::vector<Texture> textures;
  std::vector<glm::vec3> positions;

  void render() override {
    App::render();
    glClearColor(0.737, 1, 0.925, 1.0f);
    //glClearColor(0.0, 0.0, 0.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui::Begin("Render settings");
    ImGui::SliderFloat("World Scale", &settings.world_scale, 0.01, 0.1);
    ImGui::SliderFloat("Fog Factor", &settings.fog_f, 0.0, 2.0);
    ImGui::SliderFloat("Isocline height", &settings.clineh, 5.0, 15.0);
    ImGui::SliderFloat("LightX", &settings.sunPos.x, 0.0, 15.0);
    ImGui::SliderFloat("LightY", &settings.sunPos.y, 15.0, 40.0);
    ImGui::SliderFloat("LightZ", &settings.sunPos.z, 0.0, 15.0);
    if (ImGui::Button("Switch Terrain")) {
      settings.terrain_gen = (settings.terrain_gen + 1)%settings.gen_num;
    }
    ImGui::End();

    glDisable(GL_CULL_FACE);
    glm::mat4 model(1.0f);
    model = glm::translate(model, settings.sunPos);

    if (settings.better_sun) {

      tess_sun_shader->use();
      tess_sun_shader->set("model", model);
      tess_sun_shader->set("view", cam->view());
      tess_sun_shader->set("projection",
                  glm::perspective(glm::radians(cam->zoom),
                                   ((float)width_) / ((float)height_), 0.1f,
                                   100.0f));

      sqsun->draw();

    }
    else {
      sun_shader->use();
      sun_shader->set("model", model);
      sun_shader->set("view", cam->view());
      sun_shader->set("projection",
                  glm::perspective(glm::radians(cam->zoom),
                                   ((float)width_) / ((float)height_), 0.1f,
                                   100.0f));

      sunbuf->draw();

    }


    model = glm::translate(model, glm::vec3(0.0));
    sun_shader->set("model", model);
    sqbuf->draw();
    //nbuf->draw();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    initTerrainUniforms(tess_shader);
    pbuf->draw();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //nbuf->draw();

  }

  void initTerrainUniforms(std::unique_ptr<Shader> &shader) {
    shader->use();
    shader->set("view", cam->view());
    shader->set("projection",
                glm::perspective(glm::radians(cam->zoom),
                                 ((float)width_) / ((float)height_), 0.1f,
                                 100.0f));


    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(0.0));
    shader->set("model", model);
    shader->set("vtx", settings.sunPos);
    shader->set("u_resolution", glm::vec2(width_, height_));
    shader->set("u_time", u_time);
    shader->set("clineh", settings.clineh);
    shader->set("viewPos", cam->position);
    shader->set("terrainGen", settings.terrain_gen);
    shader->set("world_scale", settings.world_scale);
    shader->set("fog_f", settings.fog_f);

  }

  void update(float deltaTime) override {
    cam->update(deltaTime);
    u_time += deltaTime;
  }


  std::vector<float> generatePlaneVx(int width, int height) { // ancho y alto en vertices
    std::vector<float> plane = {};

    float min_x = -50.0;
    float max_x = 50.0;
    float x_step = (max_x - min_x)/width;

    float min_y = -50.0;
    float max_y = 50.0;
    float y_step = (max_y - min_y)/height;

    for (int row = 0; row<height; row++) {
      float nrow = min_y + row*y_step;
      for (int col = 0; col<width; col++) {
        plane.emplace_back(nrow);
        plane.emplace_back(0.0);
        plane.emplace_back(min_x + col*x_step);
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

  std::vector<float> generatePatchPlaneVx(int width, int height) { // alto y ancho en quads
    std::vector<float> vertices = {};

    float min_x = -100.0;
    float max_x = 100.0;
    float x_step = (max_x - min_x)/width;

    float min_z = -100.0;
    float max_z = 100.0;
    float z_step = (max_z - min_z)/height;

    for (int i = 0; i<height; i++) {
      float nrow = min_z + z_step*i;
      for (int j = 0; j<width; j++) {
        float ncol = min_x + x_step*j;
        vertices.emplace_back(ncol);
        vertices.emplace_back(0.0);
        vertices.emplace_back(nrow);

        vertices.emplace_back(ncol + x_step);
        vertices.emplace_back(0.0);
        vertices.emplace_back(nrow);

        vertices.emplace_back(ncol + x_step);
        vertices.emplace_back(0.0);
        vertices.emplace_back(nrow + z_step);

        vertices.emplace_back(ncol);
        vertices.emplace_back(0.0);
        vertices.emplace_back(nrow + z_step);




      }
    }




    return vertices;
  }

  std::vector<unsigned int> generatePatchPlaneInds(int width, int height) { // ancho y alto en vertices
    std::vector<unsigned int> inds = {};

    for (int row = 0; row<height; row++) {
      for (int col = 0; col<width; col ++) {

        int curInd = row*width + col;
        int diagDown = curInd + width + 1;
        int next = curInd + 1;
        int down = curInd + width;

        if (row < height-1 && col < width - 1) { // generar índices de cuadrados

          inds.emplace_back(down);
          inds.emplace_back(diagDown);
          inds.emplace_back(next);
          inds.emplace_back(curInd);
        }
      }
    }

    return inds;
  }

  std::vector<float> generateCubeVx(float edge_len) {
    std::vector<float> vxs;
    for (int j = 0; j<2; j++) {
      float y = pow(-1, j)*edge_len/2;
      for (int k = 0; k<2; k++) {
        float z = pow(-1, k)*edge_len/2;
        for (int i = 0; i<2; i++) {
          float x = pow(-1, i)*edge_len/2;
          vxs.emplace_back(x);
          vxs.emplace_back(y);
          vxs.emplace_back(z);
        }
      }
    }

    return vxs;
  }

  std::vector<unsigned int> generateCubePatchInds() {
    std::vector<unsigned int> inds = {
      0, 1, 2, 3, // bottom face
      4, 5, 6, 7, // top face
      0, 1, 4, 5, // front face
      2, 3, 6, 7, // back face
      1, 2, 5, 6, // right face
      0, 3, 4, 7 // left face
    };
  }

  std::vector<unsigned int> generateCubeInds() {
    std::vector<unsigned int> inds = {
      0, 1, 3, //top 1
      3, 1, 2, //top 2
      2, 6, 7, //front 1
      7, 3, 2, //front 2
      7, 6, 5, //bottom 1
      5, 4, 7, //bottom 2
      5, 1, 4, //back 1
      4, 1, 0, //back 2
      4, 3, 7, //right 1
      3, 4, 0, //right 2
      5, 6, 2, //left 1
      5, 1, 2  //left 2
    };

    return inds;
  }

};

int main() {
  MyApp app;
  app.run();
  return 0;
}
