#include "ResourceStateMachine.h"
#include "Application.h"
#include "Transition.h"
#include "States.h"

#include "Resources/ResourceClip.h"
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
#define JSON_TAG_ID "Id"

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
	assert(document.IsObject());

	const rapidjson::Value& clipsArray = document[JSON_TAG_CLIPS].GetArray();
	assert(clipsArray.IsArray());
	for (rapidjson::Value::ConstValueIterator itr = clipsArray.Begin(); itr != clipsArray.End(); ++itr){
		UID clipUID = itr->GetUint64();
		ResourceClip* clip = App->resources->GetResource<ResourceClip>(clipUID);
		App->resources->IncreaseReferenceCount(clipUID);
		clipsUids.push_back(clipUID);
	}

	std::unordered_map<UID, States> stateMap;
	for (auto const& p : document[JSON_TAG_STATES].GetArray()) {
			UID id = p[JSON_TAG_ID].GetUint64();
			std::string name = p[JSON_TAG_NAME].GetString();
			UID clipId = p[JSON_TAG_CLIP_ID].GetUint64();
			States state =  States(name, clipId, 0, id);
			states.push_back(state);
			stateMap.insert(std::make_pair(id, state));

	}

	for (auto const& p : document[JSON_TAG_TRANSITIONS].GetArray()) {
		std::string triggerName = p[JSON_TAG_TRIGGER_NAME].GetString();
		UID id = p[JSON_TAG_ID].GetUint64();
		UID source = p[JSON_TAG_SOURCE].GetUint64();
		UID target = p[JSON_TAG_TARGET].GetUint64();
		float interpolationDuration = p[JSON_TAG_INTERPOLATION_DURATION].GetFloat();
		Transition transition = Transition(stateMap.find(source)->second, stateMap.find(target)->second, interpolationDuration,id);
		transitions.insert(std::make_pair(triggerName, transition));
	}	

	unsigned timeMs = timer.Stop();
	LOG("Material loaded in %ums", timeMs);
}

void ResourceStateMachine::Unload() {
	std::list<UID>::iterator itClip;
	for (itClip = clipsUids.begin(); itClip != clipsUids.end(); ++itClip) {
		App->resources->DecreaseReferenceCount((*itClip));
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

	// Saving Clips UIDs
	rapidjson::Value clipsArrayJson(rapidjson::kArrayType);
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	std::list<UID>::iterator itClip;
	for (itClip = clipsUids.begin(); itClip != clipsUids.end(); ++itClip) {		
		rapidjson::Value id;
		id.SetUint64((*itClip));
		clipsArrayJson.PushBack(id, allocator);
	}
	document.AddMember(JSON_TAG_CLIPS, clipsArrayJson, allocator);

	// Saving States
	rapidjson::Value statesArrayJson(rapidjson::kArrayType);
	std::list<States>::iterator itState;
	for (itState = states.begin(); itState != states.end(); ++itState) {		
		rapidjson::Value objValue(rapidjson::kObjectType);
		rapidjson::Value name((*itState).name.c_str(), allocator);
		objValue.AddMember(JSON_TAG_ID, (*itState).id, allocator);
		objValue.AddMember(JSON_TAG_NAME, name, allocator);
		objValue.AddMember(JSON_TAG_CLIP_ID, (*itState).clipUid, allocator);

		statesArrayJson.PushBack(objValue, allocator);
	}
	document.AddMember(JSON_TAG_STATES, statesArrayJson, allocator);
	
	//Saving transitions
	rapidjson::Value transitionsArrayJson(rapidjson::kArrayType);
	for (const auto &element : transitions) {
		rapidjson::Value objValue(rapidjson::kObjectType);
		rapidjson::Value triggerName(element.first.c_str(), allocator);
		objValue.AddMember(JSON_TAG_TRIGGER_NAME, triggerName, allocator);
		objValue.AddMember(JSON_TAG_ID, element.second.id, allocator);
		objValue.AddMember(JSON_TAG_SOURCE, element.second.source.id, allocator);
		objValue.AddMember(JSON_TAG_TARGET, element.second.target.id, allocator);
		objValue.AddMember(JSON_TAG_INTERPOLATION_DURATION, element.second.interpolationDuration, allocator);

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

States ResourceStateMachine::AddState(const std::string &name, UID clipUID) {
	States state = States(name, clipUID);	
	states.push_back(state);
	AddClip(clipUID);

	return state;
}

void ResourceStateMachine::AddClip(UID clipUid) {
	if (!(std::find(clipsUids.begin(), clipsUids.end(), clipUid) != clipsUids.end())) {
		clipsUids.push_back(clipUid);
	}
}

void ResourceStateMachine::AddTransition(const States& from, const States& to, const float interpolation, const std::string& name) {
	//Checking for unique name
	Transition* transition = FindTransitionGivenName(name);
	if (transition) {
		return;
	}

	transitions.insert(std::make_pair(name, Transition(from, to, interpolation)));
}

Transition* ResourceStateMachine::FindTransitionGivenName(const std::string& name) {
	if (transitions.empty()) {
		return nullptr;
	}

	std::unordered_map<std::string, Transition>::iterator it = transitions.find(name);
	if (it != transitions.end()) {
		return &(*it).second;
	} 

	return nullptr;
}
