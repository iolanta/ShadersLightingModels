#version 430

out vec4 color;
uniform bool use_texture;
uniform vec4 warmColor;
uniform vec4 coolColor;
uniform float diffuseCool;
uniform float diffuseWarm;

// параметры точечного источника освещения
uniform vec4 light_position;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;
uniform vec3 light_attenuation;

// параметры материала
uniform sampler2D material_texture;
uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform vec4 material_emission;
uniform float material_shininess;

// параметры, полученные из вершинного шейдера
in vec2 vert_texcoord;   // текстурные координаты
in vec3 vert_normal;     // нормаль
in vec3 vert_lightDir;   // направление на источник освещения
in vec3 vert_viewDir;    // направление от вершины к наблюдателю
in float vert_distance;  // расстояние от вершины до источника освещения
in vec3 vert_color;

void main(){
	vec3 normal = normalize(vert_normal);
	vec3 lightDir = normalize(vert_lightDir);
	vec3 viewDir = normalize(vert_viewDir);
	
	vec4 kCool = min( coolColor + material_diffuse * diffuseWarm, 1.0);
	vec4  kWarm  = min ( warmColor + diffuseWarm * material_diffuse, 1.0 );
    vec4  kFinal = mix ( kCool, kWarm, dot(normal, lightDir) );
	
	vec3 ref = reflect(-lightDir, normal);
	float spec = pow ( max ( dot ( ref, viewDir ), 0.0 ), material_shininess );
	
	color = min (kFinal + spec * material_specular, 1.0) ;
	if(use_texture)
		color *= texture(material_texture, vert_texcoord);
}