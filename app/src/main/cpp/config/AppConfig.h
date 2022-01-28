#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "globals.h"
#include "GLUtils.h"
#include "render/ModelObj.h"
#include "3rdparty/json/json.hpp"

using json = nlohmann::json;

#include <string>

struct Parameters
{
	std::string face_image;
	std::string hair_selection;
	std::string hair_texture;
	std::string hair_obj;
	int enable_wireframe;
	float ratio_width;
	float ratio_height;
	float ratio_length;
	float pitch;
	float yaw;
	float roll;
	float topheadx;
	float topheady;
	float topheadz;
	int front_vert_index;
};

class AppConfig
{
public:
	void LoadConfig(std::string file_path, std::string params);
	Parameters params;
};

#endif