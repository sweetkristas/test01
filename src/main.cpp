#include "stdafx.h"

#include "win_sfml_lib.h"


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
"io.write(\"Returning data back to C\\n\");\n"
"return x\n"
};

int lua_test()
{
	lua_State *L;
	L = luaL_newstate();
	luaL_openlibs(L);
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
			}
		}
	}
	window.close();

	return 0;
}
