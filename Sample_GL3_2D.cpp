#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <list>
#include "tile_info.h"

using namespace std;


struct VAO {
    GLuint VertexArrayID;
    GLuint VertexBuffer;
    GLuint ColorBuffer;

    GLenum PrimitiveMode;
    GLenum FillMode;
    int NumVertices;
};
typedef struct VAO VAO;

struct GLMatrices {
//    glm::mat4 projection;
    glm::mat4 projectionO, projectionP;
    glm::mat4 model;
    glm::mat4 view;
    GLuint MatrixID;
} Matrices;

int proj_type;
int do_rot, floor_rel;;
GLuint programID;
double last_update_time, current_time;
glm::vec3 rect_pos, floor_pos;
float rectangle_rotation = 0;

float tile_width=0.5,tile_height=0.25; //if change here, change in struct tile
typedef struct tile {
    float x,y,z;
    static const float width=0.5,height=0.25;
    int type;
    VAO* object;
}tile;
list<tile> tiles_on_display;

typedef struct cubeS {
    float x,y,z;
    float width,height;
    float rotationX,rotationY;
    VAO* object;
}cubeS;
cubeS cube1,cube2;
float cuboid_lengthX,cuboid_lengthY,cuboid_lengthZ;

//extern int tile_for_level[3][11][11];


typedef struct score_boardS {
    int score,level;
    float time;
}score_boardS;
score_boardS score_board;

typedef struct boardS {
    static const float x=-3.5;
    static const float y=-3.5;
    static const float z=0.0;
}boardS;
boardS board;

void check_cube_fall();

/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open())
	{
	    std::string Line = "";
	    while(getline(VertexShaderStream, Line))
		VertexShaderCode += "\n" + Line;
	    VertexShaderStream.close();
	}

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
	std::string Line = "";
	while(getline(FragmentShaderStream, Line))
	    FragmentShaderCode += "\n" + Line;
	FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    //    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    //    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

    // Compile Fragment Shader
    //    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    //    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

    // Link the program
    //    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    //    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void quit(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void initGLEW(void){
    glewExperimental = GL_TRUE;
    if(glewInit()!=GLEW_OK){
	fprintf(stderr,"Glew failed to initialize : %s\n", glewGetErrorString(glewInit()));
    }
    if(!GLEW_VERSION_3_3)
	fprintf(stderr, "3.3 version not available\n");
}



/* Generate VAO, VBOs and return VAO handle */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
    struct VAO* vao = new struct VAO;
    vao->PrimitiveMode = primitive_mode;
    vao->NumVertices = numVertices;
    vao->FillMode = fill_mode;

    // Create Vertex Array Object
    // Should be done after CreateWindow and before any other GL calls
    glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
    glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
    glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

    glBindVertexArray (vao->VertexArrayID); // Bind the VAO
    glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
    glVertexAttribPointer(
                          0,                  // attribute 0. Vertices
                          3,                  // size (x,y,z)
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );

    glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
    glVertexAttribPointer(
                          1,                  // attribute 1. Color
                          3,                  // size (r,g,b)
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );

    return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
    GLfloat* color_buffer_data = new GLfloat [3*numVertices];
    for (int i=0; i<numVertices; i++) {
        color_buffer_data [3*i] = red;
        color_buffer_data [3*i + 1] = green;
        color_buffer_data [3*i + 2] = blue;
    }

    return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}

/* Render the VBOs handled by VAO */
void draw3DObject (struct VAO* vao)
{
    // Change the Fill Mode for this object
    glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

    // Bind the VAO to use
    glBindVertexArray (vao->VertexArrayID);

    // Enable Vertex Attribute 0 - 3d Vertices
    glEnableVertexAttribArray(0);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

    // Enable Vertex Attribute 1 - Color
    glEnableVertexAttribArray(1);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

    // Draw the geometry !
    glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}

/**************************
 * Customizable functions *
 **************************/

float rectangle_rot_dir = 1;
bool rectangle_rot_status = true;

/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */
glm::mat4 rotationOverallCube = glm::mat4(1.0f);
    //rotation of cube1
glm::mat4 rotateCube = glm::mat4(1.0f);
glm::mat4 translateCubeEdge = glm::mat4(1.0f);
glm::mat4 translateCube1EdgeR = glm::mat4(1.0f);
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Function is called first on GLFW_PRESS.

    float translateInX, translateInY, temp;

    if (action == GLFW_RELEASE) {
        switch (key) {
	case GLFW_KEY_C:
	    rectangle_rot_status = !rectangle_rot_status;
	    break;
	case GLFW_KEY_P:
	    break;
	case GLFW_KEY_X:
	    // do something ..
	    break;
	default:
	    break;
        }
    }
    else if (action == GLFW_PRESS) {
        switch (key) {
    case GLFW_KEY_RIGHT:
        cube1.rotationX+=90;
        if (cube1.rotationX>360)
            cube1.rotationX=90;
        cube1.x+=cuboid_lengthX;

        //matrices
        rotateCube = glm::rotate((float)(90*M_PI/180.0f), glm::vec3(0,1,0));
        translateInX=cuboid_lengthX;
        translateCubeEdge = glm::translate(glm::vec3(-1*(translateInX),0.0,cube1.height));
        translateCube1EdgeR = glm::translate(glm::vec3(0.0,0.0,-1*cube1.height));
        rotationOverallCube = translateCube1EdgeR * rotateCube * translateCubeEdge * rotationOverallCube;

        //interchange cuboid z x lenghts
        temp=cuboid_lengthZ;
        cuboid_lengthZ=cuboid_lengthX;
        cuboid_lengthX=temp;
        check_cube_fall();

        break;
        case GLFW_KEY_LEFT:
            cube1.rotationX-=90;
            if (cube1.rotationX<-360)
                cube1.rotationX=-90;

            //Matrices
            rotateCube = glm::rotate((float)(-90*M_PI/180.0f), glm::vec3(0,1,0));
            translateInX=cuboid_lengthZ;
            translateCubeEdge = glm::translate(glm::vec3(0.0,0.0,cube1.height));
            translateCube1EdgeR = glm::translate(glm::vec3(cuboid_lengthZ,0.0,-1*cube1.height));
            rotationOverallCube = translateCube1EdgeR * rotateCube * translateCubeEdge * rotationOverallCube;


            //interchange cuboid z x lenghts
            temp=cuboid_lengthZ;
            cuboid_lengthZ=cuboid_lengthX;
            cuboid_lengthX=temp;

            //position cube1
            cube1.x-=cuboid_lengthX;
            //position cube2

            //if(cube1.rotationX)
            check_cube_fall();
            break;
        case GLFW_KEY_UP:
            cube1.rotationY-=90;
            if (cube1.rotationY<-360)
                cube1.rotationY=-90;
            cube1.y+=cuboid_lengthY;

            rotateCube = glm::rotate((float)(-90*M_PI/180.0f), glm::vec3(1,0,0));
            translateInY=cuboid_lengthY;
            translateCubeEdge = glm::translate(glm::vec3(0.0,-1*(translateInY),cube1.height));
            translateCube1EdgeR = glm::translate(glm::vec3(0.0,0.0,-1*cube1.height));
            rotationOverallCube = translateCube1EdgeR * rotateCube * translateCubeEdge * rotationOverallCube;

            //interchange cuboid z x lenghts
            temp=cuboid_lengthZ;
            cuboid_lengthZ=cuboid_lengthY;
            cuboid_lengthY=temp;
            check_cube_fall();
            break;
        case GLFW_KEY_DOWN:
            cube1.rotationY+=90;
            if (cube1.rotationY>360)
                cube1.rotationY=90;

            //matrices
            rotateCube = glm::rotate((float)(90*M_PI/180.0f), glm::vec3(1,0,0));
            translateInY=cuboid_lengthZ;
            translateCubeEdge = glm::translate(glm::vec3(0.0,0.0,cube1.height));
            translateCube1EdgeR = glm::translate(glm::vec3(0.0,translateInY,-1*cube1.height));
            rotationOverallCube = translateCube1EdgeR * rotateCube * translateCubeEdge * rotationOverallCube;
            //interchange cuboid z x lenghts
            temp=cuboid_lengthZ;
            cuboid_lengthZ=cuboid_lengthY;
            cuboid_lengthY=temp;
            //position cube1
            cube1.y-=cuboid_lengthY;
            check_cube_fall();
            break;

        case GLFW_KEY_ESCAPE:
	    quit(window);
	    break;
	default:
	    break;
        }
    }
}

/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
    switch (key) {
    case 'Q':
    case 'q':
	quit(window);
	break;
    case 'a':
	rect_pos.x -= 0.1;
	break;
    case 'd':
	rect_pos.x += 0.1;
	break;
    case 'w':
	rect_pos.y += 0.1;
	break;
    case 's':
	rect_pos.y -= 0.1;
	break;
    case 'r':
	rect_pos.z -= 0.1;
	break;
    case 'f':
	rect_pos.z += 0.1;
	break;
    case 'e':
	rectangle_rotation += 1;
	break;
    case 'j':
	floor_pos.x -= 0.1;
	break;
    case 'l':
	floor_pos.x += 0.1;
	break;
    case 'i':
	floor_pos.y += 0.1;
	break;
    case 'k':
	floor_pos.y -= 0.1;
	break;
    case 'y':
	floor_pos.z -= 0.1;
	break;
    case 'h':
	floor_pos.z += 0.1;
	break;
    case 'g':
	floor_rel ^= 1;
	break;
    case ' ':
	do_rot ^= 1;
	break;
    case 't':
	proj_type ^= 1;
	break;
    default:
	break;
    }
}

/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
    switch (button) {
    case GLFW_MOUSE_BUTTON_RIGHT:
	if (action == GLFW_RELEASE) {
	    rectangle_rot_dir *= -1;
	}
	break;
    default:
	break;
    }
}


/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (GLFWwindow* window, int width, int height)
{
    int fbwidth=width, fbheight=height;
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);

    GLfloat fov = M_PI/2;

    // sets the viewport of openGL renderer
    glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

    // Store the projection matrix in a variable for future use
    // Perspective projection for 3D views
    Matrices.projectionP = glm::perspective(fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);

    // Ortho projection for 2D views
    Matrices.projectionO = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 500.0f);
}

VAO *rectangle, *cam, *floor_vao;

// Creates the rectangle object used in this sample code
void createRectangle ()
{
    // GL3 accepts only Triangles. Quads are not supported
    static const GLfloat vertex_buffer_data [] = {
	-0.5, 0.5, 0.5,
	-0.5, -0.5, 0.5,
	0.5, -0.5, 0.5,
	-0.5, 0.5, 0.5,
	0.5, -0.5, 0.5,
	0.5, 0.5, 0.5,
	0.5, 0.5, 0.5,
	0.5, -0.5, 0.5,
	0.5, -0.5, -0.5,
	0.5, 0.5, 0.5,
	0.5, -0.5, -0.5,
	0.5, 0.5, -0.5,
	0.5, 0.5, -0.5,
	0.5, -0.5, -0.5,
	-0.5, -0.5, -0.5,
	0.5, 0.5, -0.5,
	-0.5, -0.5, -0.5,
	-0.5, 0.5, -0.5,
	-0.5, 0.5, -0.5,
	-0.5, -0.5, -0.5,
	-0.5, -0.5, 0.5,
	-0.5, 0.5, -0.5,
	-0.5, -0.5, 0.5,
	-0.5, 0.5, 0.5,
	-0.5, 0.5, -0.5,
	-0.5, 0.5, 0.5,
	0.5, 0.5, 0.5,
	-0.5, 0.5, -0.5,
	0.5, 0.5, 0.5,
	0.5, 0.5, -0.5,
	-0.5, -0.5, 0.5,
	-0.5, -0.5, -0.5,
	0.5, -0.5, -0.5,
	-0.5, -0.5, 0.5,
	0.5, -0.5, -0.5,
	0.5, -0.5, 0.5,
	-0.5, 0.5, 0.5,
	0.5, 0.5, -0.5,
	0.5, 0.75, -0.5,
    };

    static const GLfloat color_buffer_data [] = {
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0, 0, 0,
	0, 0, 0,
	1, 1, 1,
    };

    // create3DObject creates and returns a handle to a VAO that can be used later
    rectangle = create3DObject(GL_TRIANGLES, 13*3, vertex_buffer_data, color_buffer_data, GL_FILL);
}

VAO* createTile (tile *curr_tile)
{
    float x=curr_tile->x,y=curr_tile->y,z=curr_tile->z;
    float w=curr_tile->width,h=curr_tile->height;
    // GL3 accepts only Triangles. Quads are not supported
    static const GLfloat vertex_buffer_data [] = {
	0.0, w, 0.0, //1
	0.0, 0.0, 0.0, //2
	w, 0.0, 0.0,  //3
    0.0, w, 0.0, //1
    w, 0.0, 0.0,  //3
    w, w, 0.0,  //4
    w, w, 0.0,  //4
    w, 0.0, 0.0,  //3
    w, 0.0, -1*h,  //5
    w, w, 0.0,  //4
    w, 0.0, -1*h,  //5
    w, w, -1.0*h,  //6
    w, w, -1.0*h,  //6
    w, 0.0, -1*h,  //5
    0.0, 0.0, -1*h,  //7
    w, w, -1.0*h,  //6
    0.0, 0.0, -1*h,  //7
	0.0, w, -1*h,  //8
    0.0, w, -1*h,  //8
    0.0, 0.0, -1*h,  //7
    0.0, 0.0, 0.0, //2
    0.0, w, -1*h,  //8
    0.0, 0.0, 0.0, //2
    0.0, w, 0.0, //1
    0.0, w, -1*h,  //8
    0.0, w, 0.0, //1
    w, w, 0.0,  //4
    0.0, w, -1*h,  //8
    w, w, 0.0,  //4
    w, w, -1.0*h,  //6
    0.0, 0.0, 0.0, //2
    0.0, 0.0, -1*h,  //7
    w, 0.0, -1*h,  //5
    0.0, 0.0, 0.0, //2
    w, 0.0, -1*h,  //5
    w, 0.0, 0.0,  //3
	};

    static const GLfloat color_buffer_data [] = {
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0, 0, 0,
	0, 0, 0,
	1, 1, 1,
    };

    // create3DObject creates and returns a handle to a VAO that can be used later
    return create3DObject(GL_TRIANGLES, 12*3, vertex_buffer_data, color_buffer_data, GL_FILL);
}

VAO* createCube (cubeS* curr_cube)
{
    float w=curr_cube->width,h=curr_cube->height;
    // GL3 accepts only Triangles. Quads are not supported
    static const GLfloat vertex_buffer_data [] = {
	0.0, w, 0.0, //1
	0.0, 0.0, 0.0, //2
	w, 0.0, 0.0,  //3
    0.0, w, 0.0, //1
    w, 0.0, 0.0,  //3
    w, w, 0.0,  //4
    w, w, 0.0,  //4
    w, 0.0, 0.0,  //3
    w, 0.0, -1*h,  //5
    w, w, 0.0,  //4
    w, 0.0, -1*h,  //5
    w, w, -1.0*h,  //6
    w, w, -1.0*h,  //6
    w, 0.0, -1*h,  //5
    0.0, 0.0, -1*h,  //7
    w, w, -1.0*h,  //6
    0.0, 0.0, -1*h,  //7
	0.0, w, -1*h,  //8
    0.0, w, -1*h,  //8
    0.0, 0.0, -1*h,  //7
    0.0, 0.0, 0.0, //2
    0.0, w, -1*h,  //8
    0.0, 0.0, 0.0, //2
    0.0, w, 0.0, //1
    0.0, w, -1*h,  //8
    0.0, w, 0.0, //1
    w, w, 0.0,  //4
    0.0, w, -1*h,  //8
    w, w, 0.0,  //4
    w, w, -1.0*h,  //6
    0.0, 0.0, 0.0, //2
    0.0, 0.0, -1*h,  //7
    w, 0.0, -1*h,  //5
    0.0, 0.0, 0.0, //2
    w, 0.0, -1*h,  //5
    w, 0.0, 0.0,  //3
	};

    static const GLfloat color_buffer_data [] = {
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0, 0, 0,
	0, 0, 0,
	1, 1, 1,
    };

    // create3DObject creates and returns a handle to a VAO that can be used later
    return create3DObject(GL_TRIANGLES, 12*3, vertex_buffer_data, color_buffer_data, GL_FILL);
}


void createCam ()
{
    // GL3 accepts only Triangles. Quads are not supported
    static const GLfloat vertex_buffer_data [] = {
	-0.1, 0, 0,
	0.1, 0, 0,
	0, 0.1, 0,
    };

    static const GLfloat color_buffer_data [] = {
	1, 1, 1,
	1, 1, 1,
	1, 1, 1,
    };

    // create3DObject creates and returns a handle to a VAO that can be used later
    cam = create3DObject(GL_TRIANGLES, 1*3, vertex_buffer_data, color_buffer_data, GL_LINE);
}
void createFloor ()
{
    // GL3 accepts only Triangles. Quads are not supported
    static const GLfloat vertex_buffer_data [] = {
	-2, -1, 2,
	2, -1, 2,
	-2, -1, -2,
	-2, -1, -2,
	2, -1, 2,
	2, -1, -2,
    };

    static const GLfloat color_buffer_data [] = {
	0.65, 0.165, 0.165,
	0.65, 0.165, 0.165,
	0.65, 0.165, 0.165,
	0.65, 0.165, 0.165,
	0.65, 0.165, 0.165,
	0.65, 0.165, 0.165,
    };

    // create3DObject creates and returns a handle to a VAO that can be used later
    floor_vao = create3DObject(GL_TRIANGLES, 2*3, vertex_buffer_data, color_buffer_data, GL_FILL);
}

float camera_rotation_angle = 90;

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw (GLFWwindow* window, float x, float y, float w, float h, int doM, int doV, int doP)
{
    int fbwidth, fbheight;
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);
    glViewport((int)(x*fbwidth), (int)(y*fbheight), (int)(w*fbwidth), (int)(h*fbheight));


    // use the loaded shader program
    // Don't change unless you know what you are doing
    glUseProgram(programID);

    // Eye - Location of camera. Don't change unless you are sure!!
    glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
    //glm::vec3 eye(1,2,2);
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    glm::vec3 target (0, 0, 0);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    glm::vec3 up (0, 1, 0);

    // Compute Camera matrix (view)
    if(doV)
	Matrices.view = glm::lookAt(eye, target, up); // Fixed camera for 2D (ortho) in XY plane
    else
	Matrices.view = glm::mat4(1.0f);

    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    //glm::mat4 VP;
    glm::mat4 VP = (proj_type?Matrices.projectionP:Matrices.projectionO) * Matrices.view;
    //if (doP)
	//VP = Matrices.projection * Matrices.view;
    //else
	//VP = Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    glm::mat4 MVP;	// MVP = Projection * View * Model

    // Load identity to model matrix
    Matrices.model = glm::mat4(1.0f);

    glm::mat4 translateRectangle = glm::translate (rect_pos);        // glTranslatef
    glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1));
    Matrices.model *= (translateRectangle * rotateRectangle);
    if(floor_rel)
	Matrices.model = Matrices.model * glm::translate(floor_pos);
    if(doM)
	MVP = VP * Matrices.model;
    else
	MVP = VP;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

    // draw3DObject draws the VAO given to it using current MVP matrix
    //draw3DObject(rectangle);

    // Load identity to model matrix
    Matrices.model = glm::mat4(1.0f);

    glm::mat4 translateCam = glm::translate(eye);
    glm::mat4 rotateCam = glm::rotate((float)((90 - camera_rotation_angle)*M_PI/180.0f), glm::vec3(0,1,0));
    Matrices.model *= (translateCam * rotateCam);
    MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

    // draw3DObject draws the VAO given to it using current MVP matrix
    draw3DObject(cam);

    list<tile>::iterator i;
    for(i=tiles_on_display.begin();i!=tiles_on_display.end();++i)
    {
        //printf("h");
    // Load identity to model matrix
        Matrices.model = glm::mat4(1.0f);

        glm::mat4 translateTile = glm::translate(glm::vec3(i->x,i->y,i->z));
        //glm::mat4 rotateCam = glm::rotate((float)((90 - camera_rotation_angle)*M_PI/180.0f), glm::vec3(0,1,0));
        Matrices.model *= (translateTile);
        MVP = VP * Matrices.model;
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

        // draw3DObject draws the VAO given to it using current MVP matrix
        draw3DObject((*i).object);
    }

    //cube1
    Matrices.model = glm::mat4(1.0f);
    /*glm::mat4 rotationOverallCube = glm::mat4(1.0f);
    //rotation of cube1
    glm::mat4 rotateCube = glm::mat4(1.0f);
    glm::mat4 translateCubeEdge = glm::mat4(1.0f);
    glm::mat4 translateCube1EdgeR = glm::mat4(1.0f);

    //RIGHT ROTATION
    for(float angle=90; angle<=cube1.rotationX;angle+=90)
    {
        rotateCube = glm::rotate((float)(90*M_PI/180.0f), glm::vec3(0,1,0));
        float translateInX=cube1.width;
        if(angle==180 || angle==360)
            translateInX=2*cube1.width;
        translateCubeEdge = glm::translate(glm::vec3(-1*(translateInX),0.0,cube1.height));
        translateCube1EdgeR = glm::translate(glm::vec3(0.0,0.0,-1*cube1.height));
        rotationOverallCube = translateCube1EdgeR * rotateCube * translateCubeEdge * rotationOverallCube;
    }

    //LEFT ROTATION
    for(float angle=-90; angle>=cube1.rotationX;angle-=90)
    {
        rotateCube = glm::rotate((float)(-90*M_PI/180.0f), glm::vec3(0,1,0));
        float translateInX=0.0;
        if(angle==-180 || angle==-360)
            translateInX=cube1.width;
        translateCubeEdge = glm::translate(glm::vec3((translateInX),0.0,cube1.height));
        translateCube1EdgeR = glm::translate(glm::vec3(cube1.width,0.0,-1*cube1.height));
        rotationOverallCube = translateCube1EdgeR * rotateCube * translateCubeEdge * rotationOverallCube;
    }

    //TOP ROTATION
    for(float angle=-90; angle>=cube1.rotationY;angle-=90)
    {
        rotateCube = glm::rotate((float)(-90*M_PI/180.0f), glm::vec3(1,0,0));
        float translateInY=cube1.width;
        if(angle==-180 || angle==-360)
            translateInY=2*cube1.width;
        translateCubeEdge = glm::translate(glm::vec3(0.0,-1*(translateInY),cube1.height));
        translateCube1EdgeR = glm::translate(glm::vec3(0.0,0.0,-1*cube1.height));
        rotationOverallCube = translateCube1EdgeR * rotateCube * translateCubeEdge * rotationOverallCube;
    }

    //DOWN ROTATION
    for(float angle=90; angle<=cube1.rotationY;angle+=90)
    {
        rotateCube = glm::rotate((float)(90*M_PI/180.0f), glm::vec3(1,0,0));
        float translateInY=0.0;
        if(angle==180 || angle==360)
            translateInY=cube1.width;
        translateCubeEdge = glm::translate(glm::vec3(0.0,(translateInY),cube1.height));
        translateCube1EdgeR = glm::translate(glm::vec3(0.0,cube1.width,-1*cube1.height));
        rotationOverallCube = translateCube1EdgeR * rotateCube * translateCubeEdge * rotationOverallCube;
    }
*/



    glm::mat4 translateCube1 = glm::translate(glm::vec3(cube1.x,cube1.y,cube1.z));
    Matrices.model = translateCube1*rotationOverallCube;
    MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

    // draw3DObject draws the VAO given to it using current MVP matrix
    draw3DObject(cube1.object);

    //cube2
    glm::mat4 translateonCube1 = glm::translate(glm::vec3(0.0,0.0,cube1.width));
    glm::mat4 translateCube2 = glm::translate(glm::vec3(cube1.x,cube1.y,cube1.z));
    Matrices.model = (translateCube2 * rotationOverallCube * translateonCube1);
    MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

    // draw3DObject draws the VAO given to it using current MVP matrix
    draw3DObject(cube2.object);


    Matrices.model = glm::translate(floor_pos);
    MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

    // draw3DObject draws the VAO given to it using current MVP matrix
    //draw3DObject(floor_vao);

}

/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height){
    GLFWwindow* window; // window desciptor/handle

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);

    if (!window) {
	exit(EXIT_FAILURE);
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    //    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval( 1 );
    glfwSetFramebufferSizeCallback(window, reshapeWindow);
    glfwSetWindowSizeCallback(window, reshapeWindow);
    glfwSetWindowCloseCallback(window, quit);
    glfwSetKeyCallback(window, keyboard);      // general keyboard input
    glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling
    glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks

    return window;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height)
{
    /* Objects should be created before any other gl function and shaders */
    // Create the models
    createRectangle ();
    createCam();
    createFloor();

//intitalise scoreboard
    score_board.level=1;
    score_board.score=0;
    score_board.time=0;
//    score_board.life=3;

//    initaliseVariables();
//initialise and create tiles
    int i,j;
    int level=score_board.level;
    float cube_initial_posX=0,cube_initial_posY=0,cube_initial_posZ=0.0;
    for(i=1;i<=10;i++)
        for(j=1;j<=10;j++)
        {
            //printf("%d",tile_for_level[level][i][j] );
            if(tile_for_level[level][i][j])
            {
                //printf("f\n");
                int tile_num=tile_for_level[level][i][j];
                tile* new_tile=new tile;
                new_tile->x=board.x+(new_tile->width)*(i-1);
                new_tile->y=board.y+(new_tile->width)*(j-1);
                new_tile->z=board.z;
                //(new_tile.color_top)=(tile_info[tile_num].color_top);
                //(new_tile.color_base)=(tile_info[tile_num].color_base);
                //(new_tile.color_side)=(tile_info[tile_num].color_side);
                new_tile->type=tile_num;
                new_tile->object=createTile(new_tile);
                tiles_on_display.push_back(*new_tile);
                if(cube_initial_posX==0.0 && cube_initial_posY==0.0 && cube_initial_posZ==0.0)
                {
                    cube_initial_posX=new_tile->x;
                    cube_initial_posY=new_tile->y;
                    cube_initial_posZ=new_tile->z;
                }
            }
        }

//cube1
    cube1.x=cube_initial_posX;
    cube1.y=cube_initial_posY;
    cube1.width=tile_width;
    cube1.height=tile_width;
    cube1.z=cube_initial_posZ+cube1.height;
    cube1.rotationX=0.0;
    cube1.rotationY=0.0;
    cube1.object=createCube(&cube1);

//cube2
    cube2.x=cube_initial_posX;
    cube2.y=cube_initial_posY;
    cube2.z=cube_initial_posZ+tile_width;
    cube2.width=tile_width;
    cube2.height=tile_width;
    cube2.object=createCube(&cube2);

    //cuboid
        cuboid_lengthX=cube1.width;
        cuboid_lengthY=cube1.width;
        cuboid_lengthZ=2*cube1.width;

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
    // Get a handle for our "MVP" uniform
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


    reshapeWindow (window, width, height);

    // Background color of the scene
    glClearColor (0.3f, 0.3f, 0.3f, 0.0f); // R, G, B, A
    glClearDepth (1.0f);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);

    // cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    // cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    // cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    // cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

void check_cube_fall()
{
    list<tile>::iterator i = tiles_on_display.begin();
    bool fall=true;
    for(;i!=tiles_on_display.end();++i)
    {
        if( cube1.x == i->x && cube1.y == i->y )
        {
                //cube on a tile
                fall=false;
                break;
        }
        else if( cuboid_lengthX > tile_width )
        {
            if( cube1.x + tile_width == i->x && cube1.y == i->y )
            {
                //cube in tile
                fall=false;
                break;
            }
        }
        else if( cuboid_lengthY > tile_width )
        {
            if( cube1.y + tile_width == i->y && cube1.x == i->x )
            {
                //cube in tile
                fall=false;
                break;
            }
        }
    }

    if(fall==true)
    {
        //life--;
        //start again
        terminate();
    }
}

int main (int argc, char** argv)
{
    int width = 600;
    int height = 600;
    proj_type = 1;
    rect_pos = glm::vec3(0, 0, 0);
    floor_pos = glm::vec3(0, 0, 0);
    do_rot = 0;
    floor_rel = 1;

    GLFWwindow* window = initGLFW(width, height);
    initalise_tile_for_levels();
    initGLEW();
    initGL (window, width, height);

    last_update_time = glfwGetTime();
    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {

	// clear the color and depth in the frame buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // OpenGL Draw commands
	current_time = glfwGetTime();
	if(do_rot)
	    camera_rotation_angle += 90*(current_time - last_update_time); // Simulating camera rotation
	if(camera_rotation_angle > 720)
	    camera_rotation_angle -= 720;
	last_update_time = current_time;
	draw(window, 0, 0, 1.0, 1.0, 1, 1, 1);

        // Swap Frame Buffer in double buffering
        glfwSwapBuffers(window);

        // Poll for Keyboard and mouse events
        glfwPollEvents();
    }

    glfwTerminate();
    //    exit(EXIT_SUCCESS);
}
