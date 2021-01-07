#include "ModuleResources.h"

#include "Globals.h"
#include "Application.h"
#include "Logging.h"
#include "ModuleFiles.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"
#include <string>

#include "Leaks.h"

bool ModuleResources::Init()
{
	textures.Allocate(100);
	cube_maps.Allocate(10);
	meshes.Allocate(1000);

	ilInit();
	iluInit();

	return true;
}

bool ModuleResources::CleanUp()
{
	for (Texture& texture : textures)
	{
		UnloadTexture(&texture);
	}

	for (CubeMap& cube_map : cube_maps)
	{
		UnloadCubeMap(&cube_map);
	}

	for (Mesh& mesh : meshes)
	{
		UnloadMesh(&mesh);
	}

	return true;
}

Texture* ModuleResources::ImportTexture(const char* file_path)
{
	LOG("Importing texture from path: \"%s\".", file_path);

	// Generate image handler
	unsigned image;
	ilGenImages(1, &image);
	DEFER
	{
		ilDeleteImages(1, &image);
	};

	// Load image
	ilBindImage(image);
	bool image_loaded = ilLoadImage(file_path);
	if (!image_loaded)
	{
		LOG("Failed to load image.");
		return nullptr;
	}
	bool image_converted = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
	if (!image_converted)
	{
		LOG("Failed to convert image.");
		return nullptr;
	}

	// Flip image if neccessary
	ILinfo info;
	iluGetImageInfo(&info);
	if (info.Origin == IL_ORIGIN_UPPER_LEFT)
	{
		iluFlipImage();
	}

	// Create texture
	Texture* texture = textures.Obtain();

	// Save texture to custom DDS file
	texture->file_id = GenerateUID();
	std::string file_name = std::to_string(texture->file_id);
	std::string dds_file_path = std::string(TEXTURES_PATH) + file_name + TEXTURE_EXTENSION;

	LOG("Saving image to \"%s\".", dds_file_path.c_str());
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	size_t size = ilSaveL(IL_DDS, nullptr, 0);
	if (size == 0)
	{
		LOG("Failed to save image.");
		return nullptr;
	}
	Buffer<char> buffer = Buffer<char>(size);
	size = ilSaveL(IL_DDS, buffer.Data(), size);
	if (size == 0)
	{
		LOG("Failed to save image.");
		return nullptr;
	}
	App->files->Save(dds_file_path.c_str(), buffer);

	LOG("Texture imported successfuly.");

	return texture;
}

Texture* ModuleResources::ObtainTexture()
{
	return textures.Obtain();
}

void ModuleResources::LoadTexture(Texture* texture)
{
	if (texture == nullptr) return;

	std::string file_name = std::to_string(texture->file_id);
	std::string file_path = std::string(TEXTURES_PATH) + file_name + TEXTURE_EXTENSION;

	LOG("Loading texture from path: \"%s\".", file_path.c_str());

	// Generate image handler
	unsigned image;
	ilGenImages(1, &image);
	DEFER
	{
		ilDeleteImages(1, &image);
	};

	// Load image
	ilBindImage(image);
	bool image_loaded = ilLoad(IL_DDS, file_path.c_str());
	if (!image_loaded)
	{
		LOG("Failed to load image.");
		return;
	}

	// Generate texture from image
	glGenTextures(1, &texture->gl_texture);
	glBindTexture(GL_TEXTURE_2D, texture->gl_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

	// Generate mipmaps and set filtering and wrapping
	glGenerateMipmap(GL_TEXTURE_2D);
	SetWrap(texture_wrap);
	SetMinFilter(min_filter);
	SetMagFilter(mag_filter);

	LOG("Texture loaded successfuly.");
}

void ModuleResources::UnloadTexture(Texture* texture)
{
	if (!texture->gl_texture) return;

	glDeleteTextures(1, &texture->gl_texture);
}

void ModuleResources::ReleaseTexture(Texture* texture)
{
	UnloadTexture(texture);
	textures.Release(texture);
}

CubeMap* ModuleResources::ImportCubeMap(const char* file_paths[6])
{
	// Create cube map
	CubeMap* cube_map = cube_maps.Obtain();

	for (unsigned i = 0; i < 6; ++i)
	{
		const char* file_path = file_paths[i];

		LOG("Importing cube map texture from path: \"%s\".", file_path);

		// Generate image handler
		unsigned image;
		ilGenImages(1, &image);
		DEFER
		{
			ilDeleteImages(1, &image);
		};

		// Load image
		ilBindImage(image);
		bool image_loaded = ilLoadImage(file_path);
		if (!image_loaded)
		{
			LOG("Failed to load image.");
			return nullptr;
		}
		bool image_converted = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
		if (!image_converted)
		{
			LOG("Failed to convert image.");
			return nullptr;
		}

		// Flip image if neccessary
		ILinfo info;
		iluGetImageInfo(&info);
		if (info.Origin != IL_ORIGIN_UPPER_LEFT) // Cube maps are the inverse of textures
		{
			iluFlipImage();
		}

		// Save texture to custom DDS file
		cube_map->file_ids[i] = GenerateUID();
		std::string file_name = std::to_string(cube_map->file_ids[i]);
		std::string dds_file_path = std::string(TEXTURES_PATH) + file_name + TEXTURE_EXTENSION;

		LOG("Saving image to \"%s\".", dds_file_path.c_str());
		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
		size_t size = ilSaveL(IL_DDS, nullptr, 0);
		if (size == 0)
		{
			LOG("Failed to save image.");
			return nullptr;
		}
		Buffer<char> buffer = Buffer<char>(size);
		size = ilSaveL(IL_DDS, buffer.Data(), size);
		if (size == 0)
		{
			LOG("Failed to save image.");
			return nullptr;
		}
		App->files->Save(dds_file_path.c_str(), buffer);

		LOG("Cube map texture imported successfuly.");
	}

	return cube_map;
}

CubeMap* ModuleResources::ObtainCubeMap()
{
	return cube_maps.Obtain();
}

void ModuleResources::LoadCubeMap(CubeMap* cube_map)
{
	// Create texture handle
	glGenTextures(1, &cube_map->gl_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map->gl_texture);

	// Load cube map
	for (unsigned i = 0; i < 6; ++i)
	{
		std::string file_name = std::to_string(cube_map->file_ids[i]);
		std::string file_path = std::string(TEXTURES_PATH) + file_name + TEXTURE_EXTENSION;

		LOG("Loading cubemap texture from path: \"%s\".", file_path.c_str());

		// Generate image handler
		unsigned image;
		ilGenImages(1, &image);
		DEFER
		{
			ilDeleteImages(1, &image);
		};

		// Load image
		ilBindImage(image);
		bool image_loaded = ilLoad(IL_DDS, file_path.c_str());
		if (!image_loaded)
		{
			LOG("Failed to load image.");
			return;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
	}

	// Set filtering and wrapping
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void ModuleResources::UnloadCubeMap(CubeMap* cube_map)
{
	if (!cube_map->gl_texture) return;

	glDeleteTextures(1, &cube_map->gl_texture);
}

void ModuleResources::ReleaseCubeMap(CubeMap* cube_map)
{
	UnloadCubeMap(cube_map);
	cube_maps.Release(cube_map);
}

Mesh* ModuleResources::ObtainMesh()
{
	return meshes.Obtain();
}

void ModuleResources::LoadMesh(Mesh* mesh)
{
	if (mesh == nullptr) return;

	std::string file_name = std::to_string(mesh->file_id);
	std::string file_path = std::string(MESHES_PATH) + file_name + MESH_EXTENSION;

	LOG("Loading mesh from path: \"%s\".", file_path.c_str());

	// Load file
	Buffer<char> buffer = App->files->Load(file_path.c_str());
	char* cursor = buffer.Data();

	// Header
	mesh->num_vertices = *((unsigned*) cursor);
	cursor += sizeof(unsigned);
	mesh->num_indices = *((unsigned*) cursor);
	cursor += sizeof(unsigned);

	unsigned position_size = sizeof(float) * 3;
	unsigned normal_size = sizeof(float) * 3;
	unsigned uv_size = sizeof(float) * 2;
	unsigned index_size = sizeof(unsigned);

	unsigned vertex_size = position_size + normal_size + uv_size;
	unsigned vertex_buffer_size = vertex_size * mesh->num_vertices;
	unsigned index_buffer_size = index_size * mesh->num_indices;

	// Vertices
	float* vertices = (float*) cursor;
	cursor += vertex_size * mesh->num_vertices;

	// Indices
	unsigned* indices = (unsigned*) cursor;

	LOG("Loading %i vertices...", mesh->num_vertices);

	// Create VAO
	glGenVertexArrays(1, &mesh->vao);
	glGenBuffers(1, &mesh->vbo);
	glGenBuffers(1, &mesh->ebo);

	glBindVertexArray(mesh->vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);

	// Load VBO
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size, vertices, GL_STATIC_DRAW);

	// Load EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_size, indices, GL_STATIC_DRAW);

	// Load vertex attributes
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*) 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*) position_size);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertex_size, (void*) (position_size + normal_size));

	// Unbind VAO
	glBindVertexArray(0);
}

void ModuleResources::UnloadMesh(Mesh* mesh)
{
	if (!mesh->vao) return;

	glDeleteVertexArrays(1, &mesh->vao);
	glDeleteBuffers(1, &mesh->vbo);
	glDeleteBuffers(1, &mesh->ebo);
}

void ModuleResources::ReleaseMesh(Mesh* mesh)
{
	UnloadMesh(mesh);
	meshes.Release(mesh);
}

void ModuleResources::SetMinFilter(TextureMinFilter filter)
{
	for (Texture& texture : textures)
	{
		glBindTexture(GL_TEXTURE_2D, texture.gl_texture);
		switch (filter)
		{
		case TextureMinFilter::NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			break;
		case TextureMinFilter::LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			break;
		case TextureMinFilter::NEAREST_MIPMAP_NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			break;
		case TextureMinFilter::LINEAR_MIPMAP_NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			break;
		case TextureMinFilter::NEAREST_MIPMAP_LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			break;
		case TextureMinFilter::LINEAR_MIPMAP_LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			break;
		}
	}

	min_filter = filter;
}

void ModuleResources::SetMagFilter(TextureMagFilter filter)
{
	for (Texture& texture : textures)
	{
		glBindTexture(GL_TEXTURE_2D, texture.gl_texture);
		switch (filter)
		{
		case TextureMagFilter::NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;
		case TextureMagFilter::LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		}
	}

	mag_filter = filter;
}

void ModuleResources::SetWrap(TextureWrap wrap)
{
	for (Texture& texture : textures)
	{
		glBindTexture(GL_TEXTURE_2D, texture.gl_texture);
		switch (wrap)
		{
		case TextureWrap::REPEAT:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			break;
		case TextureWrap::CLAMP_TO_EDGE:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			break;
		case TextureWrap::CLAMP_TO_BORDER:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			break;
		case TextureWrap::MIRROR_REPEAT:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			break;
		case TextureWrap::MIRROR_CLAMP_TO_EDGE:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_TO_EDGE);
			break;
		}
	}

	texture_wrap = wrap;
}

TextureMinFilter ModuleResources::GetMinFilter() const
{
	return min_filter;
}

TextureMagFilter ModuleResources::GetMagFilter() const
{
	return mag_filter;
}

TextureWrap ModuleResources::GetWrap() const
{
	return texture_wrap;
}
