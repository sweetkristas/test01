#include "stdafx.h"

#include "win_sfml_lib.h"

#define LOCAL_PI 3.1415926535897932384626433832795

namespace 
{
	std::default_random_engine& get_random_engine()
	{
		std::random_device r;
		static std::default_random_engine e1(r());
		return e1;
	}
}
void renderingThread(sf::Window* window)
{
	// activate the window's context
	window->setActive(true);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	const std::vector<GLfloat> g_vertex_buffer_data{
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f,  1.0f, 0.0f,
	};

	// This will identify our vertex buffer
	GLuint vertexbuffer;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data.data(), GL_STATIC_DRAW);

	// the rendering loop
	while(window->isOpen()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// draw...

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
		glDisableVertexAttribArray(0);

		// end the current frame -- this is a rendering function (it requires the context to be active)
		window->display();
	}
}

const std::string g_script{
	"-- script.lua\n"
	"-- Receives a table, returns the sum of its components.\n"
	"io.write(\"The table the script received has:\\n\");\n"
"x = 0\n"
"for i = 1, #foo do\n"
"print(i, foo[i])\n"
"x = x + foo[i]\n"
"end\n"
"io.write(\"point_direction(0, 0, 3, 3) = \", point_direction(0,0,3,3),\"\\n\")\n"
"io.write(\"point_distance(0, 0, 3, 3) = \", point_distance(0,0,3,3),\"\\n\")\n"
"io.write(\"lengthdir_x(5, 30) = \", lengthdir_x(5, 30), \"\\n\")\n"
"io.write(\"lengthdir_y(5, 30) = \", lengthdir_y(5, 30), \"\\n\")\n"
"io.write(\"choose(1, 'c', 'a', 'b') = \", choose(1, 'c', 'a', 'b'), \"\\n\")\n"
"io.write(\"random_range(5, 30) = \", random_range(5, 30), \"\\n\")\n"
"io.write(\"Returning data back to C\\n\");\n"
"return x\n"
};

static int l_point_direction(lua_State* L) 
{
	// get arguments
	const double x1 = luaL_checknumber(L, 1);
	const double y1 = luaL_checknumber(L, 2);
	const double x2 = luaL_checknumber(L, 3);
	const double y2 = luaL_checknumber(L, 4);
	const double result = atan2(y2 - y1, x2 - x1) * 180 / LOCAL_PI;
	// push result	
	lua_pushnumber(L, result);
	// number of results
	return 1;
}

static int l_point_distance(lua_State* L) 
{
	// get arguments
	const double x1 = luaL_checknumber(L, 1);
	const double y1 = luaL_checknumber(L, 2);
	const double x2 = luaL_checknumber(L, 3);
	const double y2 = luaL_checknumber(L, 4);
	const double dx = x2 - x1;
	const double dy = y2 - y1;
	// push result	
	lua_pushnumber(L, std::sqrt(dx * dx + dy * dy));
	// number of results
	return 1;
}

static int l_dot_product(lua_State* L) 
{
	const double x1 = luaL_checknumber(L, 1);
	const double y1 = luaL_checknumber(L, 2);
	const double x2 = luaL_checknumber(L, 3);
	const double y2 = luaL_checknumber(L, 4);
	const double result = x1 * x2 + y1 * y2;
	lua_pushnumber(L, result);
	return 1;
}

static int l_lengthdir_x(lua_State* L) 
{
	const double len = luaL_checknumber(L, 1);
	const double dir = luaL_checknumber(L, 2) * LOCAL_PI / 180.0;
	const double result = len * std::cos(dir);
	lua_pushnumber(L, result);
	return 1;
}

static int l_lengthdir_y(lua_State* L) 
{
	const double len = luaL_checknumber(L, 1);
	const double dir = luaL_checknumber(L, 2) * LOCAL_PI / 180.0;
	const double result = len * std::sin(dir);
	lua_pushnumber(L, result);
	return 1;
}

static int l_choose(lua_State* L) 
{
	const int num_args = lua_gettop(L);
	std::uniform_int_distribution<int> range(1, num_args);
	const int choice = range(get_random_engine());
	lua_pushvalue(L, choice);
	return 1;
}

static int l_random_range(lua_State* L)
{
	const double r1 = luaL_checknumber(L, 1);
	const double r2 = luaL_checknumber(L, 2);
	std::uniform_real_distribution<double> range(r1, r2);
	const double result = range(get_random_engine());
	lua_pushnumber(L, result);
	return 1;
}

static int l_irandom_range(lua_State* L)
{
	const int r1 = luaL_checkinteger(L, 1);
	const int r2 = luaL_checkinteger(L, 2);
	std::uniform_int_distribution<int> range(r1, r2);
	const int result = range(get_random_engine());
	lua_pushinteger(L, result);
	return 1;
}
static int l_random(lua_State* L)
{
	const double upper_bound = luaL_checknumber(L, 1);
	std::uniform_real_distribution<double> range(0, upper_bound);
	const double result = range(get_random_engine());
	lua_pushnumber(L, result);
	return 1;
}

static int l_irandom(lua_State* L)
{
	const int upper_bound = luaL_checkinteger(L, 1);
	std::uniform_int_distribution<int> range(0, upper_bound);
	const int result = range(get_random_engine());
	lua_pushinteger(L, result);
	return 1;
}

static const struct luaL_Reg test_lib [] = 
{
	{"point_direction", l_point_direction},
	{"point_distance", l_point_distance},
	{"dot_product", l_dot_product},
	{"lengthdir_x", l_lengthdir_x},
	{"lengthdir_y", l_lengthdir_y},
	{"choose", l_choose},
	{"random_range", l_random_range},
	{"irandom_range", l_irandom_range},
	{"random", l_random},
	{"irandom", l_irandom},
	{NULL, NULL}
};


int lua_test()
{
	lua_State *L;
	L = luaL_newstate();
	luaL_openlibs(L);
	
	// Load the defined C functions into global table.
	lua_pushglobaltable(L);
	lua_pushvalue(L, -2);
	luaL_setfuncs(L, test_lib, 1);
	lua_pop(L, 1);

	luaL_loadstring(L, g_script.c_str());
	lua_newtable(L);
	for(int n = 1; n <= 5; n++) {
		lua_pushnumber(L, n);		// Push the table index
		lua_pushnumber(L, n * 2);	// Push the cell value
		lua_rawset(L, -3);			// Stores the pair in the table
	}
	lua_setglobal(L, "foo");
	int result = lua_pcall(L, 0, LUA_MULTRET, 0);
	if(result) {
		std::cerr << "Failed to run script: " << lua_tostring(L, -1) << "\n";
		return 0;
	}
	double sum = lua_tonumber(L, -1);
	std::cout << "Script returned: " << sum << "\n";
	lua_pop(L, 1);
	lua_close(L);
	return 1;
}

struct component;
typedef std::vector<component*> component_list;

class entity
{
public:
	entity() : components_(registered_components_.size(), nullptr) {}
	static size_t register_component(const std::string& name) {
		auto res = registered_components_.size();
		registered_components_.emplace_back(name);
		return res;
	}
private:
	component_list components_;
	static std::vector<std::string> registered_components_;
};

std::vector<std::string> entity::registered_components_;

struct component_registrar
{
	explicit component_registrar(const std::string& name) : id(entity::register_component(name)) {}
	size_t id;
};

struct component
{
	virtual ~component();
	unsigned id;
};

struct position : public component
{
	position() : x(0), y(0) {}
	int x;
	int y;	
};
component_registrar pos_component("position");

struct velocity : public component
{
	velocity() : angle(0), speed(0), vspeed(0), hspeed(0) {}
	float angle;
	float speed;
	// vspeed = speed * sin(angle) and is thus the vertical component of speed.
	float vspeed;
	// hspeed = speed * cos(angle) and is thus the horizontal component of speed.
	float hspeed;
};
component_registrar vel_component("velocity");

struct sprite : public component
{
	sprite() : sprite_id(0) {}
	unsigned sprite_id;
};

class system
{

};

int main()
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 3;
	settings.minorVersion = 2;

	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);

	GLenum err = glewInit();
	if(err != GLEW_OK) {
		std::cerr << "Error: " << glewGetErrorString(err) << "\n";
		return -1;
	}

	// deactivate its OpenGL context
	window.setActive(false);

	sf::ContextSettings current_settings = window.getSettings();
	std::cout << "depth bits:" << current_settings.depthBits << std::endl;
	std::cout << "stencil bits:" << current_settings.stencilBits << std::endl;
	std::cout << "antialiasing level:" << current_settings.antialiasingLevel << std::endl;
	std::cout << "version:" << current_settings.majorVersion << "." << current_settings.minorVersion << std::endl;

	lua_test();

	sf::Thread thread(&renderingThread, &window);
	thread.launch();

	bool running = true;
	while(running)	{
		sf::Event event;
		while(window.pollEvent(event)) {
			if(event.type == sf::Event::Closed) {
				running = false;
			} else if(event.type == sf::Event::Resized) {
				glViewport(0, 0, event.size.width, event.size.height);
			} else if(event.type == sf::Event::KeyPressed) {
				lua_test();
			}
		}
	}
	window.close();

	return 0;
}
