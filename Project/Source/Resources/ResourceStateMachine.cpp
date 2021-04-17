#include "ResourceStateMachine.h"
#include "ResourceTransition.h"
#include "ResourceStates.h"
#include "Resources/ResourceClip.h"

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

#define JSON_TAG_CLIPS "Clips"
#define JSON_TAG_STATES "States"
#define JSON_TAG_TRANSITIONS "Transitions"

#define JSON_TAG_CLIP_ID "ClipId"

#define JSON_TAG_NAME "Name"
#define JSON_TAG_ANIMATION_UID "AnimationUID"
#define JSON_TAG_BEGIN_INDEX "BeginIndex"
#define JSON_TAG_END_INDEX "EndIndex"
#define JSON_TAG_CLIP_UID "ClipUID"
#define JSON_TAG_LOOP "Loop"
#define JSON_TAG_ID "Id"

#define JSON_TAG_CURRENTTIME "CurrentTime"
#define JSON_TAG_SOURCE "Source"
#define JSON_TAG_TARGET "Target"
#define JSON_TAG_TRIGGER_NAME "Trigger"
#define JSON_TAG_INTERPOLATION_DURATION "Interpolation"


void ResourceStateMachine::Load() {
	// Timer to measure loading a material
	MSTimer timer;
	timer.Start();
	std::string filePath = GetResourceFilePath();
	LOG("Loading material from path: \"%s\".", filePath.c_str());

	Buffer<char> buffer = App->files->Load(filePath.c_str());
	if (buffer.Size() == 0) return;

	rapidjson::Document document;
	document.ParseInsitu<rapidjson::kParseNanAndInfFlag>(buffer.Data());
	if (document.HasParseError()) {
		LOG("Error parsing JSON: %s (offset: %u)", rapidjson::GetParseError_En(document.GetParseError()), document.GetErrorOffset());
		return;
	}
	JsonValue jStateMachine(document, document);

	document.SetObject();

	std::unordered_map<UID, ResourceClip*> clipsMap;
	/*const rapidjson::Value& clipsArray = document[JSON_TAG_CLIPS];
	assert(clipsArray.IsArray());
	for (rapidjson::Value::ConstValueIterator itr = clipsArray.Begin(); itr != clipsArray.End(); ++itr){
		UID clipUID = itr->GetUint();
		ResourceClip* clip = (ResourceClip*) App->resources->GetResource(clipUID);
		App->resources->IncreaseReferenceCount(clipUID);
		clips.push_back(clip);
		clipsMap.insert(std::make_pair(clipUID, clip));
	}*/

	for (auto& p : document[JSON_TAG_CLIPS].GetArray()) {
		UID clipUID = p.GetUint();
		ResourceClip* clip = (ResourceClip*) App->resources->GetResource(clipUID);
		App->resources->IncreaseReferenceCount(clipUID);
		clips.push_back(clip);
		clipsMap.insert(std::make_pair(clipUID, clip));
	}

	std::unordered_map<UID, ResourceStates*> stateMap;
	for (auto const& p : document[JSON_TAG_STATES].GetArray()) {
			UID id = p[JSON_TAG_ID].GetUint64();
			std::string name = p[JSON_TAG_NAME].GetString();
			UID clipId = p[JSON_TAG_CLIP_ID].GetUint64();
			float currentTime = p[JSON_TAG_CURRENTTIME].GetFloat();
			ResourceStates* state = new ResourceStates(name, clipsMap.find(clipId)->second, currentTime);
			states.push_back(state);
			stateMap.insert(std::make_pair(id, state));

	}

	for (auto const& p : document[JSON_TAG_TRANSITIONS].GetArray()) {
		std::string triggerName = p[JSON_TAG_TRIGGER_NAME].GetString();
		UID id = p[JSON_TAG_ID].GetUint64();
		UID source = p[JSON_TAG_SOURCE].GetUint64();
		UID target = p[JSON_TAG_TARGET].GetUint64();
		float interpolationDuration = p[JSON_TAG_INTERPOLATION_DURATION].GetFloat();
		ResourceTransition* transition = new ResourceTransition(stateMap.find(source)->second, stateMap.find(target)->second, interpolationDuration,id);
		transitions.insert(std::make_pair(triggerName, transition));
	}	

	unsigned timeMs = timer.Stop();
	LOG("Material loaded in %ums", timeMs);
}

void ResourceStateMachine::Unload() {
	std::list<ResourceClip*>::iterator itClip;
	for (itClip = clips.begin(); itClip != clips.end(); ++itClip) {
		App->resources->DecreaseReferenceCount((*itClip)->GetId());
	}
}

void ResourceStateMachine::SaveToFile(const char* filePath) {
	LOG("Saving ResourceStateMachine to path: \"%s\".", filePath);

	MSTimer timer;
	timer.Start();
	
	// Create document
	rapidjson::Document document;
	JsonValue jStateMachine(document, document);

	// Save JSON values
	document.SetObject();

	rapidjson::Value clipsArrayJson(rapidjson::kArrayType);
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	std::list<ResourceClip*>::iterator itClip;
	for (itClip = clips.begin(); itClip != clips.end(); ++itClip) {		
		clipsArrayJson.PushBack( (*itClip)->GetId(), allocator);
		(*itClip)->SaveToFile((*itClip)->GetResourceFilePath().c_str());
	}
	document.AddMember(JSON_TAG_CLIPS, clipsArrayJson, allocator);

	rapidjson::Value statesArrayJson(rapidjson::kArrayType);
	std::list<ResourceStates*>::iterator itState;
	for (itState = states.begin(); itState != states.end(); ++itState) {		
		rapidjson::Value objValue(rapidjson::kObjectType);
		rapidjson::Value name((*itState)->name.c_str(), allocator);
		objValue.AddMember(JSON_TAG_ID, (*itState)->id, allocator);
		objValue.AddMember(JSON_TAG_NAME, name, allocator);
		objValue.AddMember(JSON_TAG_CLIP_ID, (*itState)->clip->GetId(), allocator);
		objValue.AddMember(JSON_TAG_CURRENTTIME, (*itState)->currentTime, allocator);

		statesArrayJson.PushBack(objValue, allocator);
	}
	document.AddMember(JSON_TAG_STATES, statesArrayJson, allocator);
	
	//Saving transitions
	rapidjson::Value transitionsArrayJson(rapidjson::kArrayType);
	for (const auto &element : transitions) {
		rapidjson::Value objValue(rapidjson::kObjectType);
		rapidjson::Value triggerName(element.first.c_str(), allocator);
		objValue.AddMember(JSON_TAG_TRIGGER_NAME, triggerName, allocator);
		objValue.AddMember(JSON_TAG_ID, element.second->id, allocator);
		objValue.AddMember(JSON_TAG_SOURCE, element.second->source->id, allocator);
		objValue.AddMember(JSON_TAG_TARGET, element.second->target->id, allocator);
		objValue.AddMember(JSON_TAG_INTERPOLATION_DURATION, element.second->interpolationDuration, allocator);

		transitionsArrayJson.PushBack(objValue, allocator);
	}
	document.AddMember(JSON_TAG_TRANSITIONS, transitionsArrayJson, allocator);


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

ResourceStates* ResourceStateMachine::AddState(std::string name, ResourceClip* clip) {
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

void ResourceStateMachine::AddClip(ResourceClip* clip) {
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

	transition = new ResourceTransition(from, to, interpolation);
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
