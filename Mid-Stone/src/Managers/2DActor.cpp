#include "Managers/2DActor.h"

ActorTwoD::ActorTwoD(){

}

ActorTwoD::~ActorTwoD(){

}

bool ActorTwoD::OnCreate()
{
	SpriteShader = new Shader("shaders/spriteVert.glsl", "shaders/spriteFrag.glsl");
	if (!SpriteShader->OnCreate())
	{
		std::cout << "Sprite Shader failed ... we have a problem\n";
		return false;
	}
	HitboxShader = new Shader("shaders/HitboxVert.glsl", "shaders/HitboxFrag.glsl");
	if (!HitboxShader->OnCreate())
	{
		std::cout << "Hitbox Shader failed ... we have a problem\n";
		return false;
	}
	sprite_Mesh = new SpriteMesh();
	sprite_Mesh->OnCreate();
	animator = new Animator();
	return true;
}

void ActorTwoD::OnDestroy(){

}

void ActorTwoD::Update(const float deltaTime_){
	animator->update(deltaTime_);
}

void ActorTwoD::Render(Matrix4 viewMatrix, Matrix4 projectionMatrix) const{

}
