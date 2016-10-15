// Includes all the SFMl libraries, statically where ever possible.

#pragma once

#ifdef _MSC_VER
#ifdef _DEBUG
#    pragma comment(lib, "sfml-graphics-s-d.lib")
#    pragma comment(lib, "sfml-window-s-d.lib")
#    pragma comment(lib, "sfml-system-s-d.lib")
#    pragma comment(lib, "sfml-audio-s-d.lib")
#    pragma comment(lib, "sfml-network-s-d.lib")
#else
#    pragma comment(lib, "sfml-graphics-s.lib")
#    pragma comment(lib, "sfml-window-s.lib")
#    pragma comment(lib, "sfml-system-s.lib")
#    pragma comment(lib, "sfml-audio-s.lib")
#    pragma comment(lib, "sfml-network-s.lib")
#endif // _DEBUG
#    pragma comment(lib, "opengl32.lib")
#    pragma comment(lib, "freetype.lib")
#    pragma comment(lib, "jpeg.lib")

#    pragma comment(lib, "winmm.lib")
#    pragma comment(lib, "ws2_32.lib")

#    pragma comment(lib, "openal32.lib")
#    pragma comment(lib, "flac.lib")
#    pragma comment(lib, "vorbisenc.lib")
#    pragma comment(lib, "vorbisfile.lib")
#    pragma comment(lib, "vorbis.lib")
#    pragma comment(lib, "ogg.lib")

#    pragma comment(lib, "glew32s.lib")
#endif // _MSC_VER

