#include "AppConfig.h"

void AppConfig::LoadConfig(std::string file_path, std::string selected_params)
{
	std::string configstext = get_file_contents(file_path.c_str());
	json configsJSON = json::parse(configstext);

	std::string hair_obj = selected_params;

	params.hair_obj = std::string(configsJSON["hairs"][hair_obj]["path"]);
	params.hair_texture = std::string(configsJSON["hairs"][hair_obj]["texture"]);
	params.ratio_width = configsJSON["hairs"][hair_obj]["transformations"]["ratio_width"];
	params.ratio_height = configsJSON["hairs"][hair_obj]["transformations"]["ratio_height"];
	params.ratio_length = configsJSON["hairs"][hair_obj]["transformations"]["ratio_length"];
	params.pitch = configsJSON["hairs"][hair_obj]["transformations"]["pitch"];
	params.yaw = configsJSON["hairs"][hair_obj]["transformations"]["yaw"];
	params.roll = configsJSON["hairs"][hair_obj]["transformations"]["roll"];
	params.topheadx = configsJSON["hairs"][hair_obj]["transformations"]["topheadx"];
	params.topheady = configsJSON["hairs"][hair_obj]["transformations"]["topheady"];
	params.topheadz = configsJSON["hairs"][hair_obj]["transformations"]["topheadz"];
	params.front_vert_index = configsJSON["hairs"][hair_obj]["transformations"]["front_vert_index"];
}