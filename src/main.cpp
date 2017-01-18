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

void gl_render()
{
	static bool init = false;
	static GLuint VertexArrayID = -1;
	static GLuint vertexbuffer = -1;
	const std::vector<GLfloat> g_vertex_buffer_data{
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f,  1.0f, 0.0f,
	};


	if(!init) {
		glGenVertexArrays(1, &VertexArrayID);

		// Generate 1 buffer, put the resulting identifier in vertexbuffer
		glGenBuffers(1, &vertexbuffer);
		// The following commands will talk about our 'vertexbuffer' buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		// Give our vertices to OpenGL.
		glBufferData(GL_ARRAY_BUFFER, g_vertex_buffer_data.size() * sizeof(GLfloat), g_vertex_buffer_data.data(), GL_STATIC_DRAW);

		init = true;
	}
	glBindVertexArray(VertexArrayID);

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
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void renderingThread(sf::RenderWindow* window)
{
	// activate the window's context
	window->setActive(true);

	sf::Texture tex;

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("../data/fonts/UbuntuMono-R.ttf", 18);
	//io.Fonts->GetTexDataAsRGBA32();

	sf::Color bgColor;
	float color[3]{ 0.f, 0.f, 0.f };

	ImGui::SFML::Init(*window);

	/*	
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	*/

	/*
	// This will identify our vertex buffer
	GLuint vertexbuffer;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data.data(), GL_STATIC_DRAW);
	*/
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	sf::Clock deltaClock;
	// the rendering loop
	while(window->isOpen()) {
		/*
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
		*/

		ImGui::SFML::Update(*window, deltaClock.restart());
		//ImGui::SetWindowFontScale(2.0f);
		ImGui::Begin("Hello, world!");
		ImGui::Button("Look at this pretty button");
		if (ImGui::ColorEdit3("Background color", color)) {
			// this code gets called if color value changes, so
			// the background color is upgraded automatically!
			bgColor.r = static_cast<sf::Uint8>(color[0] * 255.f);
			bgColor.g = static_cast<sf::Uint8>(color[1] * 255.f);
			bgColor.b = static_cast<sf::Uint8>(color[2] * 255.f);
		}
		ImGui::End();

		window->clear(bgColor);
		//window->draw(shape);

		gl_render();
		window->resetGLStates();


		ImGui::Render();

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

/*int lua_test()
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
}*/


//////////////////////////////////////////////////////////////////////////////
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
	virtual ~component() {}
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

struct lua_component : public component
{
	lua_component() : table() {}
	std::map<std::string, boost::variant<bool,int,std::string>> table;
};

typedef std::vector<int> entity_list;

class ecs_system
{
public:
	virtual ~ecs_system() {}
	virtual void start() {}
	virtual void stop() {}
	virtual void update(double t, const entity_list& elist) = 0;
private:
};

class lua_system : public ecs_system
{
public:
	explicit lua_system(std::function<void(double, std::vector<std::pair<int,int>>)> fn)
		: fn_(fn)
	{}
private:
	std::function<void(double, std::vector<std::pair<int,int>>)> fn_;
	void update(double t, const entity_list& elist) override final
	{
		std::vector<std::pair<int, int>> new_elist(elist.size());
		std::transform(elist.begin(), elist.end(), new_elist.begin(), [](int n){ return std::make_pair(n, n); });
		fn_(t, new_elist);
	}
};

//////////////////////////////////////////////////////////////////////////////

static double ex_point_direction(double x1, double y1, double x2, double y2) 
{
	return atan2(y2 - y1, x2 - x1) * 180 / LOCAL_PI;
}

static double ex_point_distance(double x1, double y1, double x2, double y2) 
{
	// get arguments
	const double dx = x2 - x1;
	const double dy = y2 - y1;
	// push result	
	return std::sqrt(dx * dx + dy * dy);
}

static double ex_dot_product(double x1, double y1, double x2, double y2) 
{
	return x1 * x2 + y1 * y2;
}

static double ex_lengthdir_x(double len, double dir) 
{
	return len * std::cos(dir * LOCAL_PI / 180.0);
}

static double ex_lengthdir_y(double len, double dir) 
{
	return len * std::sin(dir * LOCAL_PI / 180.0);
}

typedef typename boost::make_recursive_variant
<
	std::string,
	double,
	bool,
	std::vector<std::pair<
	boost::variant<std::string,double,bool>,
	boost::recursive_variant_
	>>
	>::type
	AnyValue;

/*static AnyValue ex_choose() 
{
	int num_args = 0;
	//const int num_args = lua_gettop(L);
	std::uniform_int_distribution<int> range(1, num_args);
	const int choice = range(get_random_engine());
	//lua_pushvalue(L, choice);
	//return 1;
}*/

static double ex_random_range(double r1, double r2)
{
	std::uniform_real_distribution<double> range(r1, r2);
	return range(get_random_engine());
}

static int ex_irandom_range(int r1, int r2)
{
	std::uniform_int_distribution<int> range(r1, r2);
	return range(get_random_engine());
}
static double ex_random(double upper_bound)
{
	std::uniform_real_distribution<double> range(0, upper_bound);
	return range(get_random_engine());
}

static int ex_irandom(int upper_bound)
{
	std::uniform_int_distribution<int> range(0, upper_bound);
	return range(get_random_engine());
}

std::string lua_file{"\
	local component_from_lua = {\
		attack_type = \"slash\",\
		attack_damage = 0,\
		chance_to_hit = 100\
	}\
	\
	function lua_sys1(n, elist)\
		io.write(\"n = \", n)\
		for i = 1, #elist do\
			print(i, elist[i])\
		end\
	end\
	register_component(\"component_from_lua\", component_from_lua)\
	register_system(\"lua_sys1\", lua_sys1)\
"};

int lua_test()
{
	LuaContext context;
	context.writeFunction("register_component", [](const std::string& cname, const std::map<std::string, boost::variant<bool,int,std::string>>& table) {  
		// XXX
		component_registrar lua_comp(cname);
		lua_component xxx;
		xxx.table = table;

		std::cout << "component name: " << cname << "\n";
		for(const auto& p : table) {

			switch(p.second.which()) {
				case 0: {
					bool pi = boost::get<bool>(p.second);
					std::cout << "\t" << p.first << "(bool) : " << (pi ? true : false);
					break;
				}
				case 1: {
					int pi = boost::get<int>(p.second);
					std::cout << "\t" << p.first << "(int) : " << pi;
					break;
				}
				case 2: {
					std::string pi = boost::get<std::string>(p.second);
					std::cout << "\t" << p.first << "(str) : " << pi;
					break;
				}
			}
			std::cout << "\n";
		}
	});

	typedef std::vector<std::unique_ptr<ecs_system>> system_list;
	system_list active_system_list;

	context.writeFunction("register_system", [&active_system_list](const std::string& cname, std::function<void(double, std::vector<std::pair<int,int>>)> fn) {  
		// XXX
		active_system_list.emplace_back(std::make_unique<lua_system>(fn));
		std::cout << "XXX: active_system_list.size() = " << active_system_list.size() << "\n";
	});

	std::map<int, int> foo;
	for(int i = 1; i != 6; ++i) {
		foo[i] = i * 2;
	}
	context.writeVariable("foo", foo);

	context.writeFunction("point_direction", ex_point_direction);
	context.writeFunction("point_distance", ex_point_distance);
	context.writeFunction("dot_product", ex_dot_product);
	context.writeFunction("lengthdir_x", ex_lengthdir_x);
	context.writeFunction("lengthdir_y", ex_lengthdir_y);
	context.writeFunction("random_range", ex_random_range);
	context.writeFunction("irandom_range", ex_irandom_range);
	context.writeFunction("random", ex_random);
	context.writeFunction("irandom", ex_irandom);

	// Need to push the choose function on conventionally.
	auto ls = context.state();
	lua_pushcfunction(ls, l_choose);
	lua_setglobal(ls, "choose");
	

	try {
		context.executeCode(lua_file);
		double sum = context.executeCode<double>(g_script);
		std::cout << "Script returned: " << sum << "\n";
	} catch(std::runtime_error& e) {
		std::cerr << e.what();
		return 0;
	}

	entity_list elist{ 1, 2, 3, 4, 5 };
	for(auto& sys : active_system_list) {
		sys->update(0.0, elist);
	}


	context.writeVariable("a", "hello");
	auto s = context.readVariable<std::string>("a");
	assert(s == "hello");
	return 1;
}

int main()
{
#if defined(_MSC_VER) && _WIN32_WINNT >= 0x0600
	SetProcessDPIAware();
#endif

	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 3;
	settings.minorVersion = 2;

	sf::RenderWindow window(sf::VideoMode(1024, 768), "SFML works!", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

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

	locator::manager service_locator_manager;

	lua_test();

	sf::Thread thread(&renderingThread, &window);
	thread.launch();

	bool running = true;
	while(running)	{
		sf::Event event;
		ImGui::SFML::ProcessEvent(event);
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

	thread.wait();

	ImGui::SFML::Shutdown();

	return 0;
}
