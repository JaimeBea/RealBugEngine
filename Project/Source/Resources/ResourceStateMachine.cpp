#include "ResourceStateMachine.h"
#include "ResourceTransition.h"
#include "ResourceStates.h"
#include "Resources/Clip.h"

#include "Application.h"

#include "Modules/ModuleFiles.h"
#include "FileSystem/JsonValue.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleResources.h"

#include "rapidjson/error/en.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

#include "Utils/Logging.h"
#include "Utils/Buffer.h"
#include "Utils/Leaks.h"

#define JSON_TAG_STATES "States"
#define JSON_TAG_CLIPS "Cips"
#define JSON_TAG_TRANSITIONS "Transitions"


void ResourceStateMachine::SaveToFile(const char* filePath) {
	LOG("Saving material to path: \"%s\".", filePath);

	MSTimer timer;
	timer.Start();

	// Create document
	rapidjson::Document document;
	JsonValue jStateMachine(document, document);

	// Save JSON values
	document.SetObject();
	rapidjson::Value myArray(rapidjson::kArrayType);
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	std::list<Clip*>::iterator it;
	for (it = clips.begin(); it != clips.end(); ++it) {		
		rapidjson::Value objValue(rapidjson::kObjectType);
		rapidjson::Value name((*it)->name.c_str(), allocator);
		objValue.AddMember("name", name, allocator);
		objValue.AddMember("animationUID", (*it)->animationUID, allocator);
		objValue.AddMember("beginIndex", (*it)->beginIndex, allocator);
		objValue.AddMember("endIndex", (*it)->endIndex, allocator);
		objValue.AddMember("loop", (*it)->loop, allocator);

		myArray.PushBack(objValue, allocator);
	}
	document.AddMember("clips", myArray, allocator);

	// Write document to buffer
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer, rapidjson::UTF8<>, rapidjson::UTF8<>, rapidjson::CrtAllocator, rapidjson::kWriteNanAndInfFlag> writer(stringBuffer);
	document.Accept(writer);

	// Save to file
	bool saved = App->files->Save(filePath, stringBuffer.GetString(), stringBuffer.GetSize());
	if (!saved) {
		LOG("Failed to save material resource.");
		return;
	}

	unsigned timeMs = timer.Stop();
	LOG("Material saved in %ums", timeMs);
}

ResourceStates* ResourceStateMachine::AddState(std::string name, Clip* clip) {
	//Checking for unique name
	ResourceTransition* transition = FindTransitionGivenName(name);
	if (transition) {
		return nullptr;
	}

	ResourceStates *state = new ResourceStates(name,clip);
	//Setting default state
	if (states.size() == 0) {
		currentState = state;
	}

	states.push_back(state);

	AddClip(clip);

	return state;
}

void ResourceStateMachine::AddClip(Clip* clip) {
	if (!(std::find(clips.begin(), clips.end(), clip) != clips.end())) {
		clips.push_back(clip);
	}
}

void ResourceStateMachine::AddTransition(ResourceStates* from, ResourceStates* to, float interpolation, std::string& name) {
	//Checking for unique name
	ResourceTransition* transition = FindTransitionGivenName(name);
	if (transition) {
		return;
	}

	transition = new ResourceTransition(from, to, name, interpolation);
	transitions.insert(std::make_pair(name,transition));
}


ResourceTransition* ResourceStateMachine::GetValidTransition(std::string& transitionName) {

	if (transitions.empty()) {
		return nullptr;
	}	

	return FindTransitionGivenName(transitionName);	
}

ResourceTransition* ResourceStateMachine::FindTransitionGivenName(std::string& name) {
	std::unordered_map<std::string, ResourceTransition*>::const_iterator it = transitions.find(name);

	if (it != transitions.end()) {
		return it->second;
	} 

	return nullptr;
}

ResourceStates* ResourceStateMachine::GetCurrentState() {
	return currentState;
}
