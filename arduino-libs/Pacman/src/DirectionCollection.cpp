#include "DirectionCollection.h"
#include "Animation.h"

namespace pacman {

Animation * DirectionCollection::GetDirFromInt(int p_index)
{
	Animation* ptr = nullptr;

	switch (p_index) {
	case 0:
		ptr = Up;
		break;
	case 1:
		ptr = Down;
		break;
	case 2:
		ptr = Left;
		break;
	case 3:
		ptr = Right;
		break;
	}

	return ptr;
}

Animation * DirectionCollection::GetDirFromVect(Vect2 p_dir)
{
	if (p_dir == Vect2::UP) {
		return Up;
	}
	if (p_dir == Vect2::DOWN) {
		return Down;
	}
	if (p_dir == Vect2::LEFT) {
		return Left;
	}
	if (p_dir == Vect2::RIGHT) {
		return Right;
	}
	return nullptr;
}

DirectionCollection::DirectionCollection()
{
	Up = new Animation();
	Down = new Animation();
	Left = new Animation();
	Right = new Animation();
}

DirectionCollection::~DirectionCollection()
{
}

void DirectionCollection::DeletePointers()
{
	delete Up;
	delete Down;
	delete Left;
	delete Right;
}

}; // namespace pacman
