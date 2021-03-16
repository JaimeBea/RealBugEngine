#include "AnimationControllerImporter.h"

#include "Application.h"
#include "Utils/Logging.h"
#include "Utils/Buffer.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleFiles.h"
#include "FileSystem/AnimationImporter.h"

AnimationController* AnimationControllerImporter::CreateAnimationCotrollerBase() {
	// Timer to measure creating animation controller base
	MSTimer timer;
	timer.Start();

	AnimationController* controller = App->resources->ObtainAnimationController();

	std::string filePath = std::string(ANIMATION_CONTROLLERS_PATH) + "/" + controller->fileName + ANIMATION_CONTROLLER_EXTENSION;

	controller->fileName = filePath;

	unsigned bufferSize = sizeof(bool) + sizeof(unsigned);
	Buffer<char> buffer = Buffer<char>(bufferSize);
	char* cursor = buffer.Data();

	*((bool*) cursor) = controller->loop;
	cursor += sizeof(bool);

	*((unsigned*) cursor) = 0;
	cursor += sizeof(unsigned);

	LOG("Saving animation controller to \"%s\".", filePath.c_str());
	App->files->Save(filePath.c_str(), buffer);

	unsigned timeMs = timer.Stop();
	LOG("Animation Controller created in %ums", timeMs);
	return controller;
}

void AnimationControllerImporter::LoadAnimationController(AnimationController* animationController) {
	if (animationController == nullptr) return;

	// Timer to measure loading a Animation Controller
	MSTimer timer;
	timer.Start();

	std::string filePath = animationController->fileName;

	LOG("Loading animation controller from path: \"%s\".", filePath.c_str());

	// Load file
	Buffer<char> buffer = App->files->Load(filePath.c_str());
	char* cursor = buffer.Data();

	animationController->loop = *((bool*) cursor);
	cursor += sizeof(bool);

	unsigned sizeName = *((unsigned*) cursor);
	cursor += sizeof(unsigned);

	unsigned bufferNameSize = sizeof(char) * sizeName;

	char* name = (char*) malloc(bufferNameSize + 1);
	memcpy_s(name, bufferNameSize, cursor, bufferNameSize);
	cursor += bufferNameSize;

	name[sizeName] = '\0';

	if (sizeName > 0) {
		if (animationController->animationResource == nullptr) animationController->animationResource = App->resources->animations.Obtain();
		animationController->animationResource->fileName = name;
		AnimationImporter::LoadAnimation(animationController->animationResource);
	}

	free(name);

	animationController->currentTime = 0.0f;
	animationController->running = false;

	unsigned timeMs = timer.Stop();
	LOG("Animation Controller loaded in %ums", timeMs);
}

void AnimationControllerImporter::SaveAnimationController(AnimationController* animationController) {
	MSTimer timer;
	timer.Start();

	LOG("Saving animation controller...");

	unsigned sizeName = 0;

	if (animationController->animationResource != nullptr) {
		sizeName = animationController->animationResource->fileName.size();
		AnimationImporter::SaveAnimation(animationController->animationResource);
	}

	unsigned bufferSize = sizeof(bool) + sizeof(unsigned) + (sizeof(char) * sizeName);

	Buffer<char> buffer(bufferSize);
	char* cursor = buffer.Data();

	*((bool*) cursor) = animationController->loop;
	cursor += sizeof(bool);

	*((unsigned*) cursor) = sizeName;
	cursor += sizeof(unsigned);

	if (sizeName > 0) {
		memcpy_s(cursor, sizeName * sizeof(char), animationController->animationResource->fileName.data(), sizeName * sizeof(char));
		cursor += sizeName * sizeof(char);
	}

	LOG("Saving animation controller to \"%s\".", animationController->fileName.c_str());
	App->files->Save(animationController->fileName.c_str(), buffer);

	unsigned timeMs = timer.Stop();
	LOG("Animation Controller saved in %ums", timeMs);
}
