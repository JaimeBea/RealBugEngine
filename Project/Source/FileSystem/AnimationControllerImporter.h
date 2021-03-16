#pragma once

class AnimationController;

namespace AnimationControllerImporter {

	AnimationController* CreateAnimationCotrollerBase();

	void LoadAnimationController(AnimationController* animationController);
	void SaveAnimationController(AnimationController* animationController);

}; // namespace AnimationControllerImporter
