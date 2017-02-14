#define GLM_FORCE_RADIANS
#include "tile_info.h"
#include "header.h"
#include <ao/ao.h>
#include <mpg123.h>
#define BITS 8;
using namespace std;

mpg123_handle *mh;
unsigned char *buffer;
size_t buffer_size;
size_t done;
int err;

int driver;
ao_device *dev;

ao_sample_format format;
int channels, encoding;
long rate;


void audio_init() {
    /* initializations */
    ao_initialize();
    driver = ao_default_driver_id();
    mpg123_init();
    mh = mpg123_new(NULL, &err);
    buffer_size= 3000;
    buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

    /* open the file and get the decoding format */
    mpg123_open(mh, "./207-miracle-moon.mp3");
    mpg123_getformat(mh, &rate, &channels, &encoding);

    /* set the output format and open the output device */
    format.bits = mpg123_encsize(encoding) * BITS;
    format.rate = rate;
    format.channels = channels;
    format.byte_format = AO_FMT_NATIVE;
    format.matrix = 0;
    dev = ao_open_live(driver, &format, NULL);
}


void audio_play() {
    /* decode and play */
    if (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
        ao_play(dev, (char*) buffer, done);
    else mpg123_seek(mh, 0, SEEK_SET);
}

void audio_close() {
    /* clean up */
    free(buffer);
    ao_close(dev);
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
    ao_shutdown();
}

struct VAO {
    GLuint VertexArrayID;
    GLuint VertexBuffer;
    GLuint ColorBuffer;

    GLenum PrimitiveMode;
    GLenum FillMode;
    int NumVertices;
};
typedef struct VAO VAO;
GLFWwindow* window;
GLFWwindow* windowS;
struct GLMatrices {
//    glm::mat4 projection;
    glm::mat4 projectionO, projectionP;
    glm::mat4 model;
    glm::mat4 view;
    GLuint MatrixID;
} Matrices;

typedef struct COLOR
{
	float r;
	float g;
	float b;
}Color;

typedef struct Structure {
	string name;
	COLOR color;
	float x,y;
	VAO* object;
	int status;
	float height,width;
	int tone;
}Structure;

map <string, Structure> TEXT;

typedef struct star_t {
	int key;
	float X,Y;
	VAO * star;
}stars;

int countstar=2;

stars stararr[4];


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
int direction;
float rotation,decrement,x,y;
bool win;
bool mouse_left_pressed, mouse_right_pressed;
//extern int tile_for_level[3][11][11];


typedef struct score_boardS {
    int score,level,life,moves;
    float time;
}score_boardS;
score_boardS score_board;

typedef struct boardS {
    static const float x=-3.5;
    static const float y=-3.5;
    static const float z=0.0;
}boardS;
boardS board;

typedef struct cameraI {
    float Ex,Ey,Ez,Tx,Ty,Tz;
}cameraI;
cameraI camV;

//rotation of cube1
glm::mat4 rotationOverallCube;
glm::mat4 rotateCube;
glm::mat4 translateCubeEdge;
glm::mat4 translateCube1EdgeR;

void check_cube_fall(bool);
bool bridge_button_pressed;
void mousescroll(GLFWwindow* window, double xoffset, double yoffset);


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
        score_board.moves+=1;
        direction=1;
        break;
        case GLFW_KEY_LEFT:
            score_board.moves+=1;
            direction=2;
            break;
        case GLFW_KEY_UP:
            score_board.moves+=1;
            direction=3;
            break;
        case GLFW_KEY_DOWN:
            score_board.moves+=1;
            direction=4;
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
int viewT;
void keyboardChar (GLFWwindow* window, unsigned int key)
{
    switch (key) {
    case 'Q':
    case 'q':
	quit(window);
	break;
    case 'j':
    case 'J':
    direction=-1;
    break;
    case 'k':
    case 'K':
    direction=-2;
    break;
    case 'u':
    case 'U':
    direction=-3;
    break;
    case 'i':
    case 'I':
    direction=-4;
    break;
    case 'b':
    case 'B':
    //block view
    viewT=2;
    break;
    case 'n':
    case 'N':
    //normal view
    viewT=1;
    break;
    case 'o':
    case 'O':
    //top down
    viewT=3;
    break;
    case 'p':
    case 'P':
    //tower
    viewT=4;
    break;
    case 'f':
    case'F':
    //follow cam
    viewT=5;
    break;
    case ' ':
	do_rot ^= 1;
	break;
    case 't':
    case 'T':
	proj_type ^= 1;
	break;
    default:
	break;
    }
}

/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
    double mouse_pos_x, mouse_pos_y;
    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
	if (action == GLFW_PRESS) {
        mouse_left_pressed=true;
        glfwGetCursorPos(window, &mouse_pos_x, &mouse_pos_y);
        mouse_pos_x=(mouse_pos_x-120)*0.5/30-3.0f;
        mouse_pos_y=(-1*mouse_pos_y+481)*0.5/30-3.0f;
        int col=(mouse_pos_x+3.5f)/tile_width;
        x = col*tile_width - 3.5f;
        int row=(mouse_pos_y+3.5f)/tile_width;
        y = row*tile_width - 3.5f;
        direction=-5;
        rotate();
        //printf("%lf %f %d\n",mouse_pos_y,cube1.y+tile_width,col );
        }
    else
    {
        mouse_left_pressed=false;
    }
    break;
    case GLFW_MOUSE_BUTTON_RIGHT:
    if(action == GLFW_PRESS)
    {
        mouse_right_pressed=true;
        //helicopter view
        viewT=6;
    }
    else
        mouse_right_pressed=false;


	break;
    default:
	break;
    }
}

void mousescroll(GLFWwindow* window, double xoffset, double yoffset)
{
    //helicopter
    viewT=6;
    if(xoffset==-1 || yoffset==-1)
        camV.Ez-=0.1;
    if(xoffset==1 || yoffset==1)
        camV.Ez+=0.1;
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

void createRectangleScore (string name, int tone, COLOR colorA, COLOR colorB, COLOR colorC, COLOR colorD, float x, float y, float height, float width, string component)
{
	// GL3 accepts only Triangles. Quads are not supported
	float w=width/2,h=height/2;
	GLfloat vertex_buffer_data [] = {
		-w,-h,0, // vertex 1
		-w,h,0, // vertex 2
		w,h,0, // vertex 3

		w,h,0, // vertex 3
		w,-h,0, // vertex 4
		-w,-h,0  // vertex 1
	};

	GLfloat color_buffer_data [] = {
		colorA.r,colorA.g,colorA.b, // color 1
		colorB.r,colorB.g,colorB.b, // color 2
		colorC.r,colorC.g,colorC.b, // color 3

		colorC.r,colorC.g,colorC.b, // color 4
		colorD.r,colorD.g,colorD.b, // color 5
		colorA.r,colorA.g,colorA.b // color 6
	};

	// create3DObject creates and returns a handle to a VAO that can be used later
	VAO *rectangle = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
	Structure vishsprite = {};
	vishsprite.color = colorA;
	vishsprite.name = name;
	vishsprite.object = rectangle;
	vishsprite.x=x;
	vishsprite.y=y;
	vishsprite.height=height;
	vishsprite.width=width;
	vishsprite.status=1;
	vishsprite.tone=tone;

	if(component=="score")
		TEXT[name] = vishsprite;
}

void setStroke(char val){
	TEXT["top"].status=0;
	TEXT["bottom"].status=0;
	TEXT["middle"].status=0;
	TEXT["left1"].status=0;
	TEXT["left2"].status=0;
	TEXT["right1"].status=0;
	TEXT["right2"].status=0;
	TEXT["middle1"].status=0;
	TEXT["middle2"].status=0;
	if( val=='O' || val=='0' || val=='2' || val=='3' || val=='5' || val=='6'|| val=='7' || val=='8' || val=='9'){
		TEXT["top"].status=1;
		//cout << "entry" << endl;
	}
	if( val=='2' || val=='3' || val=='4' || val=='5' || val=='6' || val=='8' || val=='9'){
		TEXT["middle"].status=1;
	}
	if( val=='0' || val=='2' || val=='3' || val=='5' || val=='6' || val=='8' || val=='9'){
		TEXT["bottom"].status=1;
	}
	if( val=='0' || val=='4' || val=='5' || val=='6' || val=='8' || val=='9' ){
		TEXT["left1"].status=1;
	}
	if(val=='0' || val=='2' || val=='6' || val=='8'){
		TEXT["left2"].status=1;
	}
	if(val=='O' || val=='0' || val=='1' || val=='2' || val=='3' || val=='4' || val=='7' || val=='8' || val=='9'){
		TEXT["right1"].status=1;
	}
	if( val=='0' || val=='1' || val=='3' || val=='4' || val=='5' || val=='6' || val=='7' || val=='8' || val=='9'){
		TEXT["right2"].status=1;
	}

}


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

VAO* createStar ()
{
	static const GLfloat vertex_buffer_data [] = {
		0,-0.2,0, // vertex 1
		-0.2,0.0,0, // vertex 2
		0, 0.2,0, // vertex 3

		0, 0.2,0, // vertex 3
		0.2, 0.0,0, // vertex 4
		0.,-0.2,0  // vertex 1
	};

	static const GLfloat color_buffer_data [] = {
		255.0f/255.0f, 153.0f/255.0f, 0.0f/255.0f, // color 1
		255.0f/255.0f, 153.0f/255.0f, 0.0f/255.0f, // color 2
		255.0f/255.0f, 153.0f/255.0f, 0.0f/255.0f, // color 4

		255.0f/255.0f, 153.0f/255.0f, 0.0f/255.0f, // color 1
		255.0f/255.0f, 153.0f/255.0f, 0.0f/255.0f, // color 2
		255.0f/255.0f, 153.0f/255.0f, 0.0f/255.0f
	};
	return create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}

VAO* createTile (tile *curr_tile)
{
    float x=curr_tile->x,y=curr_tile->y,z=curr_tile->z;
    float w=curr_tile->width-0.01,h=curr_tile->height;
    int type=curr_tile->type;
    // GL3 accepts only Triangles. Quads are not supported
    static const GLfloat vertex_buffer_data [] = {
    //Top
	0.0, w, 0.0, //1
	0.0, 0.0, 0.0, //2
	w, 0.0, 0.0,  //3
    0.0, w, 0.0, //1
    w, 0.0, 0.0,  //3
    w, w, 0.0,  //4
    //Right
    w, w, 0.0,  //4
    w, 0.0, 0.0,  //3
    w, 0.0, -1*h,  //5
    w, w, 0.0,  //4
    w, 0.0, -1*h,  //5
    w, w, -1.0*h,  //6
    //Bottom
    w, w, -1.0*h,  //6
    w, 0.0, -1*h,  //5
    0.0, 0.0, -1*h,  //7
    w, w, -1.0*h,  //6
    0.0, 0.0, -1*h,  //7
	0.0, w, -1*h,  //8
    //Left
    0.0, w, -1*h,  //8
    0.0, 0.0, -1*h,  //7
    0.0, 0.0, 0.0, //2
    0.0, w, -1*h,  //8
    0.0, 0.0, 0.0, //2
    0.0, w, 0.0, //1
    //Up
    0.0, w, -1*h,  //8
    0.0, w, 0.0, //1
    w, w, 0.0,  //4
    0.0, w, -1*h,  //8
    w, w, 0.0,  //4
    w, w, -1.0*h,  //6
    //Down
    0.0, 0.0, 0.0, //2
    0.0, 0.0, -1*h,  //7
    w, 0.0, -1*h,  //5
    0.0, 0.0, 0.0, //2
    w, 0.0, -1*h,  //5
    w, 0.0, 0.0,  //3
	};

    float base[3],top[3],side[3];
    //printf("type-%d\n",type );
    base[0]=tile_info[type].color_base[0];
    base[1]=tile_info[type].color_base[1];
    base[2]=tile_info[type].color_base[2];

    top[0]=tile_info[type].color_top[0];
    top[1]=tile_info[type].color_top[1];
    top[2]=tile_info[type].color_top[2];

    side[0]=tile_info[type].color_side[0];
    side[1]=tile_info[type].color_side[1];
    side[2]=tile_info[type].color_side[2];
    //if(type==3)
    //printf("%f %f %f, %f %f %f, %f %f %f", base[0],base[1],base[2],top[1],top[1],top[2],side[0],side[1],side[2]);
    GLfloat color_buffer_data [] = {
    //Top
    base[0],base[1],base[2],
	base[0],base[1],base[2],
	base[0],base[1],base[2],
	base[0],base[1],base[2],
	base[0],base[1],base[2],
	base[0],base[1],base[2],
    //Right
	side[0],side[1],side[2],
	side[0],side[1],side[2],
	side[0],side[1],side[2],
	side[0],side[1],side[2],
	side[0],side[1],side[2],
	side[0],side[1],side[2],
    //Bottom
	top[0],top[1],top[2],
    top[0],top[1],top[2],
	top[0],top[1],top[2],
	top[0],top[1],top[2],
	top[0],top[1],top[2],
	top[0],top[1],top[2],
    //Left
    side[0],side[1],side[2],
	side[0],side[1],side[2],
	side[0],side[1],side[2],
	side[0],side[1],side[2],
	side[0],side[1],side[2],
	side[0],side[1],side[2],
    //Up
    side[0],side[1],side[2],
    side[0],side[1],side[2],
    side[0],side[1],side[2],
    side[0],side[1],side[2],
    side[0],side[1],side[2],
    side[0],side[1],side[2],
    //Down
    side[0],side[1],side[2],
    side[0],side[1],side[2],
    side[0],side[1],side[2],
    side[0],side[1],side[2],
    side[0],side[1],side[2],
    side[0],side[1],side[2],
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
double mx,my;
void getView()
{
    //normal view
    if(viewT==1)
    {
        camV.Ex=5*cos(camera_rotation_angle*M_PI/180.0f);
        camV.Ey=0;
        camV.Ez=5*sin(camera_rotation_angle*M_PI/180.0f);
        camV.Tx=0;
        camV.Ty=0;
        camV.Tz=0;
    }
    //block view
    if(viewT==2)
    {
        camV.Ex=cube1.x+cuboid_lengthX;
        camV.Ey=cube1.y+cuboid_lengthY;
        camV.Ez=(cube1.z+cuboid_lengthZ);
        camV.Tx=0;
        camV.Ty=0;
        camV.Tz=0;

    }
    //top down
    if(viewT==3)
    {
        camV.Ex=0;
        camV.Ey=0;
        camV.Ez=5;
        camV.Tx=0;
        camV.Ty=0;
        camV.Tz=0;

    }
    //tower
    if(viewT==4)
    {
        camV.Ex=board.x;
        camV.Ey=board.y;
        camV.Ez=5;
        camV.Tx=0;
        camV.Ty=0;
        camV.Tz=0;

    }
    //follow cam
    if(viewT==5)
    {
        camV.Ex=cube1.x;
        camV.Ey=cube1.y;
        camV.Ez=3;
        camV.Tx=0;
        camV.Ty=0;
        camV.Tz=0;

    }
    //helicopter
    if(viewT==6)
    {
        if(mouse_right_pressed)
        {
            glfwGetCursorPos(window, &mx, &my);
            mx=(mx-120)*0.5/30-3.0f;
            my=(-1*my+481)*0.5/30-3.0f;
        }
        camV.Ex=mx;
        camV.Ey=my;
        //camV.Ez=3;
        camV.Tx=0;
        camV.Ty=0;
        camV.Tz=0;

    }
}

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

    getView();

    // Eye - Location of camera. Don't change unless you are sure!!
    glm::vec3 eye (camV.Ex,camV.Ey,camV.Ez);
    //glm::vec3 eye(1,2,2);
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    glm::vec3 target (camV.Tx,camV.Ty,camV.Tz);
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
        if((i->type!=0 && i->type!=2 && i->type!=5)||(i->type==5 && bridge_button_pressed))
        {
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
    }

    rotate();
    //cube1
    Matrices.model = glm::mat4(1.0f);

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

    //fixed portion
    glm::vec3 eyeF (0,0,5);
    //glm::vec3 eye(1,2,2);
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    glm::vec3 targetF (0,0,0);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    //glm::vec3 up (0, 1, 0);

    // Compute Camera matrix (view)
    if(doV)
	Matrices.view = glm::lookAt(eyeF, targetF, up); // Fixed camera for 2D (ortho) in XY plane
    else
	Matrices.view = glm::mat4(1.0f);

    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    //glm::mat4 VP;
    //VP = (proj_type?Matrices.projectionP:Matrices.projectionO) * Matrices.view;
    VP = Matrices.projectionP * Matrices.view;
    //if (doP)
	//VP = Matrices.projection * Matrices.view;
    //else
	//VP = Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    // MVP = Projection * View * Model


    // draw3DObject draws the VAO given to it using current MVP matrix
    //draw3DObject(floor_vao);
    //score
    int k,score=score_board.moves;
	for(k=1;k<=4;k++)
	{
		float translation;
		float translation1=3.7f;
		if(k==1)
		{
			int temp=score%10;
			setStroke(temp+'0');
			translation=3.4f;
		}
		else if(k==2)
		{
			int temp=(score/10)%10;
			setStroke(temp+'0');
			translation=3.0;
		}
		else if(k==3)
		{
			int temp=(score/100)%10;
			setStroke(temp+'0');
			translation=2.6;
		}
		else if(k==4)
		{
			int temp=(score/1000)%10;
			setStroke(temp+'0');
			translation=2.2;
		}

		for(map<string,Structure>::iterator it=TEXT.begin();it!=TEXT.end();it++){
			string current = it->first;
			if(TEXT[current].status==1)
			{
				//    cout << "1" << endl;
				glm::mat4 MVP;  // MVP = Projection * View * Model

				Matrices.model = glm::mat4(1.0f);


				glm::mat4 ObjectTransform;
				glm::mat4 translateObject = glm::translate (glm::vec3(TEXT[current].x+translation,TEXT[current].y+translation1  ,0.0f)); // glTranslatef
				// glm::mat4 rotateTriangle = glm::rotate((float)((TEXT[current].curr_angle)*M_PI/180.0f), glm::vec3(0,0,1));  // rotate about vector (1,0,0)
				ObjectTransform=translateObject;
				Matrices.model *= ObjectTransform;
				MVP = VP * Matrices.model; // MVP = p * V * M

				glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
				draw3DObject(TEXT[current].object);
			}
		}
	}

    /** LEVEL **/
	for(k=1;k<=1;k++)
	{
		float translation;
		float translation1=3.7f;
		if(k==1)
		{
			int temp=score_board.level;
			setStroke(temp+'0');
			translation=-2.4f;
		}

		for(map<string,Structure>::iterator it=TEXT.begin();it!=TEXT.end();it++)
        {
			string current = it->first;
			if(TEXT[current].status==1)
			{
				//    cout << "1" << endl;
				glm::mat4 MVP;  // MVP = Projection * View * Model

				Matrices.model = glm::mat4(1.0f);


				glm::mat4 ObjectTransform;
				glm::mat4 translateObject = glm::translate (glm::vec3(TEXT[current].x+translation,TEXT[current].y+translation1  ,0.0f)); // glTranslatef
				// glm::mat4 rotateTriangle = glm::rotate((float)((TEXT[current].curr_angle)*M_PI/180.0f), glm::vec3(0,0,1));  // rotate about vector (1,0,0)
				ObjectTransform=translateObject;
				Matrices.model *= ObjectTransform;
				MVP = VP * Matrices.model; // MVP = p * V * M

				glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
				draw3DObject(TEXT[current].object);
			}
		}
	}

    //lifes
    int j;
    for(j=0;j<=2;j++)
	{
		Matrices.model = glm::mat4(1.0f);
		glm::mat4 translateStar = glm::translate (glm::vec3(stararr[j].X, stararr[j].Y, 0));        // glTranslatef
		//     glm::mat4 rotateMirror = glm::rotate((float)(mirarr[i].mirror_rotation*M_PI/180.0f), glm::vec3(0,0,1));
		glm::mat4 StarTransform = translateStar ;
		Matrices.model *= (StarTransform);
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        if(stararr[j].key==0)
        {	draw3DObject(stararr[j].star);}
	}

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
    glfwSetScrollCallback(window, mousescroll);
    return window;
}

void initialiseVariables(bool updateL)
{
    //initialise and create tiles
        int i,j;
        int level=score_board.level;
        float cube_initial_posX=0,cube_initial_posY=0,cube_initial_posZ=0.0;
        win=false;
        direction=0;
        //camera
        viewT=1;
        for(i=1;i<=10;i++)
            for(j=1;j<=10;j++)
            {
                if(tile_for_level[level][i][j])
                {
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
                    if(cube_initial_posX==0.0 && cube_initial_posY==0.0 && cube_initial_posZ==0.0 && new_tile->type!=2)
                    {
                        cube_initial_posX=new_tile->x;
                        cube_initial_posY=new_tile->y;
                        cube_initial_posZ=new_tile->z;
                    }
                }
            }

    bridge_button_pressed=false;

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
        //matrices for cube rotation
        rotationOverallCube=glm::mat4(1.0f);
        rotateCube=glm::mat4(1.0f);
        translateCubeEdge=glm::mat4(1.0f);
        translateCube1EdgeR=glm::mat4(1.0f);

        if(updateL)
            countstar=2;
        for(i=0;i<=countstar;i++)
            stararr[i].key=0;


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
    score_board.life=3;
    score_board.moves=0;

    decrement=0.0f;
    initialiseVariables(true);
    COLOR black = {0/255.0,0/255.0,0/255.0};
    float height1 = 0.02f;
	float width1 = 0.2f;
	createRectangleScore("top",0,black,black,black,black,0,0.2,height1,width1,"score");
	createRectangleScore("bottom",0,black,black,black,black,0,-0.2,height1,width1,"score");
	createRectangleScore("middle",0,black,black,black,black,0,0,height1,width1,"score");
	createRectangleScore("left1",0,black,black,black,black,-0.1,0.1,width1,height1,"score");
	createRectangleScore("left2",0,black,black,black,black,-0.1,-0.1,width1,height1,"score");
	createRectangleScore("right1",0,black,black,black,black,0.1,0.1,width1,height1,"score");
	createRectangleScore("right2",0,black,black,black,black,0.1,-0.1,width1,height1,"score");


    //lifes
    int i;
     for(i=0;i<=2;i++)
     {
         stararr[i].star = createStar();
     }
     stararr[1].X =-3.7f ; stararr[1].Y=3.7f ; stararr[1].key = 0;
     stararr[2].X =-3.3f ; stararr[2].Y=3.7f ; stararr[2].key = 0;
     stararr[0].X =-2.9f ; stararr[0].Y=3.7f ; stararr[0].key = 0;


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

void check_cube_fall(bool fallen)
{
    list<tile>::iterator i = tiles_on_display.begin();
    int col=(cube1.x+3.5f)/tile_width;
    float Cx = col*tile_width - 3.5f;
    int row=(cube1.y+3.5f)/tile_width;
    float Cy = row*tile_width - 3.5f;

    bool fall=true;
    if(!fallen)
    {
        for(;i!=tiles_on_display.end();++i)
        {
        //block in square hole
            if((i->type)==2)
            {
                if( Cx == i->x && Cy == i->y && cuboid_lengthX == tile_width && cuboid_lengthY == tile_width )
                {
                    direction=5;
                    win=true;
                }
            }
            //normal tile
            if(i->type==1 || (i->type==5 && bridge_button_pressed))
            {
                if( Cx == i->x && Cy == i->y )
                {
                    //cube on a tile
                    fall=false;
                    break;
                }
                else if( cuboid_lengthX > tile_width )
                {
                    if( Cx + tile_width == i->x && Cy == i->y )
                    {
                        //cube in tile
                        fall=false;
                        break;
                    }
                }
                else if( cuboid_lengthY > tile_width )
                {
                    if( Cy + tile_width == i->y && Cx == i->x )
                    {
                        //cube in tile
                        fall=false;
                        break;
                    }
                }
            }
            //fragie tile
            if(i->type==3)
            {
                //block vertically on tile
                if(cuboid_lengthY==tile_width && cuboid_lengthX==tile_width && Cx==i->x && Cy==i->y)
                {
                    fall=true;
                    i->type=0;
                    break;
                }
                else if((Cx==i->x && Cy==i->y) || (Cx+(cuboid_lengthX-tile_width))==i->x && (Cy+(cuboid_lengthY-tile_width))==i->y)
                {
                    fall=false;
                    break;
                }
            }
            //bridge button
            if(i->type==4)
            {
                //block vertically on tile
                if(cuboid_lengthY==tile_width && cuboid_lengthX==tile_width && Cx==i->x && Cy==i->y)
                {
                    fall=false;
                    bridge_button_pressed^=true;
                    break;
                }
                else if((Cx==i->x && Cy==i->y) || (Cx+(cuboid_lengthX-tile_width))==i->x && (Cy+(cuboid_lengthY-tile_width))==i->y)
                {
                    fall=false;
                    break;
                }

            }
        }
        if(fall)
            direction=5;

    }
    if(fallen)
    {
        //direction=5;
        score_board.life-=1;
        stararr[countstar].key=1;
        countstar--;
        //start again
        if(score_board.life==0)
            terminate();
        else
        {
            //start again
            initialiseVariables(false);
        }
    }
}

void rotate()
{
    float temp,translateInX,translateInY,d=0.0625;
    //rotate right
    if(direction==1)
    {
        cube1.rotationX+=10;
        //matrices
       rotateCube = glm::rotate((float)(10*M_PI/180.0f), glm::vec3(0,1,0));
       translateInX=cuboid_lengthX;
       translateCubeEdge = glm::translate(glm::vec3(-1*(translateInX),0.0,cube1.height));
       if(cube1.rotationX==90)
        translateCube1EdgeR = glm::translate(glm::vec3(0.0,0.0,-1*cube1.height));
       else
        translateCube1EdgeR = glm::translate(glm::vec3((translateInX),0.0,-1*cube1.height));
       rotationOverallCube = translateCube1EdgeR * rotateCube * translateCubeEdge * rotationOverallCube;

       if (cube1.rotationX>=90)
       {
           cube1.rotationX=0;
           direction=0;
           cube1.x+=cuboid_lengthX;
           //interchange cuboid z x lenghts
          temp=cuboid_lengthZ;
          cuboid_lengthZ=cuboid_lengthX;
          cuboid_lengthX=temp;
          check_cube_fall(false);
       }
    }
    //rotate left
    if(direction==2)
    {
        cube1.rotationX-=10;
        //Matrices
        rotateCube = glm::rotate((float)(-10*M_PI/180.0f), glm::vec3(0,1,0));
        translateInX=cuboid_lengthZ;
        translateCubeEdge = glm::translate(glm::vec3(0.0,0.0,cube1.height));
        if(cube1.rotationX==-90)
            translateCube1EdgeR = glm::translate(glm::vec3(cuboid_lengthZ,0.0,-1*cube1.height));
        else
            translateCube1EdgeR = glm::translate(glm::vec3(0.0,0.0,-1*cube1.height));
        rotationOverallCube = translateCube1EdgeR * rotateCube * translateCubeEdge * rotationOverallCube;

        if(cube1.rotationX<=-90)
        {
            cube1.rotationX=0;
            direction=0;
            //interchange cuboid z x lenghts
           temp=cuboid_lengthZ;
           cuboid_lengthZ=cuboid_lengthX;
           cuboid_lengthX=temp;

           //position cube1
           cube1.x-=cuboid_lengthX;
           //position cube2

           //if(cube1.rotationX)
           check_cube_fall(false);

        }

    }

    //rotate up
    if(direction==3)
    {
        cube1.rotationY-=10;
        //matrices
        rotateCube = glm::rotate((float)(-10*M_PI/180.0f), glm::vec3(1,0,0));
        translateInY=cuboid_lengthY;
        translateCubeEdge = glm::translate(glm::vec3(0.0,-1*(translateInY),cube1.height));
        if(cube1.rotationY==-90)
            translateCube1EdgeR = glm::translate(glm::vec3(0.0,0.0,-1*cube1.height));
        else
            translateCube1EdgeR = glm::translate(glm::vec3(0.0,translateInY,-1*cube1.height));
        rotationOverallCube = translateCube1EdgeR * rotateCube * translateCubeEdge * rotationOverallCube;

        if (cube1.rotationY<=-90)
        {
            cube1.rotationY=0;
            direction=0;
            cube1.y+=cuboid_lengthY;

            //interchange cuboid z x lenghts
            temp=cuboid_lengthZ;
            cuboid_lengthZ=cuboid_lengthY;
            cuboid_lengthY=temp;
            check_cube_fall(false);
        }

    }

    //rotate Down
    if(direction==4)
    {
        cube1.rotationY+=10;
        //matrices
        rotateCube = glm::rotate((float)(10*M_PI/180.0f), glm::vec3(1,0,0));
        translateInY=cuboid_lengthZ;
        translateCubeEdge = glm::translate(glm::vec3(0.0,0.0,cube1.height));
        if(cube1.rotationY==90)
           translateCube1EdgeR = glm::translate(glm::vec3(0.0,translateInY,-1*cube1.height));
        else
            translateCube1EdgeR = glm::translate(glm::vec3(0.0,0.0,-1*cube1.height));
        rotationOverallCube = translateCube1EdgeR * rotateCube * translateCubeEdge * rotationOverallCube;

        if(cube1.rotationY>=90)
        {
            cube1.rotationY=0;
            direction=0;
            temp=cuboid_lengthZ;
            cuboid_lengthZ=cuboid_lengthY;
            cuboid_lengthY=temp;
            //position cube1
            cube1.y-=cuboid_lengthY;
            check_cube_fall(false);

        }

    }

    //fall Down
    if(direction==5)
    {
        //matrices
        cube1.z-=0.05;
        if(cube1.z<-5)
            if(win)
                update_level();
            else
                check_cube_fall(true);
    }
    //jump left
    if(direction==-1)
    {
        decrement+=d;
        cube1.x-=d;
        if(decrement==2*tile_width)
        {
            direction=0;
            decrement=0;
            check_cube_fall(false);
        }
    }
    //jump right
    if(direction==-2)
    {
        decrement+=d;
        cube1.x+=d;
        if(decrement==2*tile_width)
        {
            direction=0;
            decrement=0;
            check_cube_fall(false);
        }
    }
    //jump up
    if(direction==-3)
    {
        decrement+=d;
        cube1.y+=d;
        if(decrement==2*tile_width)
        {
            direction=0;
            decrement=0;
            check_cube_fall(false);
        }
    }
    //jump down
    if(direction==-4)
    {
        decrement+=d;
        cube1.y-=d;
        if(decrement==2*tile_width)
        {
            direction=0;
            decrement=0;
            check_cube_fall(false);
        }
    }
    //move with cursor
    if(direction==-5)
    {
        if(cube1.x<x)
            cube1.x+=d;
        else
            cube1.x-=d;

        if(cube1.y<y)
            cube1.y+=d;
        else
            cube1.y-=d;

        if(cube1.x==x && cube1.y==y)
        {
            direction=0;
        }
        check_cube_fall(false);
    }



}

    void update_level()
{
    score_board.level+=1;

    if(score_board.level>2)
        //win
        terminate();
    initialiseVariables(true);
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

    window = initGLFW(width, height);
    initalise_tile_for_levels();
    initGLEW();
    initGL (window, width, height);

    last_update_time = glfwGetTime();
    /* Draw in loop */
    audio_init();
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
    audio_play();
    //rotate();
	draw(window, 0, 0, 1.0, 1.0, 1, 1, 1);

        // Swap Frame Buffer in double buffering
        glfwSwapBuffers(window);

        // Poll for Keyboard and mouse events
        glfwPollEvents();
    }

    glfwTerminate();
    //    exit(EXIT_SUCCESS);
}
