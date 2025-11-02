#include "Managers/2DActor.h"
#include <Utils/MemoryMonitor.h>

ActorTwoD::ActorTwoD(){
	 entity = nullptr;
	 SpriteShader = nullptr;
	 HitboxShader = nullptr;
	 sprite_Mesh = nullptr;
	 sprite_Renderer = nullptr;
	 animator = nullptr;
}

ActorTwoD::~ActorTwoD(){
	OnDestroy();
}

bool ActorTwoD::OnCreate(const char* FileName, int rows, int columns)
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

	sprite_Renderer = new SpriteRenderer();
	if(rows == NULL || columns == NULL)
		sprite_Renderer->loadImage(FileName);
	else {
		sprite_Renderer->loadImage(FileName, rows, columns);
		animator = new Animator();
	}

	entity = new Entity();
	entity->OnCreate(sprite_Renderer);

	return true;
}

void ActorTwoD::OnDestroy(){
	 delete entity;
	 entity = nullptr;
	 SpriteShader->OnDestroy();
	 delete SpriteShader;
	 SpriteShader = nullptr;
	 HitboxShader->OnDestroy();
	 delete HitboxShader;
	 HitboxShader = nullptr;
	 sprite_Mesh->OnDestroy();
	 delete sprite_Mesh;
	 sprite_Mesh = nullptr;
	 delete sprite_Renderer;
	 sprite_Renderer = nullptr;
	 delete animator;
	 animator = nullptr;
}

void ActorTwoD::Update(const float deltaTime_){
	animator->update(deltaTime_);
}

void ActorTwoD::Render(Matrix4 viewMatrix, Matrix4 projectionMatrix) const{
	if (draw_Hitbox) {
		entity->DrawHitBox(projectionMatrix, viewMatrix, sprite_Mesh);
	}
	if (sprite_Renderer->GetColumns() != NULL) {
		glUseProgram(SpriteShader->GetProgram());
		glUniformMatrix4fv(static_cast<GLint>(SpriteShader->GetUniformID("projectionMatrix")), 1, GL_FALSE, projectionMatrix);
		glUniformMatrix4fv(static_cast<GLint>(SpriteShader->GetUniformID("viewMatrix")), 1, GL_FALSE, viewMatrix);
		sprite_Renderer->renderSpriteSheet(SpriteShader, sprite_Mesh, entity->GetModelMatrix(), animator->getCurrentClip()->getCurrentFrame());
	}
	else {
		glUseProgram(SpriteShader->GetProgram());
		glUniformMatrix4fv(static_cast<GLint>(SpriteShader->GetUniformID("projectionMatrix")), 1, GL_FALSE, projectionMatrix);
		glUniformMatrix4fv(static_cast<GLint>(SpriteShader->GetUniformID("viewMatrix")), 1, GL_FALSE, viewMatrix);
		sprite_Renderer->renderSprite(SpriteShader, sprite_Mesh, entity->GetModelMatrix());
	}

}

void ActorTwoD::AddClip(const std::string& name, const AnimationClip* clip) const{
	animator->addAnimationClip(name, const_cast<AnimationClip*>(clip));
}

void ActorTwoD::ReBuildAll(const char* FileName, int rows, int columns){
	if (rows != NULL && columns != NULL) {
		sprite_Renderer->loadImage(FileName, rows, columns);
		entity->CreateHitBox(sprite_Renderer, animator->getCurrentClip()->getCurrentFrame());
	}
	else {
		sprite_Renderer->loadImage(FileName);
		entity->CreateHitBox(sprite_Renderer);
	}
}
