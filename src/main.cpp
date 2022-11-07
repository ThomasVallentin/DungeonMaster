#include "Core/Application.h"


int main(int argc, char* argv[]) {
    Application& app = Application::Init(argc, argv);
    
    app.Run();
    return app.GetExitCode();
}


// glm::mat3 translate(const glm::vec2 &offset) {
//     return glm::mat3(1, 0, offset.x,
//                      0, 1, offset.y,
//                      0, 0, 1);
// }

// glm::mat3 rotate(const float &degrees) {
//     float radians = degrees * M_PI / 180.0;
//     return glm::mat3(std::cos(radians), -std::sin(radians), 0,
//                      std::sin(radians), std::cos(radians), 0,
//                      0, 0, 1);
// }

// glm::mat3 scale(const glm::vec2 &scl) {
//     return glm::mat3(scl.x, 0, 0, 
//                      0, scl.y, 0,
//                      0, 0, 1);
// }


    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 
    // glimac::Program program;
    // if (argc == 3) {
    //     glimac::FilePath applicationPath(argv[0]);
    //     std::cout << applicationPath.dirPath() + "TP2/" + argv[1] << std::endl;
    //     program = loadProgram(applicationPath.dirPath() + "TP2/" + argv[1],
    //                           applicationPath.dirPath() + "TP2/" + argv[2]);
    //     program.use();

    //     printf("#  INFO: Loaded successfully vertex shader \"%s\" and fragment shader \"%s!\n", argv[1], argv[2]);
    // } else {
    //     printf("# ERROR: Not enough shader path have been given. Expected 2, got %d.\n"
    //            "         No shader will be used.\n", argc - 1);
    // }
    // 
    // // Createing and filling the vbo
    // uint32_t vertexBuffer;
    // glGenBuffers(1, &vertexBuffer);

    // float vertices[] = {-0.5f, -0.5f, 0.0f, 1.0f,
    //                      0.5f, -0.5f, 1.0f, 1.0f,
    //                      0.0f,  0.5f, 0.5f, 0.0f
    //                    };
    // glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    // // Creating and filling the vao
    // uint32_t vertexArray;
    // glGenVertexArrays(1, &vertexArray);
    // glBindVertexArray(vertexArray);

    // // Define the locations of the arttibutes stored in the vertex buffer.
    // const char VERTEX_ARRAY_POSITION = 0;
    // const char VERTEX_ARRAY_TEXCOORDS = 1;

    // glEnableVertexAttribArray(VERTEX_ARRAY_POSITION);
    // glEnableVertexAttribArray(VERTEX_ARRAY_TEXCOORDS);

    // glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    // glVertexAttribPointer(VERTEX_ARRAY_POSITION, 2, GL_FLOAT, false, 
    //                       4 * sizeof(float), 0);
    // glVertexAttribPointer(VERTEX_ARRAY_TEXCOORDS, 2, GL_FLOAT, false,
    //                       4 * sizeof(float), (const void*)(2 * sizeof(float)));
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    // std::vector<glm::vec2> translations = {{-0.5, -0.5}, 
    //                                        {-0.5, 0.5},
    //                                        {0.5, -0.5}, 
    //                                        {0.5, 0.5}};
    // std::vector<glm::vec3> colors = {{1.0, 0.0, 0.0}, 
    //                                  {0.0, 1.0, 0.0},
    //                                  {0.0, 0.0, 1.0}, 
    //                                  {1.0, 0.0, 1.0}};

    // std::unique_ptr<glimac::Image> image = glimac::loadImage("/home/tvallentin/Projects/M2/Synthese_Image/TP2/bin/Debug/assets/textures/triforce.png");

    // GLuint triforceTexture;
    // glGenTextures(1, &triforceTexture);
    // glBindTexture(GL_TEXTURE_2D, triforceTexture);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->getWidth(), image->getHeight(), 0, GL_RGBA, GL_FLOAT, image->getPixels());
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glBindTexture(GL_TEXTURE_2D, 0);

    // /* Loop until the user closes the window */
    // while (!glfwWindowShouldClose(window)) {
    //     glClearColor(0.2f, 0.2f, 0.25f, 1.f);
    //     glClear(GL_COLOR_BUFFER_BIT);

    //     glm::mat3 scaleMat = scale({0.25, 0.25});

    //     for (size_t i=0; i < 4 ; i++) {
    //         float time = glfwGetTime() * 100.0;
    //         glm::mat3 rotationMat = rotate((i % 2) ? time : time * -1.0f);
    //         glm::mat3 modelMatrix = scaleMat * rotationMat * translate(translations[i]);

    //         // Setting uniforms
    //         int uModelMatrixLocation = glGetUniformLocation(program.getGLId(), "uModelMatrix");
    //         glUniformMatrix3fv(uModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    //         int uColorLocation = glGetUniformLocation(program.getGLId(), "uColor");
    //         glUniform3f(uColorLocation, colors[i].x, colors[i].y, colors[i].z);
            
    //         glBindTexture(GL_TEXTURE_2D, triforceTexture);
    //         int uTextureLocation = glGetUniformLocation(program.getGLId(), "uTexture");
    //         glUniform1i(uTextureLocation, 0);

    //         glDrawArrays(GL_TRIANGLES, 0, 3);
            
    //         glBindTexture(GL_TEXTURE_2D, triforceTexture);
    //     }

    //     /* Swap front and back buffers */
    //     glfwSwapBuffers(window);
    //     /* Poll for and process events */
    //     glfwPollEvents();
    // }

    // // Freeing the resources
    // glDeleteBuffers(1, &vertexBuffer);
    // glDeleteVertexArrays(1, &vertexArray);
    // glDeleteTextures(1, &triforceTexture);
