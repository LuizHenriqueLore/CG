#include <cg_window.hpp>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

#define MOVE_TOP 1
#define MOVE_BOTTOM 2
#define MOVE_LEFT 3
#define MOVE_RIGHT 4

#define DEFAULT_ROTATE_SPEED 0.05
#define SPEED_UP 0.0011 
#define SPEED_DOWN -0.0011

#define MOVE_SPEED 0.02f

class Demo : public cgicmc::Window {
public:
  Demo() : cgicmc::Window() {}

  void run() override {
    while (!glfwWindowShouldClose(_window)) {

      stateW = glfwGetKey(_window, GLFW_KEY_W);
      stateS = glfwGetKey(_window, GLFW_KEY_S);
      stateA = glfwGetKey(_window, GLFW_KEY_A);
      stateD = glfwGetKey(_window, GLFW_KEY_D);

      stateQ = glfwGetKey(_window, GLFW_KEY_Q);
      stateE = glfwGetKey(_window, GLFW_KEY_E);
      stateSpace = glfwGetKey(_window, GLFW_KEY_SPACE);
      

      //não permite andar na diagonal
      /*if (stateW == GLFW_PRESS){
        moveObject(MOVE_TOP);
      }else{
        if (stateS == GLFW_PRESS){
          moveObject(MOVE_BOTTOM);
        }else{
          if (stateA == GLFW_PRESS){
            moveObject(MOVE_LEFT);
          }else{
            if (stateD == GLFW_PRESS){
              moveObject(MOVE_RIGHT);
            }
          }
        }
      }*/

      //movimentação (permite andar na diagonal)
      if (stateW == GLFW_PRESS)
        moveObject(MOVE_TOP);
      if (stateS == GLFW_PRESS)
        moveObject(MOVE_BOTTOM);
      if (stateA == GLFW_PRESS)
        moveObject(MOVE_LEFT);
      if (stateD == GLFW_PRESS)
        moveObject(MOVE_RIGHT);

      //para ou retorna a rotação
      if(stateSpace == GLFW_PRESS && spaceReleased)  
        changeRotation();
      else
        if(stateSpace == GLFW_RELEASE)
          spaceReleased = 1;

      //altera a velocidade da rotação
      if(stateQ == GLFW_PRESS)  
        changeSpeedRotation(SPEED_UP);
      else
        if(stateE == GLFW_PRESS)  
          changeSpeedRotation(SPEED_DOWN);
      

      rotateObject(rotateSpeed);
      changeScene();

      // Comandos de renderizacao vao aqui
      glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      // etc...

      glm::mat4 transform = glm::mat4(1.0f);
      //transform = glm::translate(transform, glm::vec3(0.5f, 0.286f, 0.0f));
      //transform = glm::rotate(transform, (float)glfwGetTime(),
      //                      glm::vec3(0.0f, 0.0f, 1.0f));
      //transform = glm::translate(transform, glm::vec3(+0.1f, 0.0f, 0.0f));

      glUseProgram(shaderProgramId);
      unsigned int transformLoc =
          glGetUniformLocation(shaderProgramId, "transform");
      glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

      glBindVertexArray(_VAO);
      glDrawArrays(GL_TRIANGLES, 0, 12);
      // Controla eventos e troca os buffers para renderizacao
      glfwSwapBuffers(_window);
      glfwPollEvents();
    }
  }
  void changeSpeedRotation(float val){
    if(!((val>0 && rotateSpeed>0.5f) || (val<0 && rotateSpeed<-0.5f)))
      rotateSpeed+= val;
    //debug
    //std::cout << rotateSpeed << "\n";
  }

  void changeRotation(){
    if(rotateSpeed!=0){
      oldRotateSpeed = rotateSpeed;
      rotateSpeed = 0;
    }
    else
      rotateSpeed = oldRotateSpeed;

    spaceReleased = 0;
    
  }

  void moveObject(int dir){
    //check if object stay in some border and translate
    if(dir==MOVE_RIGHT)
      if(!(_vertices.at(6) + MOVE_SPEED> 1.0f))
        for (int i=0; i<_vertices.size(); i+=3)
          _vertices.at(i)+=MOVE_SPEED;
    if(dir==MOVE_LEFT)
      if(!(_vertices.at(6) - MOVE_SPEED< -1.0f))
        for (int i=0; i<_vertices.size(); i+=3)
          _vertices.at(i)-=MOVE_SPEED;
    if(dir==MOVE_TOP)
      if(!(_vertices.at(7) + MOVE_SPEED> 1.0f))
        for (int i=1; i<_vertices.size(); i+=3)
          _vertices.at(i)+=MOVE_SPEED;
    if(dir==MOVE_BOTTOM)
      if(!(_vertices.at(7) - MOVE_SPEED< -1.0f))
        for (int i=1; i<_vertices.size(); i+=3)
          _vertices.at(i)-=MOVE_SPEED;
  }
  void rotateObject(float angle){
    float x, y;
    //centro do catavento
    float x1 = _vertices.at(6);
    float y1 = _vertices.at(7);
    for (int i=0; i<_vertices.size(); i+=3){
      //translado o catavento para o centro
      x = _vertices.at(i) - x1;
      y = _vertices.at(i+1) - y1;
      //realizo a rotação e translado para a posição de origem
      _vertices.at(i)=(x*cos(angle) - y*sin(angle)) + x1;
      _vertices.at(i+1)=(x*sin(angle) + y*cos(angle)) + y1;
    }
  }
  void changeScene(){
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _vertices.size(), _vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  void prepare() {
    float array[] = {
                      -0.15f, 0.0f, 0.0f,   //left triangle1 ok
                      0.0f, -0.1f, 0.0f,    //right
                      0.0f, 0.0f, 0.0f, //top
                      0.0f, 0.15f, 0.0f,   //left triangle2
                      -0.1f, 0.0f, 0.0f,    //right
                      0.0f, 0.0f, 0.0f, //top
                      0.15f, 0.0f, 0.0f,   //left triangle3
                      0.0f, 0.1f, 0.0f,    //right
                      0.0f, 0.0f, 0.0f, //top
                      0.0f, -0.15f, 0.0f,   //left triangle4
                      0.1f, 0.0f, 0.0f,    //right
                      0.0f, 0.0f, 0.0f //top
                    };
    _vertices.insert(_vertices.begin(), array, array + 9*4);
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _vertices.size(), _vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  unsigned int loadVertexShader() {
    _vertexShader = "#version 330 core\n"
                    "layout(location = 0) in vec3 aPos;\n"
                    "uniform mat4 transform;\n"
                    "void main() {\n"
                    "gl_Position = transform * vec4(aPos, 1.0f);\n"
                    "}\0";

    unsigned int vertexShaderId;
    vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderId, 1, &_vertexShader, NULL);
    glCompileShader(vertexShaderId);

    int succes;
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &succes);
    if (!succes) {
      char infoLog[512];
      glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
      //debug
      //std::cerr << "Vertex shader error: " << infoLog << std::endl;
    }
    return vertexShaderId;
  }

  unsigned int loadFragmentShader() {
    const char *shaderSource = "#version 330 core\n"
                               "out vec4 FragColor;\n"
                               "void main()\n"
                               "{\n"
                               "   FragColor = vec4(0.6f, 0.8f, 0.8f, 1.0f);\n"
                               "}\n\0";
    unsigned int fragmentShaderId;
    fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderId, 1, &shaderSource, NULL);
    glCompileShader(fragmentShaderId);

    int success = 0;
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
      char infoLog[512];
      glGetShaderInfoLog(fragmentShaderId, 512, NULL, infoLog);
      std::cout << "Shader source: " << shaderSource << std::endl;
      std::cerr << "Fragment shader error: " << infoLog << std::endl;
    }
    return fragmentShaderId;
  }

  void loadShaders() {
    unsigned int vshader, fshader;
    vshader = loadVertexShader();
    fshader = loadFragmentShader();

    shaderProgramId = glCreateProgram();
    glAttachShader(shaderProgramId, fshader);
    glAttachShader(shaderProgramId, vshader);
    glLinkProgram(shaderProgramId);

    int success;
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);
    if (!success) {
      char infoLog[512];
      glGetProgramInfoLog(shaderProgramId, 512, NULL, infoLog);
      std::cerr << "Program shader error: " << infoLog << std::endl;
    }

    glDeleteShader(vshader);
    glDeleteShader(fshader);
  }

protected:
  std::vector<float> _vertices;
  const char *_vertexShader;
  int shaderProgramId;
  unsigned int _VBO, _VAO;
  float rotateSpeed = DEFAULT_ROTATE_SPEED, oldRotateSpeed = DEFAULT_ROTATE_SPEED;
  int stateW, stateS, stateA, stateD;
  int stateSpace, stateQ, stateE;
  int spaceReleased = 1;
};

int main(int argc, char const *argv[]) {
  Demo window;
  window.createWindow();
  window.loadShaders();
  window.prepare();
  window.run();
}