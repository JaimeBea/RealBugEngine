#pragma once

enum class UpdateStatus;
struct Event;
class Module {
public:
	virtual ~Module();

	virtual bool Init();
	virtual bool Start();
	virtual UpdateStatus PreUpdate();
	virtual UpdateStatus Update();
	virtual UpdateStatus PostUpdate();
	virtual bool CleanUp();
	virtual void RecieveEvent(const Event& e);
};
